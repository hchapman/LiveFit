#include "KFPrediction.hpp"
#include "KFBallTracker.hpp"

#include <QDebug>

#include <iostream>


KFPrediction::KFPrediction()
{

}

KFPrediction::KFPrediction(const KFPrediction& k)
{
    mBbox = QRectF(k.mBbox);
    mJet = QPointF(k.mJet);
    mSeen = k.mSeen;
    mDt = k.mDt;
}

KFPrediction::KFPrediction(cv::Mat kfStatePre, double dt, bool seen)
{
    double x, y, w, h, dx, dy;

    mDt = dt;

    x = kfStatePre.at<double>(BallKFState::x);
    y = kfStatePre.at<double>(BallKFState::y);
    w = kfStatePre.at<double>(BallKFState::w);
    h = kfStatePre.at<double>(BallKFState::h);
    dx = kfStatePre.at<double>(BallKFState::dx);
    dy = kfStatePre.at<double>(BallKFState::dy);

    //std::cout << "dxdy" << dx << " " << dy << "\n";

    mSeen = seen;
    //std::cout << seen << "\n";
    mBbox = QRectF(QPointF(x-w/2, y-h/2), QSizeF(w, h));
    mJet = QPointF(dx, dy);
    //qDebug() << mJet;
}
