#include "KFBallTracker.hpp"
#include "KalmanFilterPlus.hpp"
#include "TrackingBall.hpp"

#include <QMap>
#include <QPoint>
#include <QVector>
#include <QDebug>

#include <iostream>
#include <opencv2/highgui.hpp>

KFBallTracker::KFBallTracker(QObject *parent) : QObject(parent)
{
    mBKfStateLen = 6;
    mBKfState = cv::Mat::zeros(mBKfStateLen, 1, CV_64F);
    mBFoundCount = 0;

    mBlurSize = 5;
    mThreshVal = 30;

    mMinRadius = 5;
    mMaxRadius = 10;

    mGravConstant = 5;

    // Control matrix shows how p/v/a interact for y
    mBKfControlLen = 1;
    mBKfControl = cv::Mat::zeros(mBKfStateLen, 1, CV_64F);
    mBKfControl.at<double>(BallKFState::y) = .5;
    mBKfControl.at<double>(BallKFState::dy) = 1;
    mBKfControlVec = cv::Mat::eye(1, 1, CV_64F)*2;

    mBKfMeasLen = 4;
    mBKfMeas = cv::Mat::zeros(mBKfMeasLen, 1, CV_64F);

    mBKf = KalmanFilterPlus(mBKfStateLen, mBKfMeasLen, mBKfControlLen, CV_64F);
    mBKf.setAlphaSq(1.02);
    flushBallKalman();
}

QMap<double, TrackingBall> KFBallTracker::processNextFrame(cv::Mat &frame, int t)
{
    mFrameHistory.append(frame.clone());

    if (mFrameHistory.size() < 2) {
        return QMap<double, TrackingBall>();
    } else if (mFrameHistory.size() < 3) {
        return QMap<double, TrackingBall>();
    }

    QList<cv::Rect> ignore = findPeople(frame);

    QMap<double, TrackingBall> res = findBalls(frame, ignore);

    mFrameHistory.removeFirst();

    return res;
}

void KFBallTracker::flushBallKalman()
{
    // Start with A = I
    mBKf.transitionMatrix = cv::Mat::eye(mBKfStateLen, mBKfStateLen, CV_64F);

    // We measure x, y, w, h of ball blob
    mBKf.measurementMatrix = cv::Mat::zeros(mBKfMeasLen, mBKfStateLen, CV_64F);
    mBKf.measurementMatrix.at<double>(BallKFMeas::x, BallKFState::x) = 1;
    mBKf.measurementMatrix.at<double>(BallKFMeas::y, BallKFState::y) = 1;
    mBKf.measurementMatrix.at<double>(BallKFMeas::w, BallKFState::w) = 1;
    mBKf.measurementMatrix.at<double>(BallKFMeas::h, BallKFState::h) = 1;

    mBKf.controlMatrix = mBKfControl.clone();

    mBKf.errorCovPre = cv::Mat::eye(mBKfStateLen, mBKfStateLen, CV_64F)*1000;
    mBKf.errorCovPost = cv::Mat::eye(mBKfStateLen, mBKfStateLen, CV_64F)*1000;

    mBKf.errorCovPre.at<double>(BallKFState::w, BallKFState::w) = 0;
    mBKf.errorCovPre.at<double>(BallKFState::h, BallKFState::h) = 0;
    mBKf.errorCovPost.at<double>(BallKFState::w, BallKFState::w) = 0;
    mBKf.errorCovPost.at<double>(BallKFState::h, BallKFState::h) = 0;

    mBKf.measurementNoiseCov = cv::Mat::eye(mBKfMeasLen, mBKfMeasLen, CV_64F)*1;
    mBKf.measurementNoiseCov.at<double>(BallKFMeas::w, BallKFMeas::w) = 1e5;
    mBKf.measurementNoiseCov.at<double>(BallKFMeas::h, BallKFMeas::h) = 1e5;

    mBKf.processNoiseCov = cv::Mat::eye(mBKfStateLen, mBKfStateLen, CV_64F)*1e-7;

    mBKfState = cv::Mat::zeros(mBKfStateLen, 1, CV_64F);
    mBKfState.at<double>(BallKFState::w) = 10;
    mBKfState.at<double>(BallKFState::h) = 10;
}

double KFBallTracker::scoreContour(TrackingBall ball,
                                   cv::Mat &threshDiff) {
    if (mBFoundCount < 2) {

    }

    return pow(2., kalmanDistance(ball.getMeas()));
}

double KFBallTracker::kalmanDistance(cv::Mat measurement) {
    cv::Mat M = (measurement - (mBKf.measurementMatrix*mBKf.statePre).t());
    cv::Mat Sigma = (mBKf.measurementMatrix
                     *mBKf.errorCovPre
                     *mBKf.measurementMatrix.t() +
                     mBKf.measurementNoiseCov);
    return ((cv::Mat)(M*(Sigma.inv()*M.t()))).at<double>(0,0) + log(cv::determinant(Sigma));
}

void KFBallTracker::setXYCovariance(double sigma)
{
    mBKf.measurementNoiseCov.at<double>(BallKFMeas::x, BallKFMeas::x) = exp(sigma);
    mBKf.measurementNoiseCov.at<double>(BallKFMeas::y, BallKFMeas::y) = exp(sigma);
}

QList<cv::Rect> KFBallTracker::findPeople(cv::Mat& nxtFrame)
{
    cv::Mat preFrame = mFrameHistory[0];
    cv::Mat curFrame = mFrameHistory[1];

    cv::Mat preGrayFrame, curGrayFrame, nxtGrayFrame;

    cv::cvtColor(preFrame, preGrayFrame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(curFrame, curGrayFrame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(nxtFrame, nxtGrayFrame, cv::COLOR_BGR2GRAY);

    cv::Mat oldThreshDiff, newThreshDiff;

    cv::absdiff(preGrayFrame, curGrayFrame, mOldDiff);
    //cv::threshold(mOldDiff, oldThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);
    cv::blur(mOldDiff, oldThreshDiff, cv::Size(mBlurSize, mBlurSize));
    cv::threshold(oldThreshDiff, oldThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    cv::Mat newDiff;
    cv::absdiff(curGrayFrame, nxtGrayFrame, newDiff);
    //cv::threshold(newDiff, newThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);
    cv::blur(newDiff, newThreshDiff, cv::Size(mBlurSize, mBlurSize));
    cv::threshold(newThreshDiff, newThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    cv::Mat threshDiff;
    cv::bitwise_and(oldThreshDiff, newThreshDiff, threshDiff);

    //emit threshReady(threshDiff.clone(), TS_GRAY);

    return QList<cv::Rect>();
}

QMap<double, TrackingBall> KFBallTracker::findMovementThresh(cv::Mat threshDiff, QList<cv::Rect> ignores) {
    std::vector<std::vector<cv::Point> > contours;
    QMap<double, TrackingBall> balls;

    cv::findContours(threshDiff, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    emit contourReady(threshDiff, TS_GRAY);

    for (unsigned long i=0; i<contours.size(); i++) {
        TrackingBall ball = TrackingBall(contours[i]);

        if (ball.r() > mMaxRadius || ball.r() < mMinRadius) {
            // We don't want HUGE blobs
            continue;
        }

        if (ball.center().x() < 20 || ball.center().x() > 300) {
            qDebug() << ball.center();
            continue;
        }

        QList<cv::Rect>::const_iterator ignore;
        bool ignoreBall = false;
        for (ignore = ignores.begin(); ignore != ignores.end(); ++ignore) {
            if (((*ignore) & ball.rect()).area()) {
                ignoreBall = true;
                break;
            }
        }
        if (ignoreBall) {
            continue;
        }

        double score = scoreContour(ball, threshDiff);

        if (score > 0.7) {
            balls[score] = ball;
        }
    }

    return balls;
}

void KFBallTracker::updateTrackFailure()
{
    mBNotFoundCount++;

    if (ballIsLost()) {
        mBFoundCount = 0;
    } else {
        mBKf.statePost = mBKfState;
    }
    emit ballPredicted(KFPrediction(mBKf.statePost, dT(), false));

}

void KFBallTracker::updateTrackSuccess(TrackingBall ball)
{
    mBKfMeas = ball.getMeas();

    if (!ballFound()) {
        mBKf.errorCovPost = cv::Mat::eye(mBKfStateLen, mBKfStateLen, CV_64F)*1000;

        mBKfState.at<double>(BallKFState::x) = mBKfMeas.at<double>(BallKFMeas::x);
        mBKfState.at<double>(BallKFState::y) = mBKfMeas.at<double>(BallKFMeas::y);
        mBKfState.at<double>(BallKFState::w) = mBKfMeas.at<double>(BallKFMeas::w);
        mBKfState.at<double>(BallKFState::h) = mBKfMeas.at<double>(BallKFMeas::h);
        mBKfState.at<double>(BallKFState::dx) = 0;
        mBKfState.at<double>(BallKFState::dy) = 0;

        mBKf.statePost = mBKfState;
    } else {
        mBKf.correct(mBKfMeas.t());
    }

    emit ballPredicted(KFPrediction(mBKf.statePost, dT(), true));

    mBNotFoundCount = 0;
    mBFoundCount++;
}

bool KFBallTracker::ballIsLost()
{
    return mBNotFoundCount >= 4;
}

bool KFBallTracker::ballFound()
{
    return mBFoundCount > 0;
}

void KFBallTracker::updateTimeState(double t)
{
    mTstart = mTstop;
    mTstop = t;
    //std::cout << "t " << mTstop << " " << mTstart;

    mBKf.transitionMatrix.at<double>(BallKFState::x, BallKFState::dx) = dT();
    mBKf.transitionMatrix.at<double>(BallKFState::y, BallKFState::dy) = dT();

    mBKfState = mBKf.predict(mBKfControlVec*mGravConstant*dT());

    // << "\n" << mKf.transitionMatrix;
    //std::cout << mKf.controlMatrix << "\n";
    //std::cout << mKf.transitionMatrix << "\n----\n";
    //std::cout << mKfState << "\n";
}

QMap<double, TrackingBall> KFBallTracker::findBalls(cv::Mat &nxtFrame, QList<cv::Rect> ignores)
{

    cv::Mat preFrame = mFrameHistory[0];
    cv::Mat curFrame = mFrameHistory[1];

    cv::Mat preGrayFrame, curGrayFrame, nxtGrayFrame;

    cv::cvtColor(preFrame, preGrayFrame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(curFrame, curGrayFrame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(nxtFrame, nxtGrayFrame, cv::COLOR_BGR2GRAY);

    cv::Mat oldThreshDiff, newThreshDiff;

    cv::absdiff(preGrayFrame, curGrayFrame, mOldDiff);
    //cv::threshold(mOldDiff, oldThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    //cv::blur(mOldDiff, oldThreshDiff, cv::Size(mBlurSize, mBlurSize));
    cv::threshold(mOldDiff, oldThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    cv::Mat newDiff;
    cv::absdiff(curGrayFrame, nxtGrayFrame, newDiff);
    //cv::threshold(newDiff, newThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    //cv::blur(newDiff, newThreshDiff, cv::Size(mBlurSize, mBlurSize));
    cv::threshold(newDiff, newThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    cv::Mat threshDiff;
    cv::bitwise_and(oldThreshDiff, newThreshDiff, threshDiff);

    //cv::erode(threshDiff, threshDiff, cv::Mat::ones(1,1,CV_32F));
    cv::morphologyEx(threshDiff, threshDiff, cv::MORPH_CLOSE, cv::Mat::ones(15,15,CV_32F));
    //cv::dilate(threshDiff, threshDiff, cv::Mat::ones(10,10,CV_32F));

    emit threshReady(threshDiff.clone(), TS_GRAY);

    QMap<double, TrackingBall> balls = findMovementThresh(threshDiff, ignores);

    if (!balls.isEmpty()) {
        updateTrackSuccess(balls.first());
    } else {
        updateTrackFailure();
    }
    return balls;
}

