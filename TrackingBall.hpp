#ifndef TRACKINGBALL_HPP
#define TRACKINGBALL_HPP

#include <opencv2/core.hpp>

#include <QPoint>

class TrackingBall
{
    cv::Mat mMeasVec;
    cv::Point2f mCenter;
    float mRadius;

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
};

#endif // TRACKINGBALL_HPP
