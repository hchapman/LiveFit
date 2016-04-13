#include "TrackingBall.hpp"

#include <opencv2/imgproc.hpp>

TrackingBall::TrackingBall() {
    mMeasVec = cv::Mat(1, 4, CV_64F);
}

TrackingBall::TrackingBall(std::vector<cv::Point> contour)
{
    cv::Rect rect = cv::boundingRect(contour);

    mMeasVec = cv::Mat(1, 4, CV_64F);

    mMeasVec.at<double>(0) = rect.x+rect.width/2;
    mMeasVec.at<double>(1) = rect.y+rect.height/2;
    mMeasVec.at<double>(2) = rect.width;
    mMeasVec.at<double>(3) = rect.height;

    cv::minEnclosingCircle(contour, mCenter, mRadius);
}

cv::Mat TrackingBall::getMeas() const
{
    return mMeasVec;
}

double TrackingBall::x() const
{
    return mMeasVec.at<double>(0);
}

double TrackingBall::y() const
{
    return mMeasVec.at<double>(1);
}

double TrackingBall::width() const
{
    return mMeasVec.at<double>(2);
}

double TrackingBall::height() const
{
    return mMeasVec.at<double>(3);
}

double TrackingBall::r() const
{
    return mRadius;
}

QPoint TrackingBall::center() const
{
    return QPoint(mCenter.x, mCenter.y);
}

QPoint TrackingBall::tl() const
{
    return QPoint(x()-width()/2, y()-height()/2);
}
