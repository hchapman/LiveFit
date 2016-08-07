SOURCES += main.cpp \
    TrackVideoWidget.cpp \
    TrackingStream.cpp \
    ProjectorDragCorner.cpp \
    FrameConverter.cpp \
    ProjectorWindow.cpp \
    KFBallTracker.cpp \
    TrackingBall.cpp \
    KFPrediction.cpp \
    Util.cpp \
    KalmanFilterPlus.cpp \
    BallTrackingFilter.cpp \
    TrackingFilter.cpp \
    PersonTrackingFilter.cpp \
    ColorComboWidget.cpp
SOURCES += LiveFitWindow.cpp

HEADERS += LiveFitWindow.hpp \
    TrackVideoWidget.hpp \
    TrackingStream.hpp \
    ProjectorDragCorner.hpp \
    FrameConverter.hpp \
    ProjectorWindow.hpp \
    KFBallTracker.hpp \
    TrackingBall.hpp \
    KFPrediction.hpp \
    ColorSpace.hpp \
    DisplayFrameType.hpp \
    Util.hpp \
    KalmanFilterPlus.hpp \
    BallTrackingFilter.hpp \
    TrackingFilter.hpp \
    PersonTrackingFilter.hpp \
    ColorComboWidget.hpp

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

CONFIG += \
    qt5 \
    c++11
QT += \
    widgets \
    gui \

FORMS += \
    LiveFitWindow.ui \
    TrackVideoWidget.ui \
    ColorComboWidget.ui

QT_CONFIG -= no-pkg-config
unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv gsl

win32 {
  DEFINES += NO_GSL
  CONFIG(debug, debug|release) {
    LIBS += -lopencv_core300d -lopencv_calib3d300d -lopencv_highgui300d \
            -lopencv_video300d -lopencv_videoio300d -lopencv_imgproc300d
  }
  else {
    LIBS += -lopencv_core300 -lopencv_calib3d300 -lopencv_highgui300 \
            -lopencv_video300 -lopencv_videoio300 -lopencv_imgproc300
  }
}

DISTFILES += \
    Doxyfile
