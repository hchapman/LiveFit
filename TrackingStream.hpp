#ifndef TRACKINGSTREAM_HPP
#define TRACKINGSTREAM_HPP

#include "KFBallTracker.hpp"

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

    KFBallTracker mTracker;

signals:
    void frameReady(const cv::Mat &);
    void started();

public slots:
    void start(int cam = 0);
    void stop();
    void changeProjectorCorners(std::vector<cv::Point2f> corners);

public:
    TrackingStream(QObject *parent = 0);
    ~TrackingStream();

    void startProcess();

    void setFov(int fov);

    cv::Point2f imageToProjector(cv::Point2f imP, double z=0);
    void updateProjectorCoordinates(std::vector<cv::Point2f> corners);
private:
    void timerEvent(QTimerEvent *);
};

#endif // TRACKINGSTREAM_HPP
