#ifndef TRACKVIDEOWIDGET_H
#define TRACKVIDEOWIDGET_H

#define UL_CORNER 0
#define BL_CORNER 1
#define BR_CORNER 2
#define UR_CORNER 3

#include "KFPrediction.hpp"
#include "TrackingBall.hpp"
#include "ui_TrackVideoWidget.h"
#include <opencv2/core.hpp>
#include <QWidget>

/**
 * @brief The TrackVideoWidget class
 * Displays a video stream and tracking data. Allows manipulation of
 * projector corners (where they should be in frame)
 */
class TrackVideoWidget : public QWidget
{
    Q_OBJECT
    QList<TrackingBall> mBalls;
    QList<KFPrediction> mPreds;

public:
    explicit TrackVideoWidget(QWidget *parent = 0);
    void updateCorners();
    QList<QPoint> getCorners();

signals:
    void resized(QSize size);
    void cornersChanged(std::vector<cv::Point2f>);

public slots:
    void setImage(const QImage &image);
    void pushBall(TrackingBall ball);

    void setCorners(QList<QPoint> corners);

    void ulCornerDropped(QPoint point);
    void blCornerDropped(QPoint point);
    void brCornerDropped(QPoint point);
    void urCornerDropped(QPoint point);

    void pushPred(KFPrediction pred);
protected:

    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent *event);
private:
    Ui::trackVideoWidget ui;
    QImage currentFrame;

};

#endif // TRACKVIDEOWIDGET_HPP
