#ifndef KFBALLTRACKER_HPP
#define KFBALLTRACKER_HPP

#include "BallTrackingFilter.hpp"
#include "ColorSpace.hpp"
#include "KFPrediction.hpp"
#include "KalmanFilterPlus.hpp"
#include "PersonTrackingFilter.hpp"
#include "TrackingBall.hpp"

#include <QObject>
#include <QPolygonF>
#include <QVector>

#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

/**
 * @brief The KFBallTracker class
 * Contains most of the logic for live-tracking objects. Recieves
 * a frame (as cv::Mat) and uses it (and its own internal state)
 * to identify a moving projectile ball and emit them (as TrackingBall
 * and as KFPrediction)
 */
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

    QPolygonF mClipShape;
    bool mClipTrack;

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
    void ballLost();
    void personSpotted(QRect);

    void threshReady(const cv::Mat &, enum ColorSpace);
    void contourReady(const cv::Mat &, enum ColorSpace);
    void blurReady(const cv::Mat &, enum ColorSpace);

public slots:
    void setClipShape(QPolygonF shape);
    void toggleClip(bool clip);

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
