#ifndef UTIL_HPP
#define UTIL_HPP

/** @file Util.hpp
 */

#include "KFPrediction.hpp"

#include <gsl/gsl_multifit.h>
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
