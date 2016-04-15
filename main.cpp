#include <QApplication>

#include "DisplayFrameType.hpp"
#include "LiveFitWindow.hpp"

Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(TrackingBall)
Q_DECLARE_METATYPE(KFPrediction)
Q_DECLARE_METATYPE(enum ColorSpace)
Q_DECLARE_METATYPE(enum DisplayFrameType)

int main(int argc, char** argv) {
    qRegisterMetaType<cv::Mat>();
    qRegisterMetaType<TrackingBall>();
    qRegisterMetaType<KFPrediction>();
    qRegisterMetaType<enum ColorSpace>();
    qRegisterMetaType<enum DisplayFrameType>();

    QApplication liveFitApp(argc, argv);
    LiveFitWindow liveFitWin;

    liveFitWin.show();

    return liveFitApp.exec();
}
