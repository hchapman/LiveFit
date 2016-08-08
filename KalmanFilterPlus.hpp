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

#ifndef KALMANFILTERPLUS_HPP
#define KALMANFILTERPLUS_HPP

#include <opencv2/video/tracking.hpp>

#include <opencv2/core.hpp>

/**
 * @brief A cv::KalmanFilter which also supports some 'forgetful' functionality.
 */
class KalmanFilterPlus : public cv::KalmanFilter
{
  double mAlphaSq;

public:
  KalmanFilterPlus();

  KalmanFilterPlus(int dynamParams, int measureParams,
                   int controlParams = 0, int type = CV_32F);

  /** Predict the location of the ball. Overloaded in order to provide the alpha
   * weight in order to give earlier predictions less significance */
  const cv::Mat& predict(const cv::Mat& control = cv::Mat());

  void setAlphaSq(double value);
};

#endif // KALMANFILTERPLUS_HPP
