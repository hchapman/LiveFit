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
 * @brief Contains most of the logic for live-tracking objects.
 *
 * Recieves a frame (as cv::Mat) and uses it (and its own internal state) to
 * identify a moving projectile ball and emit them (as TrackingBall and as
 * KFPrediction)
 */
class KFBallTracker : public QObject
{
  Q_OBJECT

  /** Unused */
  int mBlobRad;
  /** Unused */
  int mLatency;

  /** Tracking filter for ball objects in the frame */
  BallTrackingFilter mBallFilter;
  /** Tracking filter for people in the frame (to ignore!); unused */
  PersonTrackingFilter mPersonFilter;

  /** History of frames possibly used for tracking (eg 3-way-diff) */
  QVector<cv::Mat> mFrameHistory;
  /** Previous frame diff of history (stored to avoid a duplicate diff) */
  cv::Mat mOldDiff;

  /** Strength of the Gaussian blur in the blur step of preprocessing */
  double mBlurSize;
  /** Minimum white value for the thresholding step */
  double mThreshVal;

  /** Minimum radius of blobs to consider */
  double mMinRadius;
  /** Maximum radius of blobs to consider */
  double mMaxRadius;

  /** If mClipTrack, only consider blobs within this polygon */
  QPolygonF mClipShape;
  /** Whether to only consider blobs in mClipShape */
  bool mClipTrack;

public:
  /** Default constructor */
  explicit KFBallTracker(QObject *parent = 0);

  /** Process the next frame, possibly finding balls and updating state */
  QMap<double, TrackingBall> processNextFrame(cv::Mat &frame, int t);

  /** Update the internal time state of the tracker */
  void updateTimeState(double t);
  /** Kalman distance; see BallFilter.kalmanDistance(); unused. */
  double kalmanDistance(cv::Mat measurement);

  /** Set mBlurSize */
  void setBlurSize(double blurSize) { mBlurSize = blurSize; }
  /** Set mThreshVal */
  void setThreshVal(double thresh) { mThreshVal = thresh; }

  /** Set mMinRadius */
  void setMinRadius(double radius) { mMinRadius = radius; }
  /** Set mMaxRadius */
  void setMaxRadius(double radius) { mMaxRadius = radius; }

  /** Set the covariance of x and y values of ball for KF */
  void setXYCovariance(double sigma);

  /** Set the acceleration due to gravity of the mBallFilter */
  void setGravConstant(double g) { mBallFilter.setGravConstant(g); }

signals:
  /** Emit a TrackingBall when a ball is spotted in the frame */
  void ballSpotted(TrackingBall);
  /** Emit a KFPrediction when the KF makes a prediction about the ball
   * location */
  void ballPredicted(KFPrediction);
  /** Emitted when the ball is lost */
  void ballLost();
  /** Emitted when a person is spotted; unused */
  void personSpotted(QRect);

  /** Emitted when a thresh image is ready to be converted */
  void threshReady(const cv::Mat &, enum ColorSpace);
  /** Emitted when a contour image is ready to be converted */
  void contourReady(const cv::Mat &, enum ColorSpace);
  /** Emitted when a blur image is ready to be converted */
  void blurReady(const cv::Mat &, enum ColorSpace);

public slots:
  /** Set the clipping shape for tracking */
  void setClipShape(QPolygonF shape);
  /** Set whether we should clip tracking */
  void toggleClip(bool clip);

protected:
  /** Find people in a frame; unused. */
  QList<cv::Rect> findPeople(cv::Mat &frame);
  /** Find balls in a frame, ignoring the region ignores */
  QMap<double, TrackingBall> findBalls(cv::Mat &frame, QList<cv::Rect> ignores);
  /** Find any movement in a thresholded image, ignoring the region ignores */
  QMap<double, TrackingBall> findMovementThresh(cv::Mat threshDiff, QList<cv::Rect> ignores);

  /** Called to alert that we did not find a ball in this frame */
  void updateTrackFailure();
  /** Called to alert that we found ball ball in this frame */
  void updateTrackSuccess(TrackingBall ball);

  /** Whether the ball is currently lost */
  bool ballIsLost();
  /** Whether we currently are tracking a found ball */
  bool ballFound();

  /** Update the internal state of the frame */
  void updateFrameState(cv::Mat *frame);
  /** Given a TrackingBall ball in threshDiff, score its likelihood to be our
   * tracking ball */
  double scoreContour(TrackingBall ball, cv::Mat &threshDiff);
};

#endif // KFBALLTRACKER_HPP
