#ifndef TRACKINGSTREAM_HPP
#define TRACKINGSTREAM_HPP

#include "ColorSpace.hpp"
#include "DisplayFrameType.hpp"
#include "KFBallTracker.hpp"
#include "KFPrediction.hpp"

#include <QBasicTimer>
#include <QImage>

#include <opencv2/videoio.hpp>

class TrackingStream : public QObject
{
    Q_OBJECT

    QBasicTimer mTimer;
    QScopedPointer<cv::VideoCapture> mVideoHandle;
    int mFov;
    QSize mVideoSize;
    QSize mProjSize;
    cv::Mat mCameraMatrix;
    cv::Mat mRMatrix;
    cv::Mat mTVector;

    cv::Mat mFrame;

    DisplayFrameType mEmitFrameType;

    bool mStarted;
    bool mProjReady;
    bool mStreamPaused;

    int mFps;

    KFBallTracker mTracker;
    bool mDisplayVideo;

signals:
    // Signals for emitting frames for display
    void frameReady(const cv::Mat &, enum ColorSpace=TS_BGR);

    // Signals which emit information about tracking
    void ballSpotted(TrackingBall ball);
    void ballPredicted(KFPrediction pred);
    void ballProjSpotted(TrackingBall ball);
    void ballProjPredicted(KFPrediction pred);

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

    void predictionBall(KFPrediction pred);

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

    cv::Point2f imageToProjector(cv::Point2f imP, double z=0);
    QPointF imageToProjector(QPointF, double z=0);

    void updateProjectorCoordinates(std::vector<cv::Point2f> corners);

    void setProjSize(const QSize& projSize);

private:
    void timerEvent(QTimerEvent *);
};

#endif // TRACKINGSTREAM_HPP
