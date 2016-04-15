SOURCES += main.cpp \
    TrackVideoWidget.cpp \
    TrackingStream.cpp \
    ProjectorDragCorner.cpp \
    FrameConverter.cpp \
    ProjectorWindow.cpp \
    KFBallTracker.cpp \
    TrackingBall.cpp \
    KFPrediction.cpp
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
    DisplayFrameType.hpp

CONFIG += qt5
QT += \
    widgets \
    gui \

FORMS += \
    LiveFitWindow.ui \
    TrackVideoWidget.ui

QT_CONFIG -= no-pkg-config
unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv
