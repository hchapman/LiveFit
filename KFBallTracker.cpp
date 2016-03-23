#include "KFBallTracker.hpp"

KFBallTracker::KFBallTracker(QObject *parent) : QObject(parent)
{
    mKfStateLen = 6;
    mKfState = cv::Mat::zeros(mKfStateLen, 1, CV_64F);
    mFoundCount = 0;

    // Control matrix shows how p/v/a interact for y
    mKfControlLen = 1;
    mKfControl = cv::Mat::zeros(mKfStateLen, 1, CV_64F);
    mKfControl.at<double>(BallKFState::y, 1) = .5;
    mKfControl.at<double>(BallKFState::dy, 1) = 1;
    mKfControlVec = cv::Mat::eye(1, 1, CV_64F)*2;

    mKfMeasLen = 4;
    mKfMeas = cv::Mat::zeros(mKfMeasLen, 1, CV_64F);

    mKf = cv::KalmanFilter(mKfStateLen, mKfMeasLen, mKfControlLen);
    flushKalman();
}

void KFBallTracker::flushKalman()
{
    // Start with A = I
    mKf.transitionMatrix = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F);

    // We measure x, y, w, h of ball blob
    mKf.measurementMatrix = cv::Mat::zeros(mKfMeasLen, mKfStateLen, CV_64F);
    mKf.measurementMatrix.at<double>(BallKFMeas::x, BallKFMeas::x) = 1;
    mKf.measurementMatrix.at<double>(BallKFMeas::y, BallKFMeas::y) = 1;
    mKf.measurementMatrix.at<double>(BallKFMeas::w, BallKFMeas::w) = 1;
    mKf.measurementMatrix.at<double>(BallKFMeas::h, BallKFMeas::h) = 1;

    mKf.controlMatrix = mKfControl.clone();

    mKf.errorCovPre = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F)*1000;
    mKf.errorCovPost = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F)*1000;

    mKf.measurementNoiseCov = cv::Mat::eye(mKfMeasLen, mKfMeasLen, CV_64F)*1e-1;

    mKfState = cv::Mat::zeros(mKfStateLen, 1, CV_64F);

}

