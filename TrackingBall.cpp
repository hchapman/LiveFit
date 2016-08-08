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

#include "TrackingBall.hpp"

#include <opencv2/imgproc.hpp>

TrackingBall::TrackingBall() {
    mMeasVec = cv::Mat(1, 4, CV_64F);
}

cv::Rect TrackingBall::rect() const
{
    return mRect;
}

TrackingBall::TrackingBall(std::vector<cv::Point> contour)
{
    mRect = cv::boundingRect(contour);

    mMeasVec = cv::Mat(1, 4, CV_64F);

    mMeasVec.at<double>(0) = mRect.x+mRect.width/2;
    mMeasVec.at<double>(1) = mRect.y+mRect.height/2;
    mMeasVec.at<double>(2) = mRect.width;
    mMeasVec.at<double>(3) = mRect.height;

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
