#ifndef LIVEFITWINDOW_HPP
#define LIVEFITWINDOW_HPP

#include "FrameConverter.hpp"
#include "TrackingStream.hpp"

#include <QSize>
#include <QMainWindow>
#include <QThread>

#include <ui_LiveFitWindow.h>

/**
 * @brief The LiveFitWindow class
 * The window which manages all logic for the LiveFit application
 */
class LiveFitWindow : public QMainWindow {
    Q_OBJECT
private:
    QSize mVideoSize;

    void createMenus();
    void chooseVideoFile();
    void setupUi();
    void setupCamera();
    void displayVideoStream();

public:
    explicit LiveFitWindow(QWidget* parent = 0);
    LiveFitWindow(char* videoFile);
    LiveFitWindow(int cameraId);

public slots:
    void startStream(char *videoFile);
    void startStream(int cameraId);

    void startCamera0();
    void startCamera1();
    void startCamera2();

protected:
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent* ev);
private slots:

    void on_pauseButton_clicked();

    void on_action_Open_triggered();

private:
    Ui::liveFitWindow ui;

    TrackingStream mTrackingStream;
    FrameConverter mFrameConverter;

    QThread mStreamThread, mConverterThread;

};

#endif // LIVEFITWINDOW_HPP
