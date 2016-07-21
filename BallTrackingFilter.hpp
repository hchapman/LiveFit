#ifndef BALLTRACKINGFILTER_HPP
#define BALLTRACKINGFILTER_HPP

#include "TrackingBall.hpp"
#include "TrackingFilter.hpp"

namespace BallKFState {
/**
 * @brief The BallKFState enum
 * An enum that relates coordinates of a Kalman Filter state vector to their meaning
 */
enum BallKFState {x, y, dx, dy, w, h};
}
namespace BallKFMeas {
/**
 * @brief The BallKFMeas enum
 * An enum that relates coordinates of a Kalman Filter measurement vector to their meaning
 */
enum BallKFMeas {x, y, w, h};
}

/**
 * @brief The BallTrackingFilter class
 * A TrackingFilter with some specifics for tracking a ball in flight
 */
class BallTrackingFilter : public TrackingFilter
{
    double mGravConstant;

public:
    BallTrackingFilter();

    void flushKalman();
    void setXYCovariance(double sigma);

    void updateTrackFailure();
    void updateTrackSuccess(TrackingBall ball);

    void updateTimeState(double t);

    void setGravConstant(double gravConstant);

};

#endif // BALLTRACKINGFILTER_HPP
