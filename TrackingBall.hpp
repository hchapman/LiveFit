#ifndef TRACKINGBALL_HPP
#define TRACKINGBALL_HPP

#include <opencv2/core.hpp>

#include <QPoint>

/**
 * @brief A ball which has been tracked by the software
 */
class TrackingBall
{
  /** The measurement vector (KF spotting input) */
  cv::Mat mMeasVec;
  /** The center of the ball mass */
  cv::Point2f mCenter;
  /** The radius of the ball mass */
  float mRadius;
  /** A Rect which encompasses the ball mass */
  cv::Rect mRect;

public:
  /** Construct an empty TrackingBall */
  TrackingBall();
  /** Construct a TrackingBall from a measurement vector (usual method) */
  TrackingBall(std::vector<cv::Point> contour);

  /** Get the measurement vector */
  cv::Mat getMeas() const;

  /** Get the x coordinate of the center */
  double x() const;
  /** Get the y coordinate of the center */
  double y() const;
  /** Get the width of the bounding rect */
  double width() const;
  /** Get the height of the bounding rect */
  double height() const;
  /** Get the radius of the bounding circle */
  double r() const;

  /** Get the center of the ball mass */
  QPoint center() const;
  /** Top left corner of the bounding rect */
  QPoint tl() const;

  /** Get the bounding rect for the ball */
  cv::Rect rect() const;
};

#endif // TRACKINGBALL_HPP
