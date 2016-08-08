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

#ifndef TRACKINGBALL_HPP
#define TRACKINGBALL_HPP

#include <opencv2/core.hpp>

#include <QPoint>

/**
 * @brief A ball which has been tracked by the software
 */
class TrackingBall
{
  /** The measurement vector (KF spotting input) */
  cv::Mat mMeasVec;
  /** The center of the ball mass */
  cv::Point2f mCenter;
  /** The radius of the ball mass */
  float mRadius;
  /** A Rect which encompasses the ball mass */
  cv::Rect mRect;

public:
  /** Construct an empty TrackingBall */
  TrackingBall();
  /** Construct a TrackingBall from a measurement vector (usual method) */
  TrackingBall(std::vector<cv::Point> contour);

  /** Get the measurement vector */
  cv::Mat getMeas() const;

  /** Get the x coordinate of the center */
  double x() const;
  /** Get the y coordinate of the center */
  double y() const;
  /** Get the width of the bounding rect */
  double width() const;
  /** Get the height of the bounding rect */
  double height() const;
  /** Get the radius of the bounding circle */
  double r() const;

  /** Get the center of the ball mass */
  QPoint center() const;
  /** Top left corner of the bounding rect */
  QPoint tl() const;

  /** Get the bounding rect for the ball */
  cv::Rect rect() const;
};

#endif // TRACKINGBALL_HPP
