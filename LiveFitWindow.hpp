#ifndef LIVEFITWINDOW_HPP
#define LIVEFITWINDOW_HPP

#include "FrameConverter.hpp"
#include "TrackingStream.hpp"

#include <QSize>
#include <QMainWindow>
#include <QThread>

#include <ui_LiveFitWindow.h>

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

protected:
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
