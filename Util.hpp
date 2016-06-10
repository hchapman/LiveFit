#ifndef UTIL_HPP
#define UTIL_HPP

#include "KFPrediction.hpp"

#include <gsl/gsl_multifit.h>
#include <stdbool.h>
#include <math.h>
#include <QList>
#include <QPoint>

bool polynomialFitKFX(int degree, double tStart,
                     QList<KFPrediction> preds, double *store);
bool polynomialFitKFY(int degree, double tStart,
                     QList<KFPrediction> preds, double *store);

#endif // UTIL_HPP
