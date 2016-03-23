#include <QApplication>

#include "LiveFitWindow.hpp"
Q_DECLARE_METATYPE(cv::Mat)

int main(int argc, char** argv) {
    qRegisterMetaType<cv::Mat>();

    QApplication liveFitApp(argc, argv);
    LiveFitWindow liveFitWin;

    liveFitWin.show();

    return liveFitApp.exec();
}
