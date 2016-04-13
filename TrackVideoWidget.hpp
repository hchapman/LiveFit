#ifndef TRACKVIDEOWIDGET_H
#define TRACKVIDEOWIDGET_H

#define UL_CORNER 0
#define BL_CORNER 1
#define BR_CORNER 2
#define UR_CORNER 3

#include "TrackingBall.hpp"
#include "ui_TrackVideoWidget.h"
#include <opencv2/core.hpp>
#include <QWidget>

class TrackVideoWidget : public QWidget
{
    Q_OBJECT
    QList<TrackingBall> mBalls;

public:
    explicit TrackVideoWidget(QWidget *parent = 0);

signals:
    void resized(QSize size);
    void cornersChanged(std::vector<cv::Point2f>);

public slots:
    void setImage(const QImage &image);
    void pushBall(TrackingBall ball);

    void ulCornerDropped(QPoint point);
    void blCornerDropped(QPoint point);
    void brCornerDropped(QPoint point);
    void urCornerDropped(QPoint point);

protected:
    void updateCorners();

    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent *event);
private:
    Ui::trackVideoWidget ui;
    QImage currentFrame;

};

#endif // TRACKVIDEOWIDGET_HPP
