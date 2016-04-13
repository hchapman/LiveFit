#include "KFBallTracker.hpp"
#include "TrackingBall.hpp"

#include <QMap>
#include <QPoint>
#include <QVector>

#include <iostream>
#include <opencv2/highgui.hpp>

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

    mKf = cv::KalmanFilter(mKfStateLen, mKfMeasLen, mKfControlLen, CV_64F);
    flushKalman();
}

QMap<double, TrackingBall> KFBallTracker::processNextFrame(cv::Mat &frame, int t)
{
    return findBalls(frame);
}

void KFBallTracker::flushKalman()
{
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

    mKf.measurementNoiseCov = cv::Mat::eye(mKfMeasLen, mKfMeasLen, CV_64F)*1e-1;

    mKfState = cv::Mat::zeros(mKfStateLen, 1, CV_64F);
    mKfState.at<double>(BallKFState::w) = 10;
    mKfState.at<double>(BallKFState::h) = 10;

}

#define SENSITIVITY 20
#define BLUR_SIZE 5

double KFBallTracker::scoreContour(TrackingBall ball,
                                   cv::Mat &threshDiff) {
    return pow(2., kalmanDistance(ball.getMeas()));
}

double KFBallTracker::kalmanDistance(cv::Mat measurement) {
    cv::Mat M = (measurement - (mKf.measurementMatrix*mKf.statePre).t());
    cv::Mat Sigma = (mKf.measurementMatrix
                     *mKf.errorCovPre
                     *mKf.measurementMatrix.t() +
                     mKf.measurementNoiseCov);
    return ((cv::Mat)(M*(Sigma.inv()*M.t()))).at<double>(0,0) + log(cv::determinant(Sigma));
}

QMap<double, TrackingBall> KFBallTracker::findMovementThresh(cv::Mat threshDiff) {
    std::vector<std::vector<cv::Point> > contours;
    QMap<double, TrackingBall> balls;

    cv::findContours(threshDiff, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (int i=0; i<contours.size(); i++) {
        TrackingBall ball = TrackingBall(contours[i]);
        double score = scoreContour(ball, threshDiff);

        if (score > 0.1) {
            balls[score] = ball;
        }
    }

    return balls;
}

void KFBallTracker::updateTrackFailure()
{
    mNotFoundCount++;

    if (ballIsLost()) {
        mFoundCount = 0;
    } else {
        mKf.statePost = mKfState;
    }
}

void KFBallTracker::updateTrackSuccess(TrackingBall ball)
{
    mKfMeas = ball.getMeas();

    if (!ballFound()) {
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

    mFoundCount++;
}

bool KFBallTracker::ballIsLost()
{
    return mNotFoundCount >= 4;
}

bool KFBallTracker::ballFound()
{
    return mFoundCount > 0;
}

void KFBallTracker::updateTimeState(int t)
{
    mTstart = mTstop;
    mTstop = t;

    mKf.transitionMatrix.at<double>(BallKFState::x, BallKFState::dx) = dT();
    mKf.transitionMatrix.at<double>(BallKFState::y, BallKFState::dy) = dT();

    mKfState = mKf.predict(mKfControlVec);
}

QMap<double, TrackingBall> KFBallTracker::findBalls(cv::Mat &nxtFrame)
{

    mFrameHistory.append(nxtFrame);

    if (mFrameHistory.size() < 2) {
        return QMap<double, TrackingBall>();
    } else if (mFrameHistory.size() < 3) {
        return QMap<double, TrackingBall>();
    }

    cv::Mat preFrame = mFrameHistory[0];
    mFrameHistory.removeFirst();
    cv::Mat curFrame = mFrameHistory[0];

    cv::Mat preGrayFrame, curGrayFrame, nxtGrayFrame;

    cv::cvtColor(preFrame, preGrayFrame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(curFrame, curGrayFrame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(nxtFrame, nxtGrayFrame, cv::COLOR_BGR2GRAY);

    cv::Mat oldThreshDiff, newThreshDiff;

    cv::absdiff(preGrayFrame, curGrayFrame, mOldDiff);
    cv::threshold(mOldDiff, oldThreshDiff, SENSITIVITY, 255, cv::THRESH_BINARY);
    cv::blur(oldThreshDiff, oldThreshDiff, cv::Size(BLUR_SIZE, BLUR_SIZE));
    cv::threshold(oldThreshDiff, oldThreshDiff, SENSITIVITY, 255, cv::THRESH_BINARY);

    cv::Mat newDiff;
    cv::absdiff(curGrayFrame, nxtGrayFrame, newDiff);
    cv::threshold(newDiff, newThreshDiff, SENSITIVITY, 255, cv::THRESH_BINARY);
    cv::blur(newThreshDiff, newThreshDiff, cv::Size(BLUR_SIZE, BLUR_SIZE));
    cv::threshold(newThreshDiff, newThreshDiff, SENSITIVITY, 255, cv::THRESH_BINARY);

    cv::Mat threshDiff;
    cv::bitwise_and(oldThreshDiff, newThreshDiff, threshDiff);

    QMap<double, TrackingBall> balls = findMovementThresh(threshDiff);
    if (!balls.isEmpty()) {
        updateTrackSuccess(balls.first());
    } else {
        updateTrackFailure();
    }
    return balls;
}

