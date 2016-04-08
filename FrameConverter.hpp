#ifndef FRAMECONVERTER_HPP
#define FRAMECONVERTER_HPP

#include <QBasicTimer>
#include <QObject>
#include <QSize>

#include <opencv2/core/mat.hpp>

class FrameConverter : public QObject
{
    Q_OBJECT

    QBasicTimer mTimer;
    cv::Mat currentFrame;
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
    void processFrame(const cv::Mat &frame);
    void setFrameSize(QSize size);

private:
    void queue(const cv::Mat & frame);
    void process(cv::Mat frame);
    void timerEvent(QTimerEvent *ev);
};

#endif // FRAMECONVERTER_HPP
