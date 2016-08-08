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
  // Sigma = HPH^T + R
  // (z - Hx)^T * Sigma^{-1} * (z - Hx) + ln det(Sigma)
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
