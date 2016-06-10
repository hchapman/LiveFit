#ifndef BALLTRACKINGFILTER_HPP
#define BALLTRACKINGFILTER_HPP

#include "TrackingBall.hpp"
#include "TrackingFilter.hpp"

namespace BallKFState {
enum BallKFState {x, y, dx, dy, w, h};
}
namespace BallKFMeas {
enum BallKFMeas {x, y, w, h};
}

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
