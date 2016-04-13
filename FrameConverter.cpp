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

void FrameConverter::processFrame(const cv::Mat& frame)
{
    if (processAll) {
        process(frame);
    } else {
        queue(frame);
    }
}

void FrameConverter::setFrameSize(QSize size)
{
    frameSize = size;
}

void FrameConverter::queue(const cv::Mat &frame)
{
    currentFrame = frame;
    if (!mTimer.isActive()) {
        mTimer.start(0, this);
    }
}

void FrameConverter::process(cv::Mat frame)
{
    cv::resize(frame, frame,
               cv::Size(frameSize.width(), frameSize.height()),
               0.3, 0.3, cv::INTER_AREA);
    cv::cvtColor(frame, frame, CV_BGR2RGB);
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
    process(currentFrame);
    currentFrame.release();
    mTimer.stop();
}

void FrameConverter::stop() {
    mTimer.stop();
}
