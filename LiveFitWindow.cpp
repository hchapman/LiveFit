#include <Qt>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>

#include "LiveFitWindow.hpp"

void LiveFitWindow::setupCamera()
{
    mFrameConverter.setProcessAll(false);
    mStreamThread.start();
    mConverterThread.start();
    mTrackingStream.moveToThread(&mStreamThread);
    mFrameConverter.moveToThread(&mConverterThread);
    mFrameConverter.connect(&mTrackingStream,
                      SIGNAL(frameReady(cv::Mat, enum ColorSpace)),
                      SLOT(processFrame(cv::Mat, enum ColorSpace)));

    ui.trackVideoWidget->connect(&mFrameConverter,
                            SIGNAL(imageReady(QImage)),
                            SLOT(setImage(QImage)));
    mFrameConverter.connect(ui.trackVideoWidget,
                      SIGNAL(resized(QSize)),
                      SLOT(setFrameSize(QSize)));

    mTrackingStream.connect(ui.trackVideoWidget,
                   SIGNAL(cornersChanged(std::vector<cv::Point2f>)),
                   SLOT(changeProjectorCorners(std::vector<cv::Point2f>)));
    ui.trackVideoWidget->connect(&mTrackingStream,
                                 SIGNAL(ballSpotted(TrackingBall)),
                                 SLOT(pushBall(TrackingBall)));
    ui.trackVideoWidget->connect(&mTrackingStream,
                                 SIGNAL(ballPredicted(KFPrediction)),
                                 SLOT(pushPred(KFPrediction)));
    ui.projectorWindow->connect(&mTrackingStream,
                                SIGNAL(ballProjPredicted(KFPrediction)),
                                SLOT(pushPred(KFPrediction)));

    mTrackingStream.connect(ui.blurSizeSpin,
                            SIGNAL(valueChanged(double)),
                            SLOT(changeBlurSize(double)));
    mTrackingStream.connect(ui.minRadiusDoubleSpinBox,
                            SIGNAL(valueChanged(double)),
                            SLOT(changeMinRadius(double)));
    mTrackingStream.connect(ui.maxRadiusDoubleSpinBox,
                            SIGNAL(valueChanged(double)),
                            SLOT(changeMaxRadius(double)));
    mTrackingStream.connect(ui.threshValDoubleSpinBox,
                            SIGNAL(valueChanged(double)),
                            SLOT(changeThreshVal(double)));
    mTrackingStream.connect(ui.gravitySpin,
                            SIGNAL(valueChanged(double)),
                            SLOT(changeGravConstant(double)));
    mTrackingStream.connect(ui.xYCovarianceDoubleSpinBox,
                            SIGNAL(valueChanged(double)),
                            SLOT(changeXYCovariance(double)));

    mTrackingStream.connect(ui.frameComboBox,
                            SIGNAL(currentTextChanged(QString)),
                            SLOT(changeDisplayFrameType(QString)));

    ui.trackVideoWidget->updateCorners();

    QMetaObject::invokeMethod(&mTrackingStream, "start",
    //                          Q_ARG(int, 0));
                              Q_ARG(QString, "/home/harrison/Dropbox/interception project/classroom-red-light.webm"));
    //                          Q_ARG(QString, "/Users/hchapman/Dropbox/interception project/classroom-red-light.webm"));
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
    QMetaObject::invokeMethod(&mTrackingStream, "stop",
                              Qt::BlockingQueuedConnection);
    QMetaObject::invokeMethod(&mFrameConverter, "stop",
                              Qt::BlockingQueuedConnection);
    mStreamThread.quit();
    mConverterThread.quit();
    mStreamThread.wait();
    mConverterThread.wait();
}

void LiveFitWindow::on_pauseButton_clicked()
{
    QMetaObject::invokeMethod(&mTrackingStream, "pauseStream");
}

void LiveFitWindow::on_action_Open_triggered()
{
    QString fname = QFileDialog::getOpenFileName(
                this, "Open video", "", "Video Files (*.mov *.avi *.webm *.mkv)");
    if (fname != NULL) {
        QMetaObject::invokeMethod(&mTrackingStream, "start",
                                  Q_ARG(QString, fname));
    }
}
