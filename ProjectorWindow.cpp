#include "ProjectorWindow.hpp"

#include <QPainter>

ProjectorWindow::ProjectorWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setStyleSheet("* {background-color: rgb(0,0,0);}");

    projSize = QSize(800, 600);
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
    mPreds.append(pred);
    if (mPreds.size() > 10) {
        mPreds.removeFirst();
    }
}

void ProjectorWindow::paintEvent(QPaintEvent* ev)
{
    QPainter painter;
    QPen ballPen = QPen((QColor(0, 0, 255)));
    QPen kfSeenPen = QPen((QColor(255,0,0)));
    QPen kfMissPen = QPen((QColor(255,255,0)));

    painter.begin(this);

    painter.setPen(ballPen);
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
        painter.drawRect(QRect((*predsIter).bbox().topLeft()*2,
                               (*predsIter).bbox().bottomRight()*2));
        painter.drawLine(
                    (*predsIter).bbox().center()*2,
                    ((*predsIter).bbox().center()*2+
                     (QPoint)(*predsIter).jet()*(*predsIter).dt()*2));
    }

    painter.end();
}

