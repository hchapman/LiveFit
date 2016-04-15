#ifndef KFBALLTRACKER_HPP
#define KFBALLTRACKER_HPP

#include "ColorSpace.hpp"
#include "KFPrediction.hpp"
#include "TrackingBall.hpp"

#include <QObject>
#include <QVector>

#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

namespace BallKFState {
enum BallKFState {x, y, dx, dy, w, h};
}
namespace BallKFMeas {
enum BallKFMeas {x, y, w, h};
}

class KFBallTracker : public QObject
{
    Q_OBJECT
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

    cv::KalmanFilter mKf;

    QVector<cv::Mat> mFrameHistory;
    cv::Mat mOldDiff;

    double mTstart;
    double mTstop;

    double mBlurSize;
    double mThreshVal;

    double mMinRadius;
    double mMaxRadius;

    double mGravConstant;

public:
    explicit KFBallTracker(QObject *parent = 0);

    QMap<double, TrackingBall> processNextFrame(cv::Mat &frame, int t);

    void updateTimeState(double t);
    void flushKalman();
    double kalmanDistance(cv::Mat measurement);

    void setBlurSize(double blurSize) { mBlurSize = blurSize; }
    void setThreshVal(double thresh) { mThreshVal = thresh; }

    void setMinRadius(double radius) { mMinRadius = radius; }
    void setMaxRadius(double radius) { mMaxRadius = radius; }

    void setXYCovariance(double sigma);

    void setGravConstant(double g) { mGravConstant = g; }
signals:
    void ballSpotted(TrackingBall);
    void ballPredicted(KFPrediction);

    void threshReady(const cv::Mat &, enum ColorSpace);
    void contourReady(const cv::Mat &, enum ColorSpace);
    void blurReady(const cv::Mat &, enum ColorSpace);

public slots:

protected:
    double dT() { return mTstop - mTstart; }

    QMap<double, TrackingBall> findBalls(cv::Mat &frame);
    QMap<double, TrackingBall> findMovementThresh(cv::Mat threshDiff);

    void updateTrackFailure();
    void updateTrackSuccess(TrackingBall ball);

    bool ballIsLost();
    bool ballFound();

    void updateFrameState(cv::Mat *frame);
    double scoreContour(TrackingBall ball, cv::Mat &threshDiff);
};

#endif // KFBALLTRACKER_HPP
