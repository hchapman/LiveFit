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

#ifndef UTIL_HPP
#define UTIL_HPP

/** @file Util.hpp
 */

#include "KFPrediction.hpp"

#ifndef NO_GSL
// GSL library is not available for VS2015, etc.
// On Unix-based systems, we use GSL here for polynomial regression
// On Windows, we don't necessarily have access to gsl without
//  much stress. So, we just don't implement regression fittng
//  for the time being (this is "ok" as in-class students will
//  tentatively just use the 3-point fit algorithm)
#include <gsl/gsl_multifit.h>
#endif

#include <stdbool.h>
#include <math.h>
#include <QList>
#include <QPoint>

/**
 * @brief Fit a polynomial equation to (x,t) points
 * @param degree Degree of polynomial to fit (usu: 1)
 * @param tStart Initial time value to fit from
 * @param preds List of KFPrediction with (x,y,t) data
 * @param store Resultant array of degree components with the coefficients; <b,M>
 * @return bool Whether the fit was successful
 */
bool polynomialFitKFX(int degree, double tStart,
                      QList<KFPrediction> preds, double *store);

/**
 * @brief Fit a polynomial equation to (y,t) points
 * @param degree Degree of polynomial to fit (usu: 2)
 * @param tStart Initial time value to fit from
 * @param preds List of KFPrediction with (x,y,t) data
 * @param store Resultant array of degree components with the coefficients; <C,B,A>
 * @return bool Whether the fit was successful
 */
bool polynomialFitKFY(int degree, double tStart,
                     QList<KFPrediction> preds, double *store);

#endif // UTIL_HPP
