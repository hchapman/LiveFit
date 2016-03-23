#ifndef KFBALLTRACKER_HPP
#define KFBALLTRACKER_HPP

#include <QObject>

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
    int mTstart;
    int mTlast;
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

public:
    explicit KFBallTracker(QObject *parent = 0);

    void processNextFrame(cv::Mat *frame, int t);
    void flushKalman();

signals:

public slots:

private:
    int dT() { return mTlast - mTstart; }

    void updateTrackFailure();
    void updateTrackSuccess();

    void updateTimeState(int t);
    void updateFrameState(cv::Mat *frame);
};

#endif // KFBALLTRACKER_HPP
