#include "TrackingFilter.hpp"

TrackingFilter::TrackingFilter()
{
    flushKalman();
}

void TrackingFilter::flushKalman()
{

}

double TrackingFilter::kalmanDistance(cv::Mat measurement)
{
    cv::Mat M = (measurement - (mKf.measurementMatrix*mKf.statePre).t());
    cv::Mat Sigma = (mKf.measurementMatrix
                     *mKf.errorCovPre
                     *mKf.measurementMatrix.t() +
                     mKf.measurementNoiseCov);
    return ((cv::Mat)(M*(Sigma.inv()*M.t()))).at<double>(0,0) + log(cv::determinant(Sigma));
}

void TrackingFilter::updateTrackFailure()
{

}

cv::Mat TrackingFilter::prediction()
{
    return mKf.statePost;
}

cv::Mat TrackingFilter::covariance()
{
    return mKf.errorCovPost;
}

void TrackingFilter::updateTimeState(double t)
{
    mTstart = mTstop;
    mTstop = t;
}

bool TrackingFilter::isLost()
{
    return mNotFoundCount >= 4;
}

bool TrackingFilter::isFound()
{
    return mFoundCount > 0;
}
