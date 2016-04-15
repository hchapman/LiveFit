#ifndef PROJECTORWINDOW_HPP
#define PROJECTORWINDOW_HPP

#include "KFPrediction.hpp"
#include "TrackingBall.hpp"

#include <QOpenGLWidget>
#include <QWidget>

class ProjectorWindow : public QOpenGLWidget
{
    Q_OBJECT
    QSize projSize;

    QList<TrackingBall> mBalls;
    QList<KFPrediction> mPreds;
public:
    explicit ProjectorWindow(QWidget* parent = 0);
public slots:
    void pushBall(TrackingBall ball);
    void pushPred(KFPrediction pred);
protected:
    void paintEvent(QPaintEvent* ev);
};

#endif // PROJECTORWINDOW_HPP
