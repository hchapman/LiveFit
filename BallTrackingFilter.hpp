#ifndef BALLTRACKINGFILTER_HPP
#define BALLTRACKINGFILTER_HPP

#include "TrackingFilter.hpp"

namespace BallKFState {
enum BallKFState {x, y, dx, dy, w, h};
}
namespace BallKFMeas {
enum BallKFMeas {x, y, w, h};
}

class BallTrackingFilter : public TrackingFilter
{
public:
    BallTrackingFilter();

signals:

public slots:
};

#endif // BALLTRACKINGFILTER_HPP
