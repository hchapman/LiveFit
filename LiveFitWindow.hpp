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

#ifndef LIVEFITWINDOW_HPP
#define LIVEFITWINDOW_HPP

#include "FrameConverter.hpp"
#include "TrackingStream.hpp"

#include <QSize>
#include <QMainWindow>
#include <QThread>

#include <ui_LiveFitWindow.h>

/**
 * @brief The window which manages all logic for the LiveFit application
 */
class LiveFitWindow : public QMainWindow {
  Q_OBJECT
private:
  /** Pixel size of the video stream*/
  QSize mVideoSize;

  /* TODO: I'm not actually sure if any of these are called */
  void createMenus();
  void chooseVideoFile();
  void setupUi();
  void displayVideoStream();
  /* ~~~~ */

  /** Set up the video stream and the converter, as well as requisite signals.
   *
   * We currently also set up a bunch of auxiliary signals here which should be
   * moved out to their own respective method */
  void setupCamera();

  /** Set up the ProjectorWindow which displays information to the class
   */
  void setupSignalConnections();

  /**
   * @brief Populate the file>camera menu
   */
  void populateCameraMenu();
public:
  /** Default constructor; default to webcam 0 */
  explicit LiveFitWindow(QWidget* parent = 0);
  /** Create a LiveFitWindow with video stream at videoFile */
  LiveFitWindow(char* videoFile);
  /** Create a LiveFitWindow with video stream from camera at index cameraId */
  LiveFitWindow(int cameraId);

public slots:
  /** Start a new stream with video from videoFile */
  void startStream(char *videoFile);
  /** Start a new stream with video from camera at index cameraId */
  void startStream(int cameraId);

  /** Equivalent to startStream(0) */
  void startCamera0();
  /** Equivalent to startStream(1) */
  void startCamera1();
  /** Equivalent to startStream(2) */
  void startCamera2();

  /** Handle a camera-related QAction, like those in file>camera */
  void sendCameraAction(QAction *);

protected:
  /** Write this application's QSettings */
  void writeSettings();
  /** Read in this application's QSettings */
  void readSettings();

  /** Called when this window is closed; destruct all necessary objects and
      close streams, etc cleanly */
  void closeEvent(QCloseEvent* ev);
private slots:

  /** Called when the pauseButton is clicked */
  void on_pauseButton_clicked();
  /** Called when the Open action is triggered (ie file>open)*/
  void on_action_Open_triggered();

  void on_actionFind_Projector_Window_triggered();

private:
  /** This window's QDesigner ui, LiveFitWindow.ui */
  Ui::liveFitWindow ui;

  /** The TrackingStream object which manages the current video stream and
      tracking */
  TrackingStream mTrackingStream;
  /** The FrameConverter which takes incoming frames from the stream and
      converts them into QImages to be sent to appropriate display widgets */
  FrameConverter mFrameConverter;

  /** A QThread for the video stream */
  QThread mStreamThread;
  /** A QThread for the video converter */
  QThread mConverterThread;

};

#endif // LIVEFITWINDOW_HPP
