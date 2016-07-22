#ifndef PROJECTORWINDOW_HPP
#define PROJECTORWINDOW_HPP

#include "KFPrediction.hpp"
#include "TrackingBall.hpp"

#include <QOpenGLWidget>
#include <QWidget>

/**
 * @brief A display widget for projector coordinates on the projector.
 *
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

    /** @brief linear coefficients of x(t) equation, (b,m) */
    double mFitLineX[2];
    /** @brief quadratic coefficients of y(t) equation, (C,B,A) */
    double mFitParabolaY[3];

    /** @brief list of points marked and selected to be solved for x(t), y(t) */
    QList<KFPrediction> mMarkedPoints;

    bool mDataStale; /** @brief whether data should be cleared on next track */

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

    int mFontSize = 12; /** @brief Data display font size in pt */

    int mPointRadius; /** @brief Radius of circles denoting tracked ball */
    double mPointThickness; /** @brief Thickness of circle lines for balls */
    double mFitThickness; /** @brief Thickness of the track fit parabola */
public:
    explicit ProjectorWindow(QWidget* parent = 0);

    /** @brief convert a relative rect in projector to window coords */
    QRectF relRectToWindow(QRectF rect);
    /** @brief convert a relative point in the projector to window coords */
    QPointF relPointToWindow(QPointF pt);

public slots:
    /** @brief push a raw tracked ball to projector */
    void pushBall(TrackingBall ball);
    /** @brief push a predicted ball (i.e. a KF prediction) to projector */
    void pushPred(KFPrediction pred);
    /** @brief mark data stale, i.e. ready to wipe on next track */
    void markDataStale();

    /** @brief toggle whether to show fit information for tracked ball */
    void toggleShowFit(bool);
    /** @brief toggle whether to show parametrized equations y(t), x(t) */
    void toggleShowParam(bool);
    /** @brief toggle whether to show jet ((dx,dy)) of tracked ball */
    void toggleShowJet(bool);
    /** @brief toggle color difference between seen/unseen KF hits */
    void toggleColorMiss(bool);
    /** @brief toggle verbose display [size, etc.] of track/predict info */
    void toggleVerboseKF(bool);

    /** @brief set whether we should show the fit parabola */
    void toggleShowParabola(bool showParabola);
    /** @brief set radius of ball track circles */
    void setPointRadius(int pointRadius);
    /** @brief set thickness of ball track marks */
    void setPointThickness(double pointThickness);
    /** @brief set thickness of fit parabola */
    void setFitThickness(double fitThickness);
    /** @brief set font size for data display */
    void setFontSize(int fontSize);

    /** @brief set minimum y-velocity for lock */
    void setMinFallSpeed(int minFallSpeed);
    /** @brief set whether we should wait until given y-velocity to lock */
    void toggleWaitTilFall(bool waitTilFall);
protected:
    /**
     * @brief Draw the data on the projector screen widget
     *
     * All drawing logic for the widget goes in here!
     * @param ev
     */
    void paintEvent(QPaintEvent* ev);
};

#endif // PROJECTORWINDOW_HPP
