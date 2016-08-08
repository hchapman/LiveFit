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

#include "KalmanFilterPlus.hpp"


void KalmanFilterPlus::setAlphaSq(double value)
{
    mAlphaSq = value;
}

KalmanFilterPlus::KalmanFilterPlus() :
    cv::KalmanFilter()
{
    mAlphaSq = 1;
}

KalmanFilterPlus::KalmanFilterPlus(int dynamParams, int measureParams, int controlParams, int type) :
    cv::KalmanFilter(dynamParams, measureParams, controlParams, type)
{
    mAlphaSq = 1;
}

const cv::Mat& KalmanFilterPlus::predict(const cv::Mat& control)
{
    // update the state: x'(k) = A*x(k)
    statePre = transitionMatrix*statePost;

    if( !control.empty() )
        // x'(k) = x'(k) + B*u(k)
        statePre += controlMatrix*control;

    // update error covariance matrices: temp1 = A*P(k)
    temp1 = mAlphaSq*transitionMatrix*errorCovPost;


    // P'(k) = temp1*At + Q
    cv::gemm(temp1, transitionMatrix, 1.0, processNoiseCov, 1.0, errorCovPre, cv::GEMM_2_T);

    // handle the case when there will be measurement before the next predict.
    statePre.copyTo(statePost);
    errorCovPre.copyTo(errorCovPost);

    return statePre;
}
