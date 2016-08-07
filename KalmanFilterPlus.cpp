#include "KalmanFilterPlus.hpp"


void KalmanFilterPlus::setAlphaSq(double value)
{
    mAlphaSq = value;
}

KalmanFilterPlus::KalmanFilterPlus() :
    cv::KalmanFilter()
{
    mAlphaSq = 1;
}

KalmanFilterPlus::KalmanFilterPlus(int dynamParams, int measureParams, int controlParams, int type) :
    cv::KalmanFilter(dynamParams, measureParams, controlParams, type)
{
    mAlphaSq = 1;
}

const cv::Mat& KalmanFilterPlus::predict(const cv::Mat& control)
{
    // update the state: x'(k) = A*x(k)
    statePre = transitionMatrix*statePost;

    if( !control.empty() )
        // x'(k) = x'(k) + B*u(k)
        statePre += controlMatrix*control;

    // update error covariance matrices: temp1 = A*P(k)
    temp1 = mAlphaSq*transitionMatrix*errorCovPost;


    // P'(k) = temp1*At + Q
    cv::gemm(temp1, transitionMatrix, 1.0, processNoiseCov, 1.0, errorCovPre, cv::GEMM_2_T);

    // handle the case when there will be measurement before the next predict.
    statePre.copyTo(statePost);
    errorCovPre.copyTo(errorCovPost);

    return statePre;
}
