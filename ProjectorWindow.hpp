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

  /** The size of the projector (in inches) */
  QSize mProjSize;

  /** A list of raw spotted balls in the projector */
  QList<TrackingBall> mBalls;
  /** A list of predictions of ball location in the projector */
  QList<KFPrediction> mPreds;
  /** A [sub]list of predictions which are used for the fit curve */
  QList<KFPrediction> mFitPreds;

  /** @brief linear coefficients of x(t) equation, (b,m) */
  double mFitLineX[2];
  /** @brief quadratic coefficients of y(t) equation, (C,B,A) */
  double mFitParabolaY[3];

  /** @brief list of points marked and selected to be solved for x(t), y(t) */
  QList<KFPrediction> mMarkedPoints;

  /** @brief whether data should be cleared on next track */
  bool mDataStale;

  /** Whether to lock the fit curve and track until manual clear */
  bool mLockFit;
  /** Number of tracked points to wait for until the curve is fit */
  int mNumPointFit;
  /** Whether the fit is 'locked' for the time being, until reset either
   * manually or after sufficient time and another track */
  bool mFitLocked;

  /** Whether we should wait until the ball is falling to lock/fit a curve */
  bool mWaitTilFall;
  /** Minimum fall speed to determine that the ball is "falling" */
  int mMinFallSpeed;

  /** Whether we fit a degree-d curve exactly to (d+1) points, or use a regression */
  bool mFitPoints;
  /** Whether we show the fit or not */
  bool mShowFit;
  /** Whether we show the parametrized equation for the fit or not */
  bool mShowParam;
  /** Whether we show (dx,dy) vectors on the display */
  bool mShowJet;
  /** Whether we should color differently the predictions for unseen balls */
  bool mColorMiss;
  /** Whether we should include width, height, etc from KF prediction data in
   * display */
  bool mVerboseKF;
  /** Whether we should show the fit parabola curve */
  bool mShowParabola;

  int mFontSize = 12; /**< @brief Data display font size in pt */

  int mPointRadius; /**< @brief Radius of circles denoting tracked ball */
  double mPointThickness; /**< @brief Thickness of circle lines for balls */
  double mFitThickness; /**< @brief Thickness of the track fit parabola */
public:
  explicit ProjectorWindow(QWidget* parent = 0);

  /** @brief convert a relative rect in projector to window coords */
  QRectF relRectToWindow(QRectF rect);
  /** @brief convert a relative point in the projector to window coords */
  QPointF relPointToWindow(QPointF pt);

public slots:
  /** @brief push a raw tracked ball to projector */
  void pushBall(TrackingBall ball);
  /** @brief push a predicted ball (ie a KF prediction) to projector */
  void pushPred(KFPrediction pred);
  /** @brief mark data stale, ie ready to wipe on next track */
  void markDataStale();

  /** @brief toggle whether to show fit information for tracked ball */
  void toggleShowFit(bool);
  /** @brief toggle whether to show parametrized equations y(t), x(t) */
  void toggleShowParam(bool);
  /** @brief toggle whether to show jet ((dx,dy)) of tracked ball */
  void toggleShowJet(bool);
  /** @brief toggle color difference between seen/unseen KF hits */
  void toggleColorMiss(bool);
  /** @brief toggle verbose display [size, etc] of track/predict info */
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
   * @param ev The QPaintEvent for this paint
   */
  void paintEvent(QPaintEvent* ev);
};

#endif // PROJECTORWINDOW_HPP
