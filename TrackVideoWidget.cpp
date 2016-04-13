#include "TrackVideoWidget.hpp"

#include <QMouseEvent>
#include <QPainter>

TrackVideoWidget::TrackVideoWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_OpaquePaintEvent);

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

    painter.begin(this);

    painter.drawImage(0, 0, currentFrame);

    painter.setPen(ballPen);
    QList<TrackingBall>::const_iterator iter;
    for (iter = mBalls.begin(); iter != mBalls.end(); ++iter) {
        painter.drawEllipse(
                    (QPoint)(*iter).center()*2, (int)(*iter).r()*2, (int)(*iter).r()*2);
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

