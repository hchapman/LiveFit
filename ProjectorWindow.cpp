#include "ProjectorWindow.hpp"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTextDocument>

#include "Util.hpp"

void ProjectorWindow::toggleShowParabola(bool showParabola)
{
    mShowParabola = showParabola;
    update();
}

void ProjectorWindow::setPointRadius(int pointRadius)
{
    mPointRadius = pointRadius;
    update();
}

void ProjectorWindow::setPointThickness(double pointThickness)
{
    mPointThickness = pointThickness;
    update();
}

void ProjectorWindow::setFitThickness(double fitThickness)
{
    mFitThickness = fitThickness;
    update();
}

void ProjectorWindow::setMinFallSpeed(int minFallSpeed)
{
    mMinFallSpeed = minFallSpeed;
}

void ProjectorWindow::toggleWaitTilFall(bool waitTilFall)
{
    mWaitTilFall = waitTilFall;
}

void ProjectorWindow::setFontSize(int fontSize)
{
    mFontSize = fontSize;
    update();
}

ProjectorWindow::ProjectorWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    // Set the background color of this widget to black (i.e. projector off)
    setStyleSheet("* {background-color: rgb(0,0,0);}");

    // Set the size of the projector screen, in inches
    mProjSize = QSize(50, 50);

    // Lock to the fit after 10 points, until next sight
    mNumPointFit = 10;
    mFitPoints = true;

    mPointRadius = 1;
    mPointThickness = 0.;
    mFitThickness = 0.;
    mShowJet = true;
    mShowParabola = true;
    mShowParam = true;

    mWaitTilFall = true;
    mMinFallSpeed = 5;
}

void ProjectorWindow::pushBall(TrackingBall ball)
{
    // Add the spotted ball to the list of balls
    mBalls.append(ball);

    // If the history is too large, remove old balls from display
    if (mBalls.size() > 10) {
        mBalls.removeFirst();
    }

}

static bool comparePredY(KFPrediction a, KFPrediction b) {
    return (a.bbox().center().y() < b.bbox().center().y());
}

void fitYTPoints(QList<KFPrediction> pts, double f[3]) {
    double t,y1,t1,y2,t2,y3,t3;
    double a,b,c;
    t = pts.at(0).t();
    t1 = 0;
    y1 = pts.at(0).bbox().center().y();
    t2 = pts.at(1).t()-t;
    y2 = pts.at(1).bbox().center().y();
    t3 = pts.at(2).t()-t;
    y3 = pts.at(2).bbox().center().y();


    if (t1 == t2 || t2 == t3 || t1 == t3) {
        f[0] = 0; f[1] = 0; f[2] = 0; return;
    }

    f[2] = ((y1 - y2)/(t1 - t2) - (y2 - y3)/(t2 - t3))/(t1 - t3);
    f[1] = (y1 - y2)/(t1 - t2) - f[2]*(t1 + t2);
    f[0] = y2 - f[2]*t2*t2 - f[1]*t2;
}

void fitXTPoints(QList<KFPrediction> pts, double f[2]) {
    double t,y1,t1,y2,t2,y3,t3;
    double a,b,c;
    t = pts.at(0).t();
    t1 = 0;
    y1 = pts.at(0).bbox().center().x();
    t3 = pts.at(2).t()-t;
    y3 = pts.at(2).bbox().center().x();

    f[1] = (y1 - y3)/(t1 - t3);
    f[0] = y1 - f[1]*t1;
}

void ProjectorWindow::pushPred(KFPrediction pred)
{
    if(mDataStale) {
        mPreds.clear();
        mFitPreds.clear();
        mDataStale = false;
        mMarkedPoints.clear();
        mFitLocked = false;
    }
    // Add the prediction to the list of predictions
    mPreds.append(KFPrediction(pred));

    // Trim the viewer points if it's too long
    if (mPreds.size() > 20) {
        mPreds.removeFirst();
    }

    if (!mNumPointFit) {
        // We fit to all predictions...
        mFitPreds.append(KFPrediction(pred));

        // If the history is too large, remove old predictions
        if (mPreds.size() > 20) {
            mFitPreds.removeFirst();
        }

    } else {

        if ((mWaitTilFall && pred.jet().y()*pred.dt() < mMinFallSpeed) ||
                (mFitPreds.size() < mNumPointFit)) {
            mFitPreds.append(KFPrediction(pred));
            mFitLocked = false;
        }

        if ((!mWaitTilFall || pred.jet().y()*pred.dt() >= mMinFallSpeed) &&
                (mFitPreds.size() >= mNumPointFit)){
            mFitLocked = true;
        }

    }

    // We fit our parabola to the predictions
    if (!mFitPoints) {
        polynomialFitKFX(2, mFitPreds.at(0).t(), mFitPreds, mFitLineX);
        polynomialFitKFY(3, mFitPreds.at(0).t(), mFitPreds, mFitParabolaY);
    } else if (mFitLocked && mFitPreds.size() > 3 && mMarkedPoints.empty()) {
        mMarkedPoints.append(mFitPreds.at(0));
        mMarkedPoints.append(*std::min_element(++mFitPreds.begin(), --mFitPreds.end(),
                                               comparePredY));
        mMarkedPoints.append(mFitPreds.at(mFitPreds.size()-1));
        fitYTPoints(mMarkedPoints, mFitParabolaY);
        fitXTPoints(mMarkedPoints, mFitLineX);
    }


    // Update the window. Hopefully, we can make this more efficient...
    update();//(pred.bbox());
}

void ProjectorWindow::markDataStale() {
    // Indicate that the data we have is stale, and to be dropped.
    mDataStale = true;
}

void ProjectorWindow::toggleShowFit(bool showFit)
{
    mShowFit = showFit;
    update();
}

void ProjectorWindow::toggleShowParam(bool showParam)
{
    mShowParam = showParam;
    update();
}

void ProjectorWindow::toggleShowJet(bool showJet)
{
    mShowJet = showJet;
    update();
}

void ProjectorWindow::toggleColorMiss(bool colorMiss)
{
    mColorMiss = colorMiss;
    update();
}

void ProjectorWindow::toggleVerboseKF(bool verboseKF)
{
    mVerboseKF = verboseKF;
    update();
}

QRectF ProjectorWindow::relRectToWindow(QRectF rect) {
    // Convert a rect in projector coordinates to widget coordinates
    return QRectF(QPointF(rect.left()*width()/mProjSize.width(),
                          rect.top()*height()/mProjSize.height()),
                  QPointF(rect.right()*width()/mProjSize.width(),
                          rect.bottom()*height()/mProjSize.height()));
}

QPointF ProjectorWindow::relPointToWindow(QPointF pt) {
    // Convert a point in projector coordinates to widget coordinates
    return QPointF(pt.x()*width()/mProjSize.width(),
                   pt.y()*height()/mProjSize.height());
}

void ProjectorWindow::paintEvent(QPaintEvent* ev)
{
    // This event is called when the widget is asked to repaint itself
    // All of our custom painter logic should go in here.

    // Painter to paint the window
    QPainter painter;

    // Hold on to the normal (transparent) brush
    QBrush normalBrush;

    // List of data HTMLs
    QStringList dataHtml;

    double size = 10;

    // Pens (i.e. line drawing settings for the painter)
    QPen ballPen = QPen((QColor(0, 0, 255)));
    ballPen.setWidthF(mPointThickness);
    QPen kfSeenPen = QPen((QColor(255,0,0)));
    kfSeenPen.setWidthF(mPointThickness);
    QPen kfMissPen = QPen((QColor(255,255,0)));
    kfMissPen.setWidthF(mPointThickness);

    QPen markPen = QPen((QColor(255,255,255)));
    markPen.setWidthF(mPointThickness);

    QPen fitPen = QPen((QColor(255,0,255)));
    fitPen.setWidthF(mFitThickness);
    QPen fitLockPen = QPen((QColor(255,255,255)));
    fitLockPen.setWidthF(mFitThickness);

    // We have to begin painting. Remember to end.
    painter.begin(this);
    normalBrush = painter.brush();

    // This brush is the background color of the widget. Fill the window in case.
    QBrush background = QBrush(QColor(0,0,0));
    painter.fillRect(ev->rect(), background);

    // Draw the ball indicators on the projector screen
    painter.setPen(ballPen);
    QList<TrackingBall>::const_iterator ballsIter;
    for (ballsIter = mBalls.begin(); ballsIter != mBalls.end(); ++ballsIter) {
        painter.drawEllipse(
                    (QPoint)(*ballsIter).center()*2,
                    (int)(*ballsIter).r()*2,
                    (int)(*ballsIter).r()*2);
    }

    // Draw the KF prediction indicators on the screen
    QList<KFPrediction>::const_iterator predsIter;
    for (predsIter = mPreds.begin(); predsIter != mPreds.end(); ++predsIter) {

        // The color of the indicator depends on if the ball was actually seen on this frame or not
        if (((*predsIter).seen() || !mColorMiss)) {
            painter.setPen(kfSeenPen);
        } else {
            painter.setPen(kfMissPen);
        }

        // Draw the bounding box of the predicted ball
        if (mVerboseKF) {
            painter.drawRect(relRectToWindow((*predsIter).bbox()));
        } else {
            painter.drawEllipse(QRectF(relPointToWindow(
                                    (*predsIter).bbox().center())-QPointF(mPointRadius/2,mPointRadius/2),
                                QSizeF(mPointRadius,mPointRadius)));
        }

        if (mShowJet) {
            // Draw the predicted velocities of the predicted ball
            painter.drawLine(relPointToWindow((*predsIter).bbox().center()),
                             relPointToWindow(
                                 (*predsIter).bbox().center()+
                                 (QPointF)(*predsIter).jet()*(*predsIter).dt()));
        }
    }

    int PX, PY;
    int j; double t0;
    j = 0;

    // Draw the predicted flight trajectory of the ball
    if (mShowParam || mShowFit) {
        double a, b, A, B, C, U, V, W;

        // x(t) = at + b
        a = mFitLineX[1];
        b = mFitLineX[0];

        // y(t) = At^2 + Bt + C
        A = mFitParabolaY[2];
        B = mFitParabolaY[1];
        C = mFitParabolaY[0];

        // y(x) = Ux^2 + Vx + W
        U = A/a/a;
        V = B/a - 2*A*b/a/a;
        W = A*b*b/a/a - B*b/a + C;

        // Show the fit parabola
        if (mShowParabola) {
            if (mFitLocked) {
                painter.setPen(fitLockPen);
            } else
            {
                painter.setPen(fitPen);
            }
            QPointF p1(0, W);
            QPointF p2(mProjSize.width(),
                       U*mProjSize.width()*mProjSize.width() +
                       V*mProjSize.width() + W);
            QPointF c(0.5*mProjSize.width(), W + .5*mProjSize.width()*V);
            QPainterPath fitPath(relPointToWindow(p1));
            fitPath.quadTo(relPointToWindow(c), relPointToWindow(p2));
            painter.drawPath(fitPath);
        }


        // Draw the predicted y(t) and x(t) equation
        if (mShowParam) {
            dataHtml << QString("y(t) = %1 t<sup>2</sup> + %2 t + %3")
                        .arg(A, 0, 'f', 3)
                        .arg(B, 0, 'f', 3)
                        .arg(C, 0, 'f', 3)
                     << QString("x(t) = %1 t + %2")
                        .arg(a, 0, 'f', 3)
                        .arg(b, 0, 'f', 3);
        }
    }

    if (!mMarkedPoints.empty()) {
        t0 = mMarkedPoints.at(0).t();
    }

    //QStringList markHtml;
    QRectF markRect;
    QFont markFont = painter.font();
    markFont.setPointSize(mFontSize);
    int mMarkRadius = mPointRadius*0.8;

    // Draw marked points
    painter.setFont(markFont);
    for (predsIter = mMarkedPoints.begin(); predsIter != mMarkedPoints.end();
         ++predsIter) {
        painter.setPen(markPen);

        dataHtml << QString("%1: (%2, %3, %4)")
                    .arg(j)
                    .arg(predsIter->t()-t0, 0, 'f', 3)
                    .arg(predsIter->bbox().center().x(), 0, 'f', 3)
                    .arg(predsIter->bbox().center().y(), 0, 'f', 3);

        markRect = QRectF(relPointToWindow(
                              (*predsIter).bbox().center())-QPointF(mMarkRadius/2,mMarkRadius/2),
                          QSizeF(mMarkRadius,mMarkRadius));

        painter.setBrush(background);
        painter.drawEllipse(markRect);
        painter.setBrush(normalBrush);

        painter.drawText(markRect,
                         Qt::AlignCenter|Qt::TextDontClip,
                         QString("%1").arg(j));
        j++;
    }

    if (!dataHtml.empty()) {
        QTextDocument td;

        td.setDefaultStyleSheet(
                    QString("body {"
                            " color: rgb(255,255,255);"
                            " font-size: %1pt;"
                            "}")
                    .arg(mFontSize));
        td.setHtml(QString(
                       "<body>%1</body>")
                   .arg(dataHtml.join("<br>"))
                   );
        td.drawContents(&painter);
    }

    painter.end();
}

