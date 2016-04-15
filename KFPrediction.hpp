#ifndef KFPREDICTION_HPP
#define KFPREDICTION_HPP

#include <opencv2/core.hpp>

#include <QPoint>
#include <QRect>

class KFPrediction
{
    QRect mBbox;
    QPoint mJet;
    double mDt;
    bool mSeen;

public:
    KFPrediction();
    KFPrediction(const KFPrediction &k);
    KFPrediction(cv::Mat kfStatePre, double dt, bool seen);

    QRect bbox() const { return mBbox; }
    QPoint jet() const { return mJet; }
    double dt() const { return mDt; }
    bool seen() const { return mSeen; }

    void setCenter(QPoint pos) { mBbox.moveCenter(pos); }
};

#endif // KFPREDICTION_HPP
