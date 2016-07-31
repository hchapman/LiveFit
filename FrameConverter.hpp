#ifndef FRAMECONVERTER_HPP
#define FRAMECONVERTER_HPP

#include <QBasicTimer>
#include <QObject>
#include <QSize>

#include "ColorSpace.hpp"

#include <opencv2/core/mat.hpp>

/**
 * @brief Recieves frames (as cv::Mat), processes them (ideally asynchronously)
 * and emits them as a QImage (i.e. for use in a QWidget)
 */
class FrameConverter : public QObject
{
  Q_OBJECT

  /** Process a frame at each tick of this timer */
  QBasicTimer mTimer;
  /** The current frame to process into a QImage */
  cv::Mat currentFrame;
  /** The current ColorSpace of the frame we are processing */
  ColorSpace currentCS;
  /** Whether we should process all frames, or drop some to keep framerate */
  bool processAll;
  /** The resolution of the frames that we are processing */
  QSize frameSize;

  /** Delete a cv::Mat object */
  static void matDeleter(void *mat) { delete static_cast<cv::Mat*>(mat); }

public:
  explicit FrameConverter(QObject *parent = 0);

  /** Set whether we should process all frames, or drop some */
  void setProcessAll(bool all) { processAll = all; }

  ~FrameConverter();
signals:
  /** Emitted when we have a QImage from processing ready for display */
  void imageReady(const QImage &);

public slots:
  /** Add a new frame to be processed; if not processAll this frame may yet be dropped. */
  void processFrame(const cv::Mat &frame, enum ColorSpace);
  /** Set the resolution of the frames we are processing */
  void setFrameSize(QSize size);
  /** Stop processing frames */
  void stop();
private:
  /** Queue a new frame to be processed (depends on processAll if it gets processed). */
  void queue(const cv::Mat & frame, ColorSpace cs);
  /** Actually process a frame. Resultant QImage gets emitted by imageReady() */
  void process(cv::Mat frame, ColorSpace cs);
  /** Called each time that mTimer ticks... we call process on the queue in here. */
  void timerEvent(QTimerEvent *ev);
};

#endif // FRAMECONVERTER_HPP
