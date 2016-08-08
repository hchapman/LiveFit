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

#include <QApplication>

#include "DisplayFrameType.hpp"
#include "LiveFitWindow.hpp"

Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(TrackingBall)
Q_DECLARE_METATYPE(enum ColorSpace)
Q_DECLARE_METATYPE(enum DisplayFrameType)
Q_DECLARE_METATYPE(KFPrediction)
Q_DECLARE_METATYPE(std::vector<cv::Point2f>)

int main(int argc, char** argv) {
    qRegisterMetaType<cv::Mat>();
    qRegisterMetaType<TrackingBall>();
    qRegisterMetaType<KFPrediction>();
    qRegisterMetaType<enum ColorSpace>();
    qRegisterMetaType<enum DisplayFrameType>();
    qRegisterMetaType<std::vector<cv::Point2f> >();

    QApplication liveFitApp(argc, argv);
    LiveFitWindow liveFitWin;

    liveFitWin.show();

    return liveFitApp.exec();
}
