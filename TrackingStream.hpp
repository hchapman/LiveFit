#ifndef TRACKINGSTREAM_HPP
#define TRACKINGSTREAM_HPP

#include "ColorSpace.hpp"
#include "DisplayFrameType.hpp"
#include "KFBallTracker.hpp"
#include "KFPrediction.hpp"

#include <QBasicTimer>
#include <QImage>

#include <opencv2/videoio.hpp>

/**
 * @brief The TrackingStream class
 * An object which owns some sort of input video stream, manages
 * running it (and any tracking, etc) and emits cv::Mat frames and
 * tracked objects
 */
class TrackingStream : public QObject
{
    Q_OBJECT

    QBasicTimer mTimer;
    QScopedPointer<cv::VideoCapture> mVideoHandle;
    int mFov;
    QSize mVideoSize;
    QSize mProjSize;
    double mBallZ;
    std::vector<cv::Point2f> mCorners;
    std::vector<cv::Point2f> mBallPlane;
    cv::Mat mCameraMatrix;
    cv::Mat mRMatrix;
    cv::Mat mTVector;
    cv::Mat mProjCornersCamera;

    cv::Mat mFrame;

    DisplayFrameType mEmitFrameType;

    bool mStarted;
    bool mProjReady;
    bool mStreamPaused;

    int mFps;

    KFBallTracker mTracker;
    bool mDisplayVideo;

    QPolygonF mProjScreen;
    bool mClipTrack;

signals:
    // Signals for emitting frames for display
    /**
     * @brief frameReady emits a cv::Mat of specified color space for processing
     */
    void frameReady(const cv::Mat &, enum ColorSpace=TS_BGR);

    // Signals which emit information about tracking
    void ballSpotted(TrackingBall ball);
    void ballPredicted(KFPrediction pred);
    void ballProjSpotted(TrackingBall ball);
    void ballProjPredicted(KFPrediction pred);
    void ballLost();

    // Signal emitted when stream starts processing
    void started();

public slots:
    // Slots to start or stop this tracking stream
    void start(int cam = 0);
    void start(QString fname);
    void stop();
    void pauseStream();

    // Slots for responding to projector settings
    void changeProjectorCorners(std::vector<cv::Point2f> corners);
    void refreshProjectorMatrices();
    void changeClipTrack(bool checked);

    void predictionBall(KFPrediction pred);

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
    void changeBlurSize(double blurSize);
    void changeThreshVal(double thresh);
    void changeMinRadius(double radius);
    void changeMaxRadius(double radius);
    void changeGravConstant(double g);
    void changeXYCovariance(double sigma);

    // Slots for receiving frames to (possibly) send along to converter
    void processThresh(const cv::Mat &, enum ColorSpace=TS_GRAY);
    void processBlur(const cv::Mat &, enum ColorSpace=TS_BGR);
    void processContour(const cv::Mat &, enum ColorSpace=TS_GRAY);
    void processVideoFrame(const cv::Mat& mat, ColorSpace=TS_BGR);

    void changeDisplayFrameType(QString s);
public:
    TrackingStream(QObject *parent = 0);
    ~TrackingStream();

    void startProcess();

    void setFov(int fov);

    /**
     * @brief imageToProjector
     *
     * Like most things, converting image coordinates (i.e. what the camera
     * sees) to projector coordinates (world coordinates, where the top
     * left corner of the projector is the origin) is just linear algebra.
     *
     * This is the inverse of projectorToImage
     *
     * The players:
     * R = mRMatrix, the camera rotation matrix
     * C = mCameraMatrix, the intrinsic camera matrix
     * T = mTVector, the camera translation vector
     * p = [imP, 1], the image point in homogeneous coordinates
     * z = the (a priori known) z-coordinate of the point in world coords
     * q = [X, Y, z] the world coordinates of the point p
     *
     * s = (z + (R^{-1}*T)_{2,0}) / (R^{-1}*C^{-1}*p)_{2,0}
     * Scaling factor that comes from transformation... picks appropriate point
     * on the line projecting to p with the input z coordinate
     *
     * q = R^{-1}*(s*C^{-1}*p - T)
     *
     *
     * @param imP
     * @param z
     * @return
     *
     */
    cv::Point2f imageToProjector(cv::Point2f imP, double z=0);

    /**
     * @brief imageToProjector
     * A wrapper that works with Qt objects instead of opencv objects
     *
     * @param z
     * @return
     */
    QPointF imageToProjector(QPointF, double z=0);

    /**
     * @brief projectorToImage converts projector (world) coords to image coords
     * @param prP the point in world coordinates
     * @return the image pixel corresponding to prP
     */
    cv::Point2f projectorToImage(cv::Point3d prP);

    void updateProjectorCoordinates(std::vector<cv::Point2f> corners);

    void setProjSize(const QSize& projSize);

    void refreshCameraMatrix();
private:
    void timerEvent(QTimerEvent *);
};

#endif // TRACKINGSTREAM_HPP
