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
 * @brief Displays a video stream and tracking data.
 *
 * Allows manipulation of
 * projector corners (where they should be in frame)
 */
class TrackVideoWidget : public QWidget
{
  Q_OBJECT

  /** A list of raw ball contour data in the frame */
  QList<TrackingBall> mBalls;
  /** A list of KF prediction data in the frame */
  QList<KFPrediction> mPreds;

public:
  explicit TrackVideoWidget(QWidget *parent = 0);

  /** Refresh the corners (ie send signal that corners changed to listeners) */
  void updateCorners();
  /** Return the list of projector corners in image coordinates */
  QList<QPoint> getCorners();

signals:
  /** Signal sent whenever the widget is resized */
  void resized(QSize size);
  /** Signal sent whenever the corners of the projector in frame change */
  void cornersChanged(std::vector<cv::Point2f>);

public slots:
  /** Set the current display image of the widget to image */
  void setImage(const QImage &image);

  /** Set all 4 corners of the projector screen (in-frame coordinates) */
  void setCorners(QList<QPoint> corners);

  /** Called when the UL corner widget is dropped at point */
  void ulCornerDropped(QPoint point);
  /** Called when the BL corner widget is dropped at point */
  void blCornerDropped(QPoint point);
  /** Called when the BR corner widget is dropped at point */
  void brCornerDropped(QPoint point);
  /** Called when the UR corner widget is dropped at point */
  void urCornerDropped(QPoint point);

  /** Push the TrackingBall ball to the list of balls */
  void pushBall(TrackingBall ball);
  /** Push the KFPrediction pred to the list of predictions */
  void pushPred(KFPrediction pred);
protected:

  /** Called on paint event; drawing logic goes here (Draw frame, points, etc) */
  void paintEvent(QPaintEvent* event);
  /** Called on resize event; */
  void resizeEvent(QResizeEvent *event);
private:
  /** UI data edited in QDesigner; see TrackVideoWidget.ui */
  Ui::trackVideoWidget ui;
  /** Current video frame to be displayed by widget */
  QImage currentFrame;

};

#endif // TRACKVIDEOWIDGET_HPP
