/*
 *  LiveFit
 *  Copyright (C) 2016 The University of Georgia
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef TRACKINGSTREAM_HPP
#define TRACKINGSTREAM_HPP

#include "ColorSpace.hpp"
#include "DisplayFrameType.hpp"
#include "KFBallTracker.hpp"
#include "KFPrediction.hpp"

#include <QBasicTimer>
#include <QImage>

#include <opencv2/videoio.hpp>

// Default settings pertaining to the TrackingStream. Ensure that
// settings in the .ui file default to these!
#define SETT_DEF_DISPLAYTYPE 0
#define SETT_DEF_XYCOVAR 1.0
#define SETT_DEF_FOV 56
#define SETT_DEF_PROJW 50
#define SETT_DEF_PROJH 50
#define SETT_DEF_BALLZ 0

/**
 * @brief An object which owns some sort of input video stream, manages
 * running it (and any tracking, etc) and emits cv::Mat frames and
 * tracked objects
 */
class TrackingStream : public QObject
{
  Q_OBJECT

  QBasicTimer mTimer; /**< Timer for processing frames */

  /**
   * @brief The handler for the input video stream
   */
  QScopedPointer<cv::VideoCapture> mVideoHandle;

  /**
   * @brief The FOV of the camera.
   *
   * A PS2 Eye has two settings:
   * - Red dot: 56deg, close up
   * - Blue dot: 75deg, wide view
   *
   * The smaller FOV is expected to cause less distortion, although
   * it is likely to have less in the frame.
   */
  int mFov;
  /**
   * @brief The dimensions of the video stream
   */
  QSize mVideoSize;

  /**
   * @brief The dimensions (in real world units) of the projector screen
   */
  QSize mProjSize;
  /**
   * @brief The distance of the ball plane from the projector plane
   * (in real world units)
   */
  double mBallZ;

  /**
   * @brief The location of projector coordinates in world (real units)
   *
   * Order is important: expected NW, SW, SE, NE (counterclockwise)
   */
  cv::Mat mProjCornersCamera;

  /**
   * @brief The location of projector coordinates in the image
   *
   * Order is important: expected NW, SW, SE, NE (counterclockwise)
   */
  std::vector<cv::Point2f> mCorners;

  /**
   * @brief The bounding polygon of image coordinates which project
   * to ball coordinates that live in the projector screen
   */
  std::vector<cv::Point2f> mBallPlane;

  /**
   * @brief The intrinsic camera matrix
   */
  cv::Mat mCameraMatrix;

  /**
   * @brief The camera's extrinsic(?) rotation matrix
   */
  cv::Mat mRMatrix;

  /**
   * @brief The camera's extrinsic(?) translation vector
   */
  cv::Mat mTVector;

  /**
   * @brief The present frame received from the video stream
   */
  cv::Mat mFrame;

  /**
   * @brief The type (e.g. video, thresh...) of frame to emit
   */
  DisplayFrameType mEmitFrameType;

  bool mStarted = false; /**< Whether the video stream is started */
  bool mProjReady = false; /**< Whether there is enough projector data for transform */
  bool mStreamPaused = false; /**< Whether the stream is currently paused */

  int mFps; /**< The FPS of the video stream (=0 for webcam stream) */

  /**
   * @brief The ball tracker; we give it frames, it gives us ball predictions
   */
  KFBallTracker mTracker;
  bool mDisplayVideo = true; /**< Whether we should display video or not */

  QPolygonF mProjScreen; /**< Region to crop to if mClipTrack is on */
  bool mClipTrack = false; /**< Whether we should clip tracking to the proj screen */

signals:
  // Signals for emitting frames for display
  /**
   * @brief frameReady emits a cv::Mat of specified color space for processing
   */
  void frameReady(const cv::Mat &, enum ColorSpace=TS_BGR);

  // Signals which emit information about tracking
  /** @brief A ball has been spotted (image coordinates) */
  void ballSpotted(TrackingBall ball);
  /** @brief A ball has been predicted (image coordinates) */
  void ballPredicted(KFPrediction pred);
  /** @brief A ball has been spotted (proj coordinates) */
  void ballProjSpotted(TrackingBall ball);
  /** @brief A ball has been predicted (proj coordinates) */
  void ballProjPredicted(KFPrediction pred);
  /** @brief We have lost track of the ball entirely */
  void ballLost();

  /** @brief Signal emitted when stream starts processing */
  void started();

public slots:
  // Slots to start or stop this tracking stream
  /** @brief Start a stream with a camera input */
  void start(int cam = 0);
  /** @brief Start a stream with a filename input */
  void start(QString fname);
  /** @brief Terminate the video stream */
  void stop();
  /** @brief Pause the current video stream */
  void pauseStream();

  // Slots for responding to projector settings
  /** @brief Set the location of projector corners in the image */
  void changeProjectorCorners(std::vector<cv::Point2f> corners);

  /** @brief Set whether or not tracking is clipped to projector */
  void changeClipTrack(bool checked);

  /** @brief Alert the stream to a predicted ball */
  void predictionBall(KFPrediction pred);


  /** @brief Set the camera FOV */
  void setFov(int fov);
  /**
   * @brief changeProjWidth sets the width of the projector screen (in inches)
   * @param width the width of the projector screen
   */
  void changeProjWidth(int width);
  /**
   * @brief changeProjHeight sets the height of the projector screen (in inches)
   * @param height the height of the projector screen
   */
  void changeProjHeight(int height);
  /**
   * @brief changeBallZ sets the z-plane that the ball is thrown in (in inches)
   * @param z the z-distance from the projector plane in which the ball lies
   */
  void changeBallZ(double z);

  // Slots for responding to tracker UI param settings
  /**
   * @brief Set the strength of blur in preprocessing step
   * @param blurSize
   */
  void changeBlurSize(double blurSize);
  /**
   * @brief Set the threshold in preprocessing step
   * @param thresh
   */
  void changeThreshVal(double thresh);
  /**
   * @brief Set the min viable radius of a ball blob
   * @param radius
   */
  void changeMinRadius(double radius);
  /**
   * @brief Set the max viable radius of a ball blob
   * @param radius
   */
  void changeMaxRadius(double radius);
  /**
   * @brief Set the accel due to gravity g
   * @param g
   */
  void changeGravConstant(double g);
  /**
   * @brief Set the covariance in predicted X, Y (a KF parameter)
   * @param sigma
   */
  void changeXYCovariance(double sigma);

  // Slots for receiving frames to (possibly) send along to converter
  /** @brief Process a "thresholded" frame, with white blobs */
  void processThresh(const cv::Mat &, enum ColorSpace=TS_GRAY);
  /** @brief Process a "blurred" frame */
  void processBlur(const cv::Mat &, enum ColorSpace=TS_BGR);
  /** @brief Process a "contour" frame, which marks contours */
  void processContour(const cv::Mat &, enum ColorSpace=TS_GRAY);
  /** @brief Process a "video" frame, which is raw video input */
  void processVideoFrame(const cv::Mat& mat, ColorSpace=TS_BGR);

  /** @brief Set the type of video frame to actually be emitted */
  void changeDisplayFrameType(QString s);
public:
  TrackingStream(QObject *parent = 0);
  ~TrackingStream();

  /** Start the tracking process */
  void startProcess();

  /**
   * @brief Convert image coordinates to projector coordinates.
   *
   * Like most things, converting image coordinates (i.e. what the camera
   * sees) to projector coordinates (world coordinates, where the top
   * left corner of the projector is the origin) is just linear algebra.
   *
   * This is the inverse of projectorToImage
   *
   * The players:
   * - R = #mRMatrix, the camera rotation matrix
   * - C = #mCameraMatrix, the intrinsic camera matrix
   * - T = #mTVector, the camera translation vector
   * - p = [\p imP, 1], the image point in homogeneous coordinates
   * - z = the (a priori known) z-coordinate of the point in world coords
   * - q = [X, Y, z] the world coordinates of the point p
   * - s = Scaling factor that comes from transformation... picks appropriate point
   * on the line projecting to p with the input z coordinate
   *
   * \f[s = (z + (R^{-1}T)_{2,0}) / (R^{-1}C^{-1}p)_{2,0}\f]
   *
   * \f[q = R^{-1}(sC^{-1}p - T)\f]
   *
   *
   * @param imP image point (x,y) to translate to world coordinates
   * @param z a priori known world z-coordinate of the point to find
   * @return
   *
   */
  cv::Point2f imageToProjector(cv::Point2f imP, double z=0);

  /**
   * @brief Convert image coordinates to projector coordinates (qt)
   *
   * A wrapper that works with Qt objects instead of opencv objects
   *
   * @param imP image point (x,y) to translate to world coordinates
   * @param z a priori known world z-coordinate of the point to find
   * @return
   */
  QPointF imageToProjector(QPointF imP, double z=0);

  /**
   * @brief projectorToImage converts projector (world) coords to image coords
   * @param prP the point in world coordinates
   * @return the image pixel corresponding to prP
   */
  cv::Point2f projectorToImage(cv::Point3d prP);

  /** @brief Set the in-image coordinates of the projector corners */
  void updateProjectorCoordinates(std::vector<cv::Point2f> corners);

  /** @brief Set the size of the projector (in real units) */
  void setProjSize(const QSize& projSize);

  /** @brief Update the camera matrices */
  void refreshCameraMatrix();
  /** @brief Update the projector transformation matrices */
  void refreshProjectorMatrices();

  /** @brief Check if a camera index is valid.
  *
  * Presently this does nothing... V4L detection on Linux seems a bit finicky
  * and making this cross platform is an interesting problem. For now, it would
  * seem reasonable if the program just be restarted in order to fix
  * any issues here.
  */
  bool cameraValid(int idx);
private:
  /** Process a timer event: Read the next frame, process it, emit it. */
  void timerEvent(QTimerEvent *);
};

#endif // TRACKINGSTREAM_HPP
