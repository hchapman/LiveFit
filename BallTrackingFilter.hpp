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

#ifndef BALLTRACKINGFILTER_HPP
#define BALLTRACKINGFILTER_HPP

#include "TrackingBall.hpp"
#include "TrackingFilter.hpp"

namespace BallKFState {
  /**
   * @brief
   * An enum that relates coordinates of a Kalman Filter state vector to their meaning
   */
  enum BallKFState {x, y, dx, dy, w, h};
}
namespace BallKFMeas {
  /**
   * @brief
   * An enum that relates coordinates of a Kalman Filter measurement vector to their meaning
   */
  enum BallKFMeas {x, y, w, h};
}

/**
 * @brief
 * A TrackingFilter with some specifics for tracking a ball in flight
 */
class BallTrackingFilter : public TrackingFilter
{
  /** A priori known acceleration due to gravity g (positive direction) is
   *  towards center of earth. */
  double mGravConstant;

public:
  BallTrackingFilter();

  /**
   * Reset the status of the KF to initial
   */
  void flushKalman();
  /**
   * Set the covariance for the x and y measurements in the KF
   */
  void setXYCovariance(double sigma);

  /**
   * Tell the KF that we failed to track a ball
   */
  void updateTrackFailure();
  /**
   * Tell the KF about a new tracked ball
   */
  void updateTrackSuccess(TrackingBall ball);

  /**
   * Change the current internal time state of the tracker (eg at a new tick)
   */
  void updateTimeState(double t);

  /**
   * Set the acceleration due to gravity
   */
  void setGravConstant(double gravConstant);

};

#endif // BALLTRACKINGFILTER_HPP
