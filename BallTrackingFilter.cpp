#include "BallTrackingFilter.hpp"
#include <QDebug>

void BallTrackingFilter::setGravConstant(double gravConstant)
{
    mGravConstant = gravConstant;
}

BallTrackingFilter::BallTrackingFilter() :
    TrackingFilter()
{
    mKfStateLen = 6;
    mKfState = cv::Mat::zeros(mKfStateLen, 1, CV_64F);
    mFoundCount = 0;

    // Control matrix shows how p/v/a interact for y
    mKfControlLen = 1;
    mKfControl = cv::Mat::zeros(mKfStateLen, 1, CV_64F);
    mKfControl.at<double>(BallKFState::y) = .5;
    mKfControl.at<double>(BallKFState::dy) = 1;
    mKfControlVec = cv::Mat::eye(1, 1, CV_64F)*2;

    mKfMeasLen = 4;
    mKfMeas = cv::Mat::zeros(mKfMeasLen, 1, CV_64F);

    mKf = KalmanFilterPlus(mKfStateLen, mKfMeasLen, mKfControlLen, CV_64F);
    mKf.setAlphaSq(1.02);
    flushKalman();
}

void BallTrackingFilter::flushKalman()
{
    TrackingFilter::flushKalman();

    // Start with A = I
    mKf.transitionMatrix = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F);

    // We measure x, y, w, h of ball blob
    mKf.measurementMatrix = cv::Mat::zeros(mKfMeasLen, mKfStateLen, CV_64F);
    mKf.measurementMatrix.at<double>(BallKFMeas::x, BallKFState::x) = 1;
    mKf.measurementMatrix.at<double>(BallKFMeas::y, BallKFState::y) = 1;
    mKf.measurementMatrix.at<double>(BallKFMeas::w, BallKFState::w) = 1;
    mKf.measurementMatrix.at<double>(BallKFMeas::h, BallKFState::h) = 1;

    mKf.controlMatrix = mKfControl.clone();

    mKf.errorCovPre = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F)*1000;
    mKf.errorCovPost = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F)*1000;

    mKf.errorCovPre.at<double>(BallKFState::w, BallKFState::w) = 0;
    mKf.errorCovPre.at<double>(BallKFState::h, BallKFState::h) = 0;
    mKf.errorCovPost.at<double>(BallKFState::w, BallKFState::w) = 0;
    mKf.errorCovPost.at<double>(BallKFState::h, BallKFState::h) = 0;

    mKf.measurementNoiseCov = cv::Mat::eye(mKfMeasLen, mKfMeasLen, CV_64F)*1;
    mKf.measurementNoiseCov.at<double>(BallKFMeas::w, BallKFMeas::w) = 1e5;
    mKf.measurementNoiseCov.at<double>(BallKFMeas::h, BallKFMeas::h) = 1e5;

    mKf.processNoiseCov = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F)*1e-7;

    mKfState = cv::Mat::zeros(mKfStateLen, 1, CV_64F);
    mKfState.at<double>(BallKFState::w) = 10;
    mKfState.at<double>(BallKFState::h) = 10;
}

void BallTrackingFilter::setXYCovariance(double sigma)
{
    mKf.measurementNoiseCov.at<double>(BallKFMeas::x, BallKFMeas::x) = exp(sigma);
    mKf.measurementNoiseCov.at<double>(BallKFMeas::y, BallKFMeas::y) = exp(sigma);
}

void BallTrackingFilter::updateTrackFailure()
{
    mNotFoundCount++;

    if (isLost()) {
        mFoundCount = 0;
    } else {
        mKf.statePost = mKfState;
    }
}

void BallTrackingFilter::updateTrackSuccess(TrackingBall ball)
{
    mKfMeas = ball.getMeas();

    if (!isFound() || mKfState.at<double>(BallKFState::y) > 1000) {
        mKf.errorCovPost = cv::Mat::eye(mKfStateLen, mKfStateLen, CV_64F)*1000;

        mKfState.at<double>(BallKFState::x) = mKfMeas.at<double>(BallKFMeas::x);
        mKfState.at<double>(BallKFState::y) = mKfMeas.at<double>(BallKFMeas::y);
        mKfState.at<double>(BallKFState::w) = mKfMeas.at<double>(BallKFMeas::w);
        mKfState.at<double>(BallKFState::h) = mKfMeas.at<double>(BallKFMeas::h);
        mKfState.at<double>(BallKFState::dx) = 0;
        mKfState.at<double>(BallKFState::dy) = 0;

        mKf.statePost = mKfState;
    } else {
        mKf.correct(mKfMeas.t());
    }

    mNotFoundCount = 0;
    mFoundCount++;
}

void BallTrackingFilter::updateTimeState(double t)
{
    TrackingFilter::updateTimeState(t);

    mKf.transitionMatrix.at<double>(BallKFState::x, BallKFState::dx) = dT();
    mKf.transitionMatrix.at<double>(BallKFState::y, BallKFState::dy) = dT();

    mKfState = mKf.predict(mKfControlVec*mGravConstant*dT());

    qDebug() << mTstart << " " << t;
    qDebug() << dT();

    // << "\n" << mKf.transitionMatrix;
    //std::cout << mKf.controlMatrix << "\n";
    //std::cout << mKf.transitionMatrix << "\n----\n";
    //std::cout << mKfState << "\n";
}


