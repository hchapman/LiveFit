/*
 *  LiveFit
 *  Copyright (C) 2016 The University of Georgia
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
    mBlurSize = SETT_DEF_BLOB_BLUR;
    mThreshVal = SETT_DEF_BLOB_THRESH;

    mMinRadius = SETT_DEF_BLOB_MINRAD;
    mMaxRadius = SETT_DEF_BLOB_MAXRAD;

    mBallFilter.setGravConstant(SETT_DEF_BLOB_GRAV);
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

double KFBallTracker::scoreContour(TrackingBall ball,
                                   cv::Mat &threshDiff) {
    return pow(2., mBallFilter.kalmanDistance(ball.getMeas()));
}

void KFBallTracker::setXYCovariance(double sigma)
{
    mBallFilter.setXYCovariance(sigma);
}

void KFBallTracker::setClipShape(QPolygonF shape)
{
    mClipShape = shape;
}

void KFBallTracker::toggleClip(bool clip)
{
    mClipTrack = clip;
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
    //std::vector<cv::Mat> contours;
    QMap<double, TrackingBall> balls;

    cv::findContours(threshDiff, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    emit contourReady(threshDiff, TS_GRAY);

    for (unsigned long i=0; i<contours.size(); i++) {
        TrackingBall ball = TrackingBall(contours[i]);

        if (ball.r() > mMaxRadius || ball.r() < mMinRadius) {
            // We don't want HUGE blobs
            continue;
        }

        if (mClipTrack && !mClipShape.containsPoint((QPointF)ball.center(), Qt::OddEvenFill)) {
            continue;
        }

        if (ball.center().x() < 20 || ball.center().x() > 300) {
            //qDebug() << ball.center();
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

        //qDebug() << score;

        if (score > 0.7) {
            balls[score] = ball;
        }
    }

    return balls;
}

void KFBallTracker::updateTrackFailure()
{
    if (!mBallFilter.isLost()) {
        mBallFilter.updateTrackFailure();

        emit ballPredicted(KFPrediction(mBallFilter.prediction(), mBallFilter.covariance(),
                                        mBallFilter.time(), mBallFilter.dT(), false));

        if (mBallFilter.isLost()) {
            emit ballLost();
        }
    }
}

void KFBallTracker::updateTrackSuccess(TrackingBall ball)
{
    mBallFilter.updateTrackSuccess(ball);

    emit ballPredicted(KFPrediction(mBallFilter.prediction(), mBallFilter.covariance(),
                                    mBallFilter.time(), mBallFilter.dT(), true));
}

void KFBallTracker::updateTimeState(double t)
{
    mBallFilter.updateTimeState(t);
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

    cv::blur(mOldDiff, oldThreshDiff, cv::Size(mBlurSize, mBlurSize));
    cv::threshold(oldThreshDiff, oldThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);
    //cv::threshold(mOldDiff, oldThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    cv::Mat newDiff;
    cv::absdiff(curGrayFrame, nxtGrayFrame, newDiff);
    //cv::threshold(newDiff, newThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    cv::blur(newDiff, newThreshDiff, cv::Size(mBlurSize, mBlurSize));
    cv::threshold(newThreshDiff, newThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);
    //cv::threshold(newDiff, newThreshDiff, mThreshVal, 255, cv::THRESH_BINARY);

    cv::Mat threshDiff;
    cv::bitwise_and(oldThreshDiff, newThreshDiff, threshDiff);

    //cv::erode(threshDiff, threshDiff, cv::Mat::ones(1,1,CV_32F));
    cv::morphologyEx(threshDiff, threshDiff, cv::MORPH_CLOSE, cv::Mat::ones(10,10,CV_32F));
    //cv::dilate(threshDiff, threshDiff, cv::Mat::ones(10,10,CV_32F));

    cv::Mat bgr, dst;
    cv::cvtColor(threshDiff, bgr, cv::COLOR_GRAY2BGR);
    addWeighted(curFrame, 0.8, bgr, 0.5, 0.0, dst);
    emit threshReady(dst, TS_BGR);

    QMap<double, TrackingBall> balls = findMovementThresh(threshDiff, ignores);

    if (!balls.isEmpty()) {
        updateTrackSuccess(balls.first());
    } else {
        updateTrackFailure();
    }
    return balls;
}

