#include <QApplication>

#include "LiveFitWindow.hpp"

Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(TrackingBall)

int main(int argc, char** argv) {
    qRegisterMetaType<cv::Mat>();
    qRegisterMetaType<TrackingBall>();

    QApplication liveFitApp(argc, argv);
    LiveFitWindow liveFitWin;

    liveFitWin.show();

    return liveFitApp.exec();
}
