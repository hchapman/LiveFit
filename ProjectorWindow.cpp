#include "ProjectorWindow.hpp"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTextDocument>

#include "Util.hpp"

ProjectorWindow::ProjectorWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    // Set the background color of this widget to black (i.e. projector off)
    setStyleSheet("* {background-color: rgb(0,0,0);}");

    // Set the size of the projector screen, in inches
    mProjSize = QSize(50, 50);
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

void ProjectorWindow::pushPred(KFPrediction pred)
{
    // Add the prediction to the list of predictions
    mPreds.append(KFPrediction(pred));

    // If the history is too large, remove old predictions
    if (mPreds.size() > 10) {
        mPreds.removeFirst();
    }

    // We fit our parabola to the predictions
    polynomialFitKFX(2, mPreds.at(0).t(), mPreds, mFitLineX);
    polynomialFitKFY(3, mPreds.at(0).t(), mPreds, mFitParabolaY);

    // Update the window. Hopefully, we can make this more efficient...
    update();//(pred.bbox());
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

    // Pens (i.e. line drawing settings for the painter)
    QPen ballPen = QPen((QColor(0, 0, 255)));
    QPen kfSeenPen = QPen((QColor(255,0,0)));
    QPen kfMissPen = QPen((QColor(255,255,0)));

    // We have to begin painting. Remember to end.
    painter.begin(this);

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
        if ((*predsIter).seen()) {
            painter.setPen(kfSeenPen);
        } else {
            painter.setPen(kfMissPen);
        }

        // Draw the bounding box of the predicted ball
        painter.drawRect(relRectToWindow((*predsIter).bbox()));

        // Draw the predicted velocities of the predicted ball
        painter.drawLine(relPointToWindow((*predsIter).bbox().center()),
                         relPointToWindow(
                             (*predsIter).bbox().center()+
                             (QPointF)(*predsIter).jet()*(*predsIter).dt()));
    }

    // Draw the predicted flight trajectory of the ball

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

    QPointF p1(0, W);
    QPointF p2(mProjSize.width(),
               U*mProjSize.width()*mProjSize.width() +
            V*mProjSize.width() + W);
    QPointF c(0.5*mProjSize.width(), W + .5*mProjSize.width()*V);
    QPainterPath fitPath(relPointToWindow(p1));
    fitPath.quadTo(relPointToWindow(c), relPointToWindow(p2));
    painter.drawPath(fitPath);

    // Draw the predicted y(x) equation
    // TODO: Decouple x, y and show x(t) and y(t)
    QTextDocument td;

    td.setDefaultStyleSheet("body {color: rgb(255,255,255); font-size: 15px;}");
    td.setHtml(QString("<body>"
                       "y(t) = %1 t<sup>2</sup> + %2 t + %3<br>"
                       "x(t) = %4 t + %5"
                       "</body>")
               .arg(A, 0, 'f', 3)
               .arg(B, 0, 'f', 3)
               .arg(C, 0, 'f', 3)
               .arg(a, 0, 'f', 3)
               .arg(b, 0, 'f', 3)
            );
    td.drawContents(&painter);

    painter.end();
}

