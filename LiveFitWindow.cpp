#include <Qt>
#include <QApplication>
#include <QDesktopWidget>

#include "LiveFitWindow.hpp"

void LiveFitWindow::setupCamera()
{
    converter.setProcessAll(false);
    captureThread.start();
    converterThread.start();
    stream.moveToThread(&captureThread);
    converter.moveToThread(&converterThread);
    converter.connect(&stream,
                      SIGNAL(frameReady(cv::Mat)),
                      SLOT(processFrame(cv::Mat)));

    ui.videoStream->connect(&converter,
                            SIGNAL(imageReady(QImage)),
                            SLOT(setImage(QImage)));
    converter.connect(ui.videoStream,
                      SIGNAL(resized(QSize)),
                      SLOT(setFrameSize(QSize)));

    stream.connect(ui.videoStream,
                   SIGNAL(cornersChanged(std::vector<cv::Point2f>)),
                   SLOT(changeProjectorCorners(std::vector<cv::Point2f>)));

    QMetaObject::invokeMethod(&stream, "start");
}

LiveFitWindow::LiveFitWindow(QWidget *parent)
    : QMainWindow(parent) {
    videoSize = QSize(320, 240);

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
    captureThread.quit();
    converterThread.quit();
    captureThread.wait();
    converterThread.wait();
}

void LiveFitWindow::on_quitButton_clicked()
{

}
