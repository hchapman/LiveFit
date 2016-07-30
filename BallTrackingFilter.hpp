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
