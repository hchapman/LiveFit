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
    mT = k.mT;
}

KFPrediction::KFPrediction(cv::Mat kfStatePre, cv::Mat kfCov, double t, double dt, bool seen)
{
    double x, y, w, h, dx, dy;

    mT = t;
    mDt = dt;

    x = kfStatePre.at<double>(BallKFState::x);
    y = kfStatePre.at<double>(BallKFState::y);
    w = kfStatePre.at<double>(BallKFState::w);
    h = kfStatePre.at<double>(BallKFState::h);
    dx = kfStatePre.at<double>(BallKFState::dx);
    dy = kfStatePre.at<double>(BallKFState::dy);

    //std::cout << "dxdy" << dx << " " << dy << "\n";

    mConfidence = cv::trace(kfCov)[0]/kfCov.size[0];

    mSeen = seen;
    //std::cout << seen << "\n";
    mBbox = QRectF(QPointF(x-w/2, y-h/2), QSizeF(w, h));
    mJet = QPointF(dx, dy);
    //qDebug() << mJet;
}
