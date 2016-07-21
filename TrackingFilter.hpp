#ifndef TRACKINGFILTER_HPP
#define TRACKINGFILTER_HPP

#include "KalmanFilterPlus.hpp"
#include <QDebug>

/**
 * @brief The TrackingFilter class
 * A wrapper around a KalmanFilter type object; additional logic
 * can be provided through extension
 */
class TrackingFilter
{
public:
    TrackingFilter();

    void flushKalman();
    double kalmanDistance(cv::Mat measurement);

    void updateTrackFailure();

    cv::Mat prediction();
    cv::Mat covariance();

    void updateTimeState(double t);

    double dT() { return mTstop - mTstart; }
    double time() { return mTstop; }

    bool isFound();
    bool isLost();

protected:
    int mBlobRad;
    int mLatency;
    int mNotFoundCount;

    int mKfStateLen;
    cv::Mat mKfState;
    int mFoundCount;

    int mKfControlLen;
    cv::Mat mKfControl;
    cv::Mat mKfControlVec;

    int mKfMeasLen;
    cv::Mat mKfMeas;

    double mTstart, mTstop;

    KalmanFilterPlus mKf;
};

#endif // TRACKINGFILTER_HPP
