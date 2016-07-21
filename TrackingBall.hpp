#ifndef TRACKINGBALL_HPP
#define TRACKINGBALL_HPP

#include <opencv2/core.hpp>

#include <QPoint>

/**
 * @brief The TrackingBall class
 * A ball which has been tracked
 */
class TrackingBall
{
    cv::Mat mMeasVec;
    cv::Point2f mCenter;
    float mRadius;
    cv::Rect mRect;

public:
    TrackingBall(std::vector<cv::Point> contour);
    cv::Mat getMeas() const;

    double x() const;
    double y() const;
    double width() const;
    double height() const;
    double r() const;

    QPoint center() const;
    QPoint tl() const;

    TrackingBall();
    cv::Rect rect() const;
};

#endif // TRACKINGBALL_HPP
