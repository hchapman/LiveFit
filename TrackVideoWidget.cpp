#include "TrackVideoWidget.hpp"

#include <QMouseEvent>
#include <QPainter>

TrackVideoWidget::TrackVideoWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_OpaquePaintEvent);
    updateCorners();

    connect(ui.ulCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(ulCornerDropped(QPoint)));
    connect(ui.blCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(blCornerDropped(QPoint)));
    connect(ui.brCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(brCornerDropped(QPoint)));
    connect(ui.urCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(urCornerDropped(QPoint)));
}

void TrackVideoWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    QPen ballPen = QPen((QColor(0, 0, 255)));
    QPen kfSeenPen = QPen((QColor(255,0,0)));
    QPen kfMissPen = QPen((QColor(255,255,0)));

    painter.begin(this);

    painter.drawImage(0, 0, currentFrame);

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

void TrackVideoWidget::resizeEvent(QResizeEvent* event)
{
    emit resized(size());
}

void TrackVideoWidget::setImage(const QImage& img)
{
    currentFrame = img;
    update();
}

void TrackVideoWidget::pushBall(TrackingBall ball)
{
    mBalls.append(ball);
    if (mBalls.size() > 10) {
        mBalls.removeFirst();
    }
}

void TrackVideoWidget::pushPred(KFPrediction pred) {
    mPreds.append(pred);
    if (mPreds.size() > 10) {
        mPreds.removeFirst();
    }
}

void TrackVideoWidget::ulCornerDropped(QPoint point)
{

}

void TrackVideoWidget::blCornerDropped(QPoint point)
{

}

void TrackVideoWidget::brCornerDropped(QPoint point)
{

}

void TrackVideoWidget::urCornerDropped(QPoint point)
{

}

void TrackVideoWidget::updateCorners()
{
    std::vector<cv::Point2f> corners;

    corners.push_back(cv::Point2f(ui.ulCorner->x(),ui.ulCorner->y()));
    corners.push_back(cv::Point2f(ui.blCorner->x(),ui.blCorner->y()));
    corners.push_back(cv::Point2f(ui.brCorner->x(),ui.brCorner->y()));
    corners.push_back(cv::Point2f(ui.urCorner->x(),ui.urCorner->y()));

    emit cornersChanged(corners);
}

