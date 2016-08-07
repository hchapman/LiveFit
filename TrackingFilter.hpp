#ifndef TRACKINGFILTER_HPP
#define TRACKINGFILTER_HPP

#include "KalmanFilterPlus.hpp"
#include <QDebug>

/**
 * @brief A wrapper around a KalmanFilter type object; additional logic
 * can be provided through extension.
 *
 * The idea is this is an "abstract" tracker which is object-agnostic; children
 * should implement some methods with additional knowledge of the type
 * of thing that they are trying to track.
 */
class TrackingFilter
{
public:
  TrackingFilter();

  /** Reset KF state to initial values */
  void flushKalman();

  /**
   * Compute the Kalman distance to a measurement vector.
   *
   * The players...
   * - z = input measurement vector
   * - x = KF state pre
   * - H = KF measurement matrix
   * - P = KF error covariance matrix pre
   * - R = KF measurement noise covariance
   *
   * With,
   * \f[\Sigma = HPH^T + R\f]
   *
   * Kalman Distance is defined as:
   * \f[(z - Hx)^T * \Sigma^{-1} * (z - Hx) + \ln \det(\Sigma)\f]
   *
   * Additional information at
   * http://www.mathworks.com/help/vision/ref/vision.kalmanfilter-class.html
   */
  double kalmanDistance(cv::Mat measurement);

  /** Alert the KF that we did not track a ball this frame */
  void updateTrackFailure();

  /** The KF prediction of the state */
  cv::Mat prediction();
  /** The KF's covariance matrix */
  cv::Mat covariance();

  /** Update the time state of the tracker to the current time */
  void updateTimeState(double t);

  /** Change in time from prior state to this one */
  double dT() { return mTstop - mTstart; }
  /** The current time state */
  double time() { return mTstop; }

  /** Whether we have found a object recently */
  bool isFound();
  /** Whether the object is lost and we don't know anything about it */
  bool isLost();

protected:
  /** Radius of the current state blob? */
  int mBlobRad;
  /** Unused? */
  int mLatency;
  /** Number of frames we have missed this object for.
   *
   * If this number gets too large, we have lost the object
   */
  int mNotFoundCount;

  /** Length of the state vector; 6 <x,y,dx,dy,w,h> */
  int mKfStateLen;
  /** State vector of most recent state */
  cv::Mat mKfState;
  /** Number of frames we have tracked this object for */
  int mFoundCount;

  int mKfControlLen;
  cv::Mat mKfControl;
  cv::Mat mKfControlVec;

  /** Length of the measurement vector; 4 <x,y,w,h> */
  int mKfMeasLen;
  /** Measurement vector of most recent state */
  cv::Mat mKfMeas;

  /** Start time of this tick (the previous timestep's time) */
  double mTstart = 0;
  /** Stop time of this tick (the current timestate) */
  double mTstop = 0;

  /** The Kalman Filter object proper */
  KalmanFilterPlus mKf;
};

#endif // TRACKINGFILTER_HPP
