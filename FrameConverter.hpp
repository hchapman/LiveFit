#ifndef FRAMECONVERTER_HPP
#define FRAMECONVERTER_HPP

#include <QBasicTimer>
#include <QObject>
#include <QSize>

#include "ColorSpace.hpp"

#include <opencv2/core/mat.hpp>

class FrameConverter : public QObject
{
    Q_OBJECT

    QBasicTimer mTimer;
    cv::Mat currentFrame;
    ColorSpace currentCS;
    bool processAll;
    QSize frameSize;

    static void matDeleter(void *mat) { delete static_cast<cv::Mat*>(mat); }

public:
    explicit FrameConverter(QObject *parent = 0);

    void setProcessAll(bool all) { processAll = all; }

    ~FrameConverter();
signals:
    void imageReady(const QImage &);

public slots:
    void processFrame(const cv::Mat &frame, enum ColorSpace);
    void setFrameSize(QSize size);
    void stop();
private:
    void queue(const cv::Mat & frame, ColorSpace cs);
    void process(cv::Mat frame, ColorSpace cs);
    void timerEvent(QTimerEvent *ev);
};

#endif // FRAMECONVERTER_HPP
