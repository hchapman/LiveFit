#include "KFPrediction.hpp"
#include "KFBallTracker.hpp"

#include <QDebug>

#include <iostream>


KFPrediction::KFPrediction()
{

}

KFPrediction::KFPrediction(const KFPrediction& k)
{
    mBbox = QRect(k.bbox());
    mJet = QPoint(k.jet());
}

KFPrediction::KFPrediction(cv::Mat kfStatePre, double dt, bool seen)
{
    double x, y, w, h, dx, dy;
    x = kfStatePre.at<double>(BallKFState::x);
    y = kfStatePre.at<double>(BallKFState::y);
    w = kfStatePre.at<double>(BallKFState::w);
    h = kfStatePre.at<double>(BallKFState::h);
    dx = kfStatePre.at<double>(BallKFState::dx);
    dy = kfStatePre.at<double>(BallKFState::dy);

    //std::cout << "dxdy" << dx << " " << dy << "\n";

    mDt = dt;
    mSeen = seen;
    //std::cout << seen << "\n";
    mBbox = QRect(QPoint(x-w/2, y-h/2), QSize(w, h));
    mJet = QPoint(dx, dy);
    qDebug() << mJet;
}
