/*
 *  LiveFit
 *  Copyright (C) 2016 The University of Georgia
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef KFPREDICTION_HPP
#define KFPREDICTION_HPP

#include <opencv2/core.hpp>

#include <QPoint>
#include <QRect>

/**
 * @brief Object which represents a prediction from the Kalman filter.
 *
 * Essentially decorates state data, but also holds onto the dT
 * from the previous timestep
 */
class KFPrediction
{
  /** Bounding box of this prediction */
  QRectF mBbox;
  /** <dx/dt, dy/dt> vector */
  QPointF mJet;

  /** Time at which this prediction was recorded */
  double mT;
  /** Change in time from step prior to detection */
  double mDt;
  /** Whether this prediction is based on a track or not */
  bool mSeen;

  /** Measure of confidence; KF distance to expected location */
    double mConfidence;

public:
  KFPrediction();
  /**
   * @brief Create a KFPrediction from prediction data
   * @param kfStatePre KF Predicted state
   * @param kfCov KF Covariance matrix
   * @param t Current time state at prediction
   * @param dt Previous change in time between states
   * @param seen Whether the ball was spotted or not
   */
  explicit KFPrediction(cv::Mat kfStatePre, cv::Mat kfCov, double t, double dt, bool seen);
  /** Copy constructor */
  KFPrediction(const KFPrediction &k);

  /** Get the bounding box */
  QRectF bbox() const { return mBbox; }
  /** Get the jet */
  QPointF jet() const { return mJet; }
  /** Get the timestep change */
  double dt() const { return mDt; }
  /** Get the time of prediction */
  double t() const {return mT; }
  /** Get whether the ball was seen */
  bool seen() const { return mSeen; }
  /** Get the confidence of this prediction */
  double confidence() const { return mConfidence; }

  /** Set the center of the bounding box of this prediction */
  void setCenter(QPointF pos) { mBbox.moveCenter(pos); }

  /** Set the top left of the bounding box */
  void setTopLeft(QPointF pos) { mBbox.setTopLeft(pos); }
  /** Set the bottom right of the bounding box */
  void setBottomRight(QPointF pos) { mBbox.setBottomRight(pos); }
};

#endif // KFPREDICTION_HPP
