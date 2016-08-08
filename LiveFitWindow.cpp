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

#include <Qt>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QSettings>

#include "LiveFitWindow.hpp"

void LiveFitWindow::setupCamera()
{
  // Start the converter processes, and hook up the signals
  mFrameConverter.setProcessAll(false);
  mStreamThread.start();
  mConverterThread.start();
  mTrackingStream.moveToThread(&mStreamThread);
  mFrameConverter.moveToThread(&mConverterThread);
  mFrameConverter.connect(&mTrackingStream,
                          SIGNAL(frameReady(cv::Mat, enum ColorSpace)),
                          SLOT(processFrame(cv::Mat, enum ColorSpace)));

  setupSignalConnections();

  // Start a stream/video
  QMetaObject::invokeMethod(
        &mTrackingStream, "start",
            Q_ARG(int, 0));
}

void LiveFitWindow::setupSignalConnections()
{
  // Hook up the projector corner ui, and the tracking signals
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
  ui.projectorWindow->connect(&mTrackingStream,
                              SIGNAL(ballLost()),
                              SLOT(markDataStale()));


  // Hook up the settings panel UI, data display options
  mTrackingStream.connect(ui.clipTrackBox,
                          SIGNAL(toggled(bool)),
                          SLOT(changeClipTrack(bool)));
  ui.projectorWindow->connect(ui.showYtBox,
                          SIGNAL(toggled(bool)),
                          SLOT(toggleShowParam(bool)));
  ui.projectorWindow->connect(ui.showJetBox,
                              SIGNAL(toggled(bool)),
                              SLOT(toggleShowJet(bool)));
  ui.projectorWindow->connect(ui.colorMissBox,
                              SIGNAL(toggled(bool)),
                              SLOT(toggleColorMiss(bool)));
  ui.projectorWindow->connect(ui.verboseKFBox,
                              SIGNAL(toggled(bool)),
                              SLOT(toggleVerboseKF(bool)));

  ui.trackVideoWidget->connect(&mFrameConverter,
                          SIGNAL(imageReady(QImage)),
                          SLOT(setImage(QImage)));
  mFrameConverter.connect(ui.trackVideoWidget,
                    SIGNAL(resized(QSize)),
                    SLOT(setFrameSize(QSize)));

  // Hook up the settings panel UI, video/world options
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

  mTrackingStream.connect(ui.projWSpinBox,
                          SIGNAL(valueChanged(int)),
                          SLOT(changeProjWidth(int)));
  mTrackingStream.connect(ui.projHSpinBox,
                          SIGNAL(valueChanged(int)),
                          SLOT(changeProjHeight(int)));
  mTrackingStream.connect(ui.ballZDoubleSpinBox,
                          SIGNAL(valueChanged(double)),
                          SLOT(changeBallZ(double)));

  // Hook up the settings panel UI, frame type options
  mTrackingStream.connect(ui.frameComboBox,
                          SIGNAL(currentTextChanged(QString)),
                          SLOT(changeDisplayFrameType(QString)));

  // Send any corner messages we need to
  ui.trackVideoWidget->updateCorners();

  connect(ui.menu_Camera,
          SIGNAL(triggered(QAction*)),
          SLOT(sendCameraAction(QAction*)));

}

void LiveFitWindow::populateCameraMenu()
{
  // We don't do anything smart here, although we certainly could check
  // here for cameras that actually exist.

  {
    const int MAX_CAMERA_INDEX = 9;
    QAction *tempCamAction;
    int i;

    //ui.menu_Camera->clear();
    for (i = 0; i < MAX_CAMERA_INDEX; i++) {
      if (mTrackingStream.cameraValid(i)) {
        tempCamAction = new QAction(
                          QString::number(i),
                          ui.menu_Camera);
        tempCamAction->setData(QVariant(i));
        ui.menu_Camera->addAction(tempCamAction);
      }
    }
  }
}

LiveFitWindow::LiveFitWindow(QWidget *parent)
    : QMainWindow(parent) {
    mVideoSize = QSize(320, 240);

    ui.setupUi(this);

    QApplication::setOrganizationName("University of Georgia");
    QApplication::setOrganizationDomain("math.uga.edu");
    QApplication::setApplicationName("LiveFit");

    setGeometry(QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    size(),
                    QApplication::desktop()->availableGeometry()));

    setupCamera();
    readSettings();

    populateCameraMenu();
}

void LiveFitWindow::startStream(char* videoFile)
{
    QMetaObject::invokeMethod(&mTrackingStream, "start",
                              Q_ARG(QString, videoFile));
}

void LiveFitWindow::startStream(int cameraId)
{
    QMetaObject::invokeMethod(&mTrackingStream, "start",
                              Q_ARG(int, cameraId));
}

void LiveFitWindow::startCamera0()
{
    startStream(0);
}
void LiveFitWindow::startCamera1()
{
    startStream(1);
}
void LiveFitWindow::startCamera2()
{
  startStream(2);
}

void LiveFitWindow::sendCameraAction(QAction *act)
{
  if (!act->data().isNull()) {
    int idx = act->data().toInt();
    if (idx == -1) {
      // Refresh camera menu

    } else {
      // Load this camera
      startStream(idx);
    }
  }
}

void LiveFitWindow::writeSettings()
{
    QSettings settings;

    // We save all panel settings, plus a few more.

    settings.beginGroup("Presentation");
    settings.setValue("lockFit", ui.lockFitBox->isChecked());
    settings.setValue("clipToProjector", ui.clipTrackBox->isChecked());
    settings.setValue("showParametrization", ui.showYtBox->isChecked());
    settings.setValue("projectorGeometry", ui.projectorDock->frameGeometry());
    settings.setValue("showJet", ui.showJetBox->isChecked());
    settings.setValue("colorMiss", ui.colorMissBox->isChecked());
    settings.setValue("verboseKF", ui.verboseKFBox->isChecked());
    settings.setValue("showFit", ui.showFit->isChecked());
    settings.setValue("projPointRadius", ui.projPointRadius->value());
    settings.setValue("pointThickness", ui.pointThickness->value());
    settings.setValue("fitThickness", ui.fitThickness->value());
    settings.setValue("projFontSize", ui.fontSizeSpinBox->value());
    settings.endGroup();

    settings.beginGroup("Tracking");
    settings.setValue("ballBlobBlur", ui.blurSizeSpin->value());
    settings.setValue("ballBlobThresh", ui.threshValDoubleSpinBox->value());
    settings.setValue("ballBlobMinRadius", ui.minRadiusDoubleSpinBox->value());
    settings.setValue("ballBlobMaxRadius", ui.maxRadiusDoubleSpinBox->value());
    settings.setValue("ballGravConstant", ui.gravitySpin->value());
    settings.setValue("frameDisplayType", ui.frameComboBox->currentIndex());
    settings.setValue("ballXYCovariance",
                      ui.xYCovarianceDoubleSpinBox->value());
    settings.endGroup();

    settings.beginGroup("World");
    // Projector corners
    settings.beginWriteArray("projectorCorners", 4);
    QList<QPoint> corners = ui.trackVideoWidget->getCorners();
    for (int i = 0; i < corners.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("corner", corners[i]);
    }
    settings.endArray();

    // Settings panel options
    settings.setValue("ballZ", ui.ballZDoubleSpinBox->value());
    settings.setValue("projWorldW", ui.projWSpinBox->value());
    settings.setValue("projWorldH", ui.projHSpinBox->value());
    settings.setValue("cameraFOV", ui.fovSpinBox->value());
    settings.endGroup();
}

void LiveFitWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("Presentation");
    ui.lockFitBox->setChecked(settings.value("lockFit", false).toBool());
    ui.clipTrackBox->setChecked(settings.value("clipToProjector", true).toBool());
    ui.showYtBox->setChecked(settings.value("showParametrization", true).toBool());
    ui.showJetBox->setChecked(settings.value("showJet", true).toBool());
    ui.verboseKFBox->setChecked(settings.value("verboseKF", false).toBool());
    ui.colorMissBox->setChecked(settings.value("colorMiss", false).toBool());
    if (settings.contains("projectorGeometry")) {
        ui.projectorDock->setGeometry(settings.value("projectorGeometry").toRect());
    }
    ui.showFit->setChecked(settings.value("showFit", true).toBool());
    ui.projPointRadius->setValue(settings.value("projPointRadius", 30).toInt());
    ui.pointThickness->setValue(settings.value("pointThickness", 0.0).toDouble());
    ui.fitThickness->setValue(settings.value("fitThickness", 0.0).toDouble());
    ui.fontSizeSpinBox->setValue(settings.value("projFontSize", 12).toInt());
    settings.endGroup();

    settings.beginGroup("Tracking");
    ui.blurSizeSpin->setValue(
                settings.value("ballBlobBlur",
                               SETT_DEF_BLOB_BLUR).toDouble());
    ui.threshValDoubleSpinBox->setValue(
                settings.value("ballBlobThresh",
                               SETT_DEF_BLOB_THRESH).toDouble());
    ui.minRadiusDoubleSpinBox->setValue(
                settings.value("ballBlobMinRadius",
                               SETT_DEF_BLOB_MINRAD).toDouble());
    ui.maxRadiusDoubleSpinBox->setValue(
                settings.value("ballBlobMaxRadius",
                               SETT_DEF_BLOB_MAXRAD).toDouble());
    ui.gravitySpin->setValue(
                settings.value("ballGravConstant",
                               SETT_DEF_BLOB_GRAV).toDouble());
    ui.frameComboBox->setCurrentIndex(
                settings.value("frameDisplayType",
                               SETT_DEF_DISPLAYTYPE).toInt());
    ui.xYCovarianceDoubleSpinBox->setValue(
                settings.value("ballXYCovariance",
                               SETT_DEF_XYCOVAR).toDouble());
    settings.endGroup();

    settings.beginGroup("World");
    // Projector corners
    QList<QPoint> corners;
    int size = settings.beginReadArray("projectorCorners");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        corners.append(settings.value("corner").toPoint());
    }
    settings.endArray();
    ui.trackVideoWidget->setCorners(corners);

    // Settings panel options
    ui.projWSpinBox->setValue(
          settings.value("projWorldW",
                         SETT_DEF_PROJW).toInt());
    ui.projHSpinBox->setValue(
          settings.value("projWorldH",
                         SETT_DEF_PROJH).toInt());
    ui.ballZDoubleSpinBox->setValue(
          settings.value("ballZ",
                         SETT_DEF_BALLZ).toDouble());
    ui.fovSpinBox->setValue(
          settings.value("cameraFOV",
                         SETT_DEF_FOV).toInt());
    settings.endGroup();
}

void LiveFitWindow::closeEvent(QCloseEvent *) {
    QMetaObject::invokeMethod(&mTrackingStream, "stop",
                              Qt::BlockingQueuedConnection);
    QMetaObject::invokeMethod(&mFrameConverter, "stop",
                              Qt::BlockingQueuedConnection);
    mStreamThread.quit();
    mConverterThread.quit();
    mStreamThread.wait();
    mConverterThread.wait();

    writeSettings();
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

void LiveFitWindow::on_actionFind_Projector_Window_triggered()
{
    ui.projectorDock->setGeometry(50, 50, 400, 400);
}
