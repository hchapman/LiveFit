#ifndef PROJECTORWINDOW_HPP
#define PROJECTORWINDOW_HPP

#include "KFPrediction.hpp"
#include "TrackingBall.hpp"

#include <QOpenGLWidget>
#include <QWidget>

/**
 * @brief The ProjectorWindow class
 * A display for projector coordinates on the projector.
 * The idea is that this displays full screen behind the demonstration
 * and shows the ball "where it is"
 */
class ProjectorWindow : public QOpenGLWidget
{
    Q_OBJECT
    QSize mProjSize;

    QList<TrackingBall> mBalls;
    QList<KFPrediction> mPreds;
    QList<KFPrediction> mFitPreds;

    double mFitLineX[2];
    double mFitParabolaY[3];

    QList<KFPrediction> mMarkedPoints;

    bool mDataStale;

    bool mLockFit;
    int mNumPointFit;
    bool mFitLocked;

    bool mWaitTilFall;
    int mMinFallSpeed;

    bool mFitPoints;
    bool mShowFit;
    bool mShowParam;
    bool mShowJet;
    bool mColorMiss;
    bool mVerboseKF;
    bool mShowParabola;

    int mPointRadius;
    double mPointThickness;
    double mFitThickness;
public:
    explicit ProjectorWindow(QWidget* parent = 0);
    QRectF relRectToWindow(QRectF rect);
    QPointF relPointToWindow(QPointF pt);

public slots:
    void pushBall(TrackingBall ball);
    void pushPred(KFPrediction pred);
    void markDataStale();

    void toggleShowFit(bool);
    void toggleShowParam(bool);
    void toggleShowJet(bool);
    void toggleColorMiss(bool);
    void toggleVerboseKF(bool);

    void toggleShowParabola(bool showParabola);
    void setPointRadius(int pointRadius);
    void setPointThickness(double pointThickness);
    void setFitThickness(double fitThickness);

    void setMinFallSpeed(int minFallSpeed);
    void toggleWaitTilFall(bool waitTilFall);
protected:
    void paintEvent(QPaintEvent* ev);
};

#endif // PROJECTORWINDOW_HPP
