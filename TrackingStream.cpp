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
    setFov(SETT_DEF_FOV);
    setProjSize(QSize(SETT_DEF_PROJW, SETT_DEF_PROJH));
    mBallZ = SETT_DEF_BALLZ;

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
    connect(&mTracker,
            SIGNAL(ballLost()),
            SIGNAL(ballLost()));

    mDisplayVideo = true;
    mProjReady = false;
    mEmitFrameType = DF_THRESH;
}

TrackingStream::~TrackingStream()
{

}

void TrackingStream::setFov(int fov) {
    mFov = fov;

    refreshCameraMatrix();
}

void TrackingStream::refreshCameraMatrix() {
    double focalW, focalH;
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
    mCorners = corners;

    mProjCornersCamera = cv::Mat::zeros(4, 3, CV_64F);
    mProjCornersCamera.at<double>(3,0) = mProjSize.width();
    mProjCornersCamera.at<double>(1,1) = mProjSize.height();
    mProjCornersCamera.at<double>(2,0) = mProjSize.width();
    mProjCornersCamera.at<double>(2,1) = mProjSize.height();

    cv::solvePnP(mProjCornersCamera, mCorners,
                 mCameraMatrix, distCoeffs, rvec, mTVector);
    cv::Rodrigues(rvec, mRMatrix);

    mBallPlane.clear();
    mBallPlane.push_back(
                projectorToImage(
                    cv::Point3d(0,0,mBallZ)));
    mBallPlane.push_back(
                projectorToImage(
                    cv::Point3d(0,mProjSize.height(),mBallZ)));
    mBallPlane.push_back(
                projectorToImage(
                    cv::Point3d(mProjSize.width(),mProjSize.height(),mBallZ)));
    mBallPlane.push_back(
                projectorToImage(
                    cv::Point3d(mProjSize.width(),0,mBallZ)));

    mProjScreen = QPolygonF(mBallPlane.size());
    for (unsigned int corner_i = 0; corner_i < mBallPlane.size(); corner_i++) {
        mProjScreen << QPointF(
                           mBallPlane.at(corner_i).x, mBallPlane.at(corner_i).y);
    }

    mTracker.setClipShape(mProjScreen);

    mProjReady = true;
}

void TrackingStream::refreshProjectorMatrices() {
    if (!mProjReady) {
        return;
    }

    cv::Mat distCoeffs;
    cv::Mat rvec(3,1,CV_64F);

    mProjCornersCamera = cv::Mat::zeros(4, 3, CV_64F);
    mProjCornersCamera.at<double>(3,0) = mProjSize.width();
    mProjCornersCamera.at<double>(1,1) = mProjSize.height();
    mProjCornersCamera.at<double>(2,0) = mProjSize.width();
    mProjCornersCamera.at<double>(2,1) = mProjSize.height();

    // Find pose of the camera (i.e. rotation, translation)
    cv::solvePnP(mProjCornersCamera, mCorners,
                 mCameraMatrix, distCoeffs, rvec, mTVector);

    // Change rotation vector rvec to 3x3 mat rMatrix
    cv::Rodrigues(rvec, mRMatrix);

    // Create the "ball plane" in image coordinates
    mBallPlane.clear();
    mBallPlane.push_back(
                projectorToImage(cv::Point3d(0,0,mBallZ)));
    mBallPlane.push_back(
                projectorToImage(cv::Point3d(0,mProjSize.height(),mBallZ)));
    mBallPlane.push_back(
                projectorToImage(
                    cv::Point3d(mProjSize.width(),mProjSize.height(),mBallZ)));
    mBallPlane.push_back(
                projectorToImage(cv::Point3d(mProjSize.width(),0,mBallZ)));

    mProjScreen = QPolygonF(mBallPlane.size());
    for (unsigned int corner_i = 0; corner_i < mBallPlane.size(); corner_i++) {
        mProjScreen << QPointF(
                           mBallPlane.at(corner_i).x, mBallPlane.at(corner_i).y);
    }

    mTracker.setClipShape(mProjScreen);
}

cv::Point2f TrackingStream::imageToProjector(cv::Point2f imP, double z)
{
    double s, x, y;

    // We use these inverses twice
    cv::Mat rinv = mRMatrix.inv(); // Is it equivalent to transpose?
    cv::Mat cinv = mCameraMatrix.inv();

    //std::cout << "\n\n";
    //std::cout << rinv << "\n";

    // Matrix (u, v, 1)
    cv::Mat uvmat = cv::Mat::ones(3,1,CV_64F);
    uvmat.at<double>(0,0) = imP.x;
    uvmat.at<double>(1,0) = imP.y;

    //std::cout << uvmat << "\n";

    s = z + ((cv::Mat)(rinv*mTVector)).at<double>(2,0);
    s /= ((cv::Mat)(rinv*cinv*uvmat)).at<double>(2,0);

    // wcMat holds world coordinates
    cv::Mat wcMat = rinv*(s*cinv*uvmat - mTVector);

    x = wcMat.at<double>(0,0);
    y = wcMat.at<double>(1,0);

    //std::cout << x << ", " << y << "\n";

    return cv::Point2f(x,y);
}

QPointF TrackingStream::imageToProjector(QPointF imP, double z)
{
    cv::Point2f pP = imageToProjector(cv::Point2f(imP.x(), imP.y()), z);
    return QPointF(pP.x, pP.y);
}

cv::Point2f TrackingStream::projectorToImage(cv::Point3d prP) {
    cv::Mat P = cv::Mat(prP);
    cv::Mat Q = mCameraMatrix*(mRMatrix*P + mTVector);

    return cv::Point2f(Q.at<double>(0,0)/Q.at<double>(2,0),
                       Q.at<double>(1,0)/Q.at<double>(2,0));
}


void TrackingStream::setProjSize(const QSize& projSize)
{
    mProjSize = projSize;
    refreshProjectorMatrices();
}
void TrackingStream::start(int cam)
{
    if (!mVideoHandle.isNull() && mVideoHandle->isOpened())
    {
        mVideoHandle->release();
    }
    mVideoHandle.reset(new cv::VideoCapture(cam));
    mFps = 0;

    if (mVideoHandle->isOpened()) {
        // Set the size of the video stream from the camera. We presently
        // force it to be 320x240, since higher sizes have trouble with the
        // USB bandwidths on some laptops
        mVideoHandle->set(CV_CAP_PROP_FRAME_WIDTH, 320);
        mVideoHandle->set(CV_CAP_PROP_FRAME_HEIGHT, 240);

        // Set the video size, refresh the camera matrix (depends on video size)
        mVideoSize = QSize(320, 240);
        refreshCameraMatrix();

        // Start the timer, and emit the started signal
        mTimer.start(0, this);
        emit started();
    }
}

void TrackingStream::start(QString fname)
{
    mVideoHandle.reset(new cv::VideoCapture(fname.toStdString()));
    mFps = mVideoHandle->get(cv::CAP_PROP_FPS);

    if (mVideoHandle->isOpened()) {
        // Set the video size, refresh the camera matrix (depends on video size)
        mVideoSize = QSize(mVideoHandle->get(cv::CAP_PROP_FRAME_WIDTH),
                           mVideoHandle->get(cv::CAP_PROP_FRAME_HEIGHT));
        refreshCameraMatrix();

        // Start the timer, and emit the started signal
        mTimer.start(1000/mFps, this);
        emit started();
    }
}

void TrackingStream::stop()
{
    mTimer.stop();
}

void TrackingStream::pauseStream()
{
    mStreamPaused = !mStreamPaused;
}

void TrackingStream::changeProjectorCorners(std::vector<cv::Point2f> corners)
{
    updateProjectorCoordinates(corners);
}

void TrackingStream::changeClipTrack(bool checked)
{
    mClipTrack = checked;
    mTracker.toggleClip(checked);
}

void TrackingStream::predictionBall(KFPrediction pred)
{
    if (!mProjReady) {
        return;
    }

    // Convert the prediction to projector coordinates
    KFPrediction projPred = KFPrediction(pred);
    //projPred.setCenter(imageToProjector(pred.bbox().center()));
    projPred.setTopLeft(imageToProjector(pred.bbox().topLeft(), mBallZ));
    projPred.setBottomRight(imageToProjector(pred.bbox().bottomRight(), mBallZ));
    emit ballProjPredicted(projPred);
}

void TrackingStream::changeProjWidth(int width)
{
    mProjSize.setWidth(width);
    refreshProjectorMatrices();
}

void TrackingStream::changeProjHeight(int height)
{
    mProjSize.setHeight(height);
    refreshProjectorMatrices();
}

void TrackingStream::changeBallZ(double z)
{
    mBallZ = z;
    refreshProjectorMatrices();
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
        for (int i = 0; i < mBallPlane.size(); i++)
        cv::circle(mat, mBallPlane[i], 2, cv::Scalar(0,0,255));
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

    if (!mStreamPaused && mVideoHandle->isOpened()) {

        if (!mVideoHandle->read(mFrame)) {
            mTimer.stop();
            return;
        }

        double t = cv::getTickCount()/cv::getTickFrequency();
        mTracker.updateTimeState(t);

        QMap<double, TrackingBall> balls = mTracker.processNextFrame(mFrame, t);
        if (!balls.isEmpty()) {
            //std::cout << balls.first().x() << ", " << balls.first().y() << "\n";
            //std::cout << "-----\n";

            emit ballSpotted(balls.first());
        }
    }

    processVideoFrame(mFrame);
}

