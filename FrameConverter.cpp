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

#include <QImage>
#include <QTimer>
#include <QTimerEvent>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "FrameConverter.hpp"

FrameConverter::FrameConverter(QObject *parent) : QObject(parent)
{

}

FrameConverter::~FrameConverter() {

}

void FrameConverter::processFrame(const cv::Mat& frame, ColorSpace cs)
{
    if (processAll) {
        process(frame, cs);
    } else {
        queue(frame, cs);
    }
}

void FrameConverter::setFrameSize(QSize size)
{
    frameSize = size;
}

void FrameConverter::queue(const cv::Mat &frame, ColorSpace cs)
{
    currentFrame = frame;
    currentCS = cs;
    if (!mTimer.isActive()) {
        mTimer.start(0, this);
    }
}

void FrameConverter::process(cv::Mat frame, ColorSpace cs)
{
    cv::resize(frame, frame,
               cv::Size(frameSize.width(), frameSize.height()),
               0.3, 0.3, cv::INTER_AREA);
    if (cs == TS_GRAY) {
        cv::cvtColor(frame, frame, CV_GRAY2RGB);
    } else if (cs == TS_BGR) {
        cv::cvtColor(frame, frame, CV_BGR2RGB);
    }
    const QImage image(frame.data, frame.cols, frame.rows, frame.step,
                       QImage::Format_RGB888, &matDeleter,
                       new cv::Mat(frame));
    emit imageReady(image);
}

void FrameConverter::timerEvent(QTimerEvent* ev)
{
    if (ev->timerId() != mTimer.timerId()) {
        return;
    }
    process(currentFrame, currentCS);
    currentFrame.release();
    mTimer.stop();
}

void FrameConverter::stop() {
    mTimer.stop();
}
