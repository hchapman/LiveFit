#include <Qt>
#include <QApplication>
#include <QDesktopWidget>

#include "LiveFitWindow.hpp"

void LiveFitWindow::setupCamera()
{
    mFrameConverter.setProcessAll(false);
    mStreamThread.start();
    mConverterThread.start();
    mTrackingStream.moveToThread(&mStreamThread);
    mFrameConverter.moveToThread(&mConverterThread);
    mFrameConverter.connect(&mTrackingStream,
                      SIGNAL(frameReady(cv::Mat)),
                      SLOT(processFrame(cv::Mat)));

    ui.trackVideoWidget->connect(&mFrameConverter,
                            SIGNAL(imageReady(QImage)),
                            SLOT(setImage(QImage)));
    mFrameConverter.connect(ui.trackVideoWidget,
                      SIGNAL(resized(QSize)),
                      SLOT(setFrameSize(QSize)));

    mTrackingStream.connect(ui.trackVideoWidget,
                   SIGNAL(cornersChanged(std::vector<cv::Point2f>)),
                   SLOT(changeProjectorCorners(std::vector<cv::Point2f>)));

    QMetaObject::invokeMethod(&mTrackingStream, "start");
}

LiveFitWindow::LiveFitWindow(QWidget *parent)
    : QMainWindow(parent) {
    mVideoSize = QSize(320, 240);

    ui.setupUi(this);

    QApplication::setOrganizationName("University of Georgia");
    QApplication::setOrganizationDomain("math.uga.edu");
    QApplication::setApplicationName("Live Ball Tracker");

    setGeometry(QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    size(),
                    QApplication::desktop()->availableGeometry()));
    setupCamera();
}

void LiveFitWindow::closeEvent(QCloseEvent *ev) {
    mStreamThread.quit();
    mConverterThread.quit();
    mStreamThread.wait();
    mConverterThread.wait();
}
