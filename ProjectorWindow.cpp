#include "ProjectorWindow.hpp"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

#include "Util.hpp"

ProjectorWindow::ProjectorWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setStyleSheet("* {background-color: rgb(0,0,0);}");

    mProjSize = QSize(50, 50);
}

void ProjectorWindow::pushBall(TrackingBall ball)
{
    mBalls.append(ball);
    if (mBalls.size() > 10) {
        mBalls.removeFirst();
    }

}

void ProjectorWindow::pushPred(KFPrediction pred)
{
    mPreds.append(KFPrediction(pred));
    if (mPreds.size() > 10) {
        mPreds.removeFirst();
    }

    polynomialFitKF(3, mPreds, mFitParabola);

    update();//(pred.bbox());
}

QRectF ProjectorWindow::relRectToWindow(QRectF rect) {
    return QRectF(QPointF(rect.left()*width()/mProjSize.width(),
                          rect.top()*height()/mProjSize.height()),
                  QPointF(rect.right()*width()/mProjSize.width(),
                          rect.bottom()*height()/mProjSize.height()));
}

QPointF ProjectorWindow::relPointToWindow(QPointF pt) {
    return QPointF(pt.x()*width()/mProjSize.width(),
                   pt.y()*height()/mProjSize.height());
}

void ProjectorWindow::paintEvent(QPaintEvent* ev)
{
    QPainter painter;
    QPen ballPen = QPen((QColor(0, 0, 255)));
    QPen kfSeenPen = QPen((QColor(255,0,0)));
    QPen kfMissPen = QPen((QColor(255,255,0)));

    painter.begin(this);

    QBrush background = QBrush(QColor(0,0,0));
    painter.fillRect(ev->rect(), background);

    painter.setPen(ballPen);
    painter.drawRect(QRect(10, 10, 40, 40));

    QList<TrackingBall>::const_iterator ballsIter;
    for (ballsIter = mBalls.begin(); ballsIter != mBalls.end(); ++ballsIter) {
        painter.drawEllipse(
                    (QPoint)(*ballsIter).center()*2, (int)(*ballsIter).r()*2, (int)(*ballsIter).r()*2);
    }

    QList<KFPrediction>::const_iterator predsIter;
    for (predsIter = mPreds.begin(); predsIter != mPreds.end(); ++predsIter) {
        if ((*predsIter).seen()) {
            painter.setPen(kfSeenPen);
        } else {
            painter.setPen(kfMissPen);
        }
        //qDebug() << (*predsIter).bbox();
        painter.drawRect(relRectToWindow((*predsIter).bbox()));
        painter.drawLine(relPointToWindow((*predsIter).bbox().center()),
                         relPointToWindow(
                             (*predsIter).bbox().center()+
                             (QPointF)(*predsIter).jet()*(*predsIter).dt()));
    }

    QPointF p1(0, mFitParabola[0]);
    QPointF p2(mProjSize.width(),
               mFitParabola[2]*mProjSize.width()*mProjSize.width() +
            mFitParabola[1]*mProjSize.width() + mFitParabola[0]);
    QPointF c(0.5*mProjSize.width(), mFitParabola[0] + .5*mProjSize.width()*mFitParabola[1]);
    QPainterPath fitPath(relPointToWindow(p1));
    fitPath.quadTo(relPointToWindow(c), relPointToWindow(p2));
    painter.drawPath(fitPath);

    painter.end();
}

