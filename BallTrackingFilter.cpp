#include "BallTrackingFilter.hpp"

BallTrackingFilter::BallTrackingFilter()
{
    mKfStateLen = 6;
    mKfState = cv::Mat::zeros(mBKfStateLen, 1, CV_64F);
    mFoundCount = 0;

    // Control matrix shows how p/v/a interact for y
    mKfControlLen = 1;
    mKfControl = cv::Mat::zeros(mBKfStateLen, 1, CV_64F);
    mKfControl.at<double>(BallKFState::y) = .5;
    mKfControl.at<double>(BallKFState::dy) = 1;
    mKfControlVec = cv::Mat::eye(1, 1, CV_64F)*2;

    mKfMeasLen = 4;
    mKfMeas = cv::Mat::zeros(mBKfMeasLen, 1, CV_64F);

    mKf = KalmanFilterPlus(mBKfStateLen, mBKfMeasLen, mBKfControlLen, CV_64F);
    mKf.setAlphaSq(1.02);
    flushKalman();

}

