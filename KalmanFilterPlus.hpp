#ifndef KALMANFILTERPLUS_HPP
#define KALMANFILTERPLUS_HPP

#include <opencv2/video/tracking.hpp>

#include <opencv2/core.hpp>

/**
 * @brief The KalmanFilterPlus class
 * A cv::KalmanFilter which also supports some 'forgetful' functionality
 */
class KalmanFilterPlus : public cv::KalmanFilter
{
    double mAlphaSq;
public:
    KalmanFilterPlus();
    KalmanFilterPlus( int dynamParams, int measureParams, int controlParams = 0, int type = CV_32F );
    const cv::Mat& predict( const cv::Mat& control = cv::Mat() );

    void setAlphaSq(double value);
};

#endif // KALMANFILTERPLUS_HPP
