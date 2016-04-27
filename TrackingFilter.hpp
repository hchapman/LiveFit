#ifndef TRACKINGFILTER_HPP
#define TRACKINGFILTER_HPP

#include "KalmanFilterPlus.hpp"



class TrackingFilter
{
public:
    TrackingFilter();

    void flushKalman();

signals:

public slots:

protected:
    int mBlobRad;
    int mLatency;
    int mBNotFoundCount;

    int mKfStateLen;
    cv::Mat mKfState;
    int mFoundCount;

    int mKfControlLen;
    cv::Mat mKfControl;
    cv::Mat mKfControlVec;

    int mKfMeasLen;
    cv::Mat mKfMeas;

    KalmanFilterPlus mKf;
};

#endif // TRACKINGFILTER_HPP
