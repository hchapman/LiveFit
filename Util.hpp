#ifndef UTIL_HPP
#define UTIL_HPP

#include "KFPrediction.hpp"

#include <gsl/gsl_multifit.h>
#include <stdbool.h>
#include <math.h>
#include <QList>
#include <QPoint>

bool polynomialFitKF(int degree,
                     QList<KFPrediction> preds, double *store);

#endif // UTIL_HPP
