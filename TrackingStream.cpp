#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include <QTimerEvent>

#include <opencv2/core/cvstd.hpp>

#include "TrackVideoWidget.hpp"
#include "TrackingStream.hpp"

TrackingStream::TrackingStream(QObject* parent) :
    QObject(parent)
{
    setFov(56);
    setProjSize(QSize(50, 50));
    connect(&mTracker,
            SIGNAL(ballPredicted(KFPrediction)),
            SIGNAL(ballPredicted(KFPrediction)));
    connect(&mTracker,
            SIGNAL(ballPredicted(KFPrediction)),
            SLOT(predictionBall(KFPrediction)));
    connect(&mTracker,
            SIGNAL(threshReady(cv::Mat,ColorSpace)),
            SLOT(processThresh(cv::Mat,ColorSpace)));
    connect(&mTracker,
            SIGNAL(contourReady(cv::Mat,ColorSpace)),
            SLOT(processContour(cv::Mat,ColorSpace)));

    mDisplayVideo = false;
    mProjReady = false;
    mEmitFrameType = DF_THRESH;
}

TrackingStream::~TrackingStream()
{

}

void TrackingStream::setFov(int fov) {
    double focalW, focalH;
    mFov = fov;
    focalW = (0.5 * mVideoSize.width() /
              tan(0.5 * mFov * M_PI / 180));
    focalH = (0.5 * mVideoSize.height() /
              tan(0.5 * mFov * M_PI *
                  (mVideoSize.height() * 1.0 / mVideoSize.width()) /
                  180));
    mCameraMatrix = cv::Mat::eye(3,3,CV_64F);
    mCameraMatrix.at<double>(0,0) = focalW;
    mCameraMatrix.at<double>(1,1) = focalH;
    mCameraMatrix.at<double>(0,2) = mVideoSize.width()/2;
    mCameraMatrix.at<double>(1,2) = mVideoSize.height()/2;
}

void TrackingStream::updateProjectorCoordinates(
        std::vector<cv::Point2f> corners) {
    cv::Mat rvec(3,1,CV_64F);
    cv::Mat distCoeffs;
    mTVector = cv::Mat(3,1,CV_64F);

    cv::Mat projCornersCamera = cv::Mat::zeros(4, 3, CV_64F);
    projCornersCamera.at<double>(UL_CORNER,0) = mProjSize.width();
    projCornersCamera.at<double>(BL_CORNER,1) = mProjSize.height();
    projCornersCamera.at<double>(UR_CORNER,0) = mProjSize.width();
    projCornersCamera.at<double>(BR_CORNER,1) = mProjSize.height();

    cv::solvePnP(projCornersCamera, corners,
                 mCameraMatrix, distCoeffs, rvec, mTVector);
    cv::Rodrigues(rvec, mRMatrix);

    mProjReady = true;
}

cv::Point2f TrackingStream::imageToProjector(cv::Point2f imP, double z)
{
    double s, x, y;

    // We use these inverses twice
    cv::Mat rinv = mRMatrix.inv();
    cv::Mat cinv = mCameraMatrix.inv();

    // Matrix (u, v, 1)
    cv::Mat uvmat = cv::Mat::ones(3,1,CV_64F);
    uvmat.at<double>(0,0) = imP.x;
    uvmat.at<double>(1,0) = imP.y;

    s = z + ((cv::Mat)(rinv*mTVector)).at<double>(2,0);
    s /= ((cv::Mat)(rinv*cinv*uvmat)).at<double>(2,0);

    // wcMat holds world coordinates
    cv::Mat wcMat = rinv*(s*cinv*uvmat - mTVector);

    x = wcMat.at<double>(0,0);
    y = wcMat.at<double>(1,0);

    return cv::Point2f(x,y);
}

QPoint TrackingStream::imageToProjector(QPoint imP, double z)
{
    cv::Point2f pP = imageToProjector(cv::Point2f(imP.x(), imP.y()), z);
    return QPoint((int)pP.x, (int)pP.y);
}


void TrackingStream::setProjSize(const QSize& projSize)
{
    mProjSize = projSize;
}
void TrackingStream::start(int cam)
{
    mVideoHandle.reset(new cv::VideoCapture(cam));
    mFps = 0;

    if (mVideoHandle->isOpened()) {
        mTimer.start(0, this);
        emit started();
    }
}

void TrackingStream::start(QString fname)
{
    mVideoHandle.reset(new cv::VideoCapture(fname.toStdString()));
    mFps = mVideoHandle->get(cv::CAP_PROP_FPS);

    if (mVideoHandle->isOpened()) {
        mTimer.start(1000/mFps, this);
        emit started();
    }
}

void TrackingStream::stop()
{
    mTimer.stop();
}

void TrackingStream::changeProjectorCorners(std::vector<cv::Point2f> corners)
{
    updateProjectorCoordinates(corners);
}

void TrackingStream::predictionBall(KFPrediction pred)
{
    return;
    if (!mProjReady) {
        return;
    }

    // Convert the prediction to projector coordinates
    KFPrediction projPred = KFPrediction(pred);
    projPred.setCenter(imageToProjector(pred.bbox().center()));
    emit ballProjPredicted(projPred);
}

void TrackingStream::changeBlurSize(double blurSize)
{
    mTracker.setBlurSize(blurSize);
}

void TrackingStream::changeThreshVal(double thresh)
{
    mTracker.setThreshVal(thresh);
}

void TrackingStream::changeMinRadius(double radius)
{
    mTracker.setMinRadius(radius);
}

void TrackingStream::changeMaxRadius(double radius)
{
    mTracker.setMaxRadius(radius);
}

void TrackingStream::changeGravConstant(double g)
{
    mTracker.setGravConstant(g);
}

void TrackingStream::changeXYCovariance(double sigma)
{
    mTracker.setXYCovariance(sigma);
}

void TrackingStream::changeDisplayFrameType(QString s) {
    if (s == "Video") {
        mEmitFrameType = DF_VIDEO;
    } else if (s == "ThreshDiff") {
        mEmitFrameType = DF_THRESH;
    } else if (s == "Contour") {
        mEmitFrameType = DF_CONTOUR;
    }
}

void TrackingStream::processVideoFrame(const cv::Mat& mat, ColorSpace cs)
{
    if (mEmitFrameType == DF_VIDEO) {
        emit frameReady(mat, cs);
    }
}

void TrackingStream::processThresh(const cv::Mat& mat, ColorSpace cs)
{
    if (mEmitFrameType == DF_THRESH) {
        emit frameReady(mat, cs);
    }
}

void TrackingStream::processBlur(const cv::Mat& mat, ColorSpace cs)
{
    if (mEmitFrameType == DF_BLUR) {
        emit frameReady(mat, cs);
    }
}

void TrackingStream::processContour(const cv::Mat& mat, ColorSpace cs)
{
    if (mEmitFrameType == DF_CONTOUR) {
        emit frameReady(mat, cs);
    }
}

void TrackingStream::timerEvent(QTimerEvent* ev)
{
    if (ev->timerId() != mTimer.timerId()) {
        return;
    }

    cv::Mat frame;
    if (!mVideoHandle->read(frame)) {
        mTimer.stop();
        return;
    }

    double t = cv::getTickCount()/cv::getTickFrequency();
    mTracker.updateTimeState(t);

    QMap<double, TrackingBall> balls = mTracker.processNextFrame(frame, t);
    if (!balls.isEmpty()) {
        //std::cout << balls.first().x() << ", " << balls.first().y() << "\n";
        //std::cout << "-----\n";

        emit ballSpotted(balls.first());
    }

    processVideoFrame(frame);
}

