#ifndef KFBALLTRACKER_HPP
#define KFBALLTRACKER_HPP

#include "BallTrackingFilter.hpp"
#include "ColorSpace.hpp"
#include "KFPrediction.hpp"
#include "KalmanFilterPlus.hpp"
#include "PersonTrackingFilter.hpp"
#include "TrackingBall.hpp"

#include <QObject>
#include <QVector>

#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

class KFBallTracker : public QObject
{
    Q_OBJECT
    int mBlobRad;
    int mLatency;

    BallTrackingFilter mBallFilter;
    PersonTrackingFilter mPersonFilter;

    QVector<cv::Mat> mFrameHistory;
    cv::Mat mOldDiff;

    double mBlurSize;
    double mThreshVal;

    double mMinRadius;
    double mMaxRadius;

public:
    explicit KFBallTracker(QObject *parent = 0);

    QMap<double, TrackingBall> processNextFrame(cv::Mat &frame, int t);

    void updateTimeState(double t);
    double kalmanDistance(cv::Mat measurement);

    void setBlurSize(double blurSize) { mBlurSize = blurSize; }
    void setThreshVal(double thresh) { mThreshVal = thresh; }

    void setMinRadius(double radius) { mMinRadius = radius; }
    void setMaxRadius(double radius) { mMaxRadius = radius; }

    void setXYCovariance(double sigma);

    void setGravConstant(double g) { mBallFilter.setGravConstant(g); }

signals:
    void ballSpotted(TrackingBall);
    void ballPredicted(KFPrediction);
    void personSpotted(QRect);

    void threshReady(const cv::Mat &, enum ColorSpace);
    void contourReady(const cv::Mat &, enum ColorSpace);
    void blurReady(const cv::Mat &, enum ColorSpace);

public slots:

protected:
    QList<cv::Rect> findPeople(cv::Mat &frame);
    QMap<double, TrackingBall> findBalls(cv::Mat &frame, QList<cv::Rect> ignores);
    QMap<double, TrackingBall> findMovementThresh(cv::Mat threshDiff, QList<cv::Rect> ignores);

    void updateTrackFailure();
    void updateTrackSuccess(TrackingBall ball);

    bool ballIsLost();
    bool ballFound();

    void updateFrameState(cv::Mat *frame);
    double scoreContour(TrackingBall ball, cv::Mat &threshDiff);
};

#endif // KFBALLTRACKER_HPP
