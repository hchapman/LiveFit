#ifndef PERSONTRACKINGFILTER_HPP
#define PERSONTRACKINGFILTER_HPP

#include "TrackingFilter.hpp"

/**
 * @brief The PersonTrackingFilter class
 * A TrackingFilter which contains the necessary logic to find people
 * in frames
 */
class PersonTrackingFilter : public TrackingFilter
{
public:
    PersonTrackingFilter();
};

#endif // PERSONTRACKINGFILTER_HPP
