#ifndef KFBALLTRACKER_HPP
#define KFBALLTRACKER_HPP

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

    int mTstart;
    int mTstop;

public:
    explicit KFBallTracker(QObject *parent = 0);

    QMap<double, TrackingBall> processNextFrame(cv::Mat &frame, int t);

    void updateTimeState(int t);
    void flushKalman();
    double kalmanDistance(cv::Mat measurement);
signals:

public slots:

protected:
    int dT() { return mTstop - mTstart; }

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
