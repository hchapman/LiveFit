#ifndef KFPREDICTION_HPP
#define KFPREDICTION_HPP

#include <opencv2/core.hpp>

#include <QPoint>
#include <QRect>

class KFPrediction
{
    QRectF mBbox;
    QPointF mJet;

    double mT;
    double mDt;
    bool mSeen;

    double mConfidence;

public:
    KFPrediction();
    explicit KFPrediction(cv::Mat kfStatePre, cv::Mat kfCov, double t, double dt, bool seen);
    KFPrediction(const KFPrediction &k);

    QRectF bbox() const { return mBbox; }
    QPointF jet() const { return mJet; }
    double dt() const { return mDt; }
    double t() const {return mT; }
    bool seen() const { return mSeen; }
    double confidence() const { return mConfidence; }

    void setCenter(QPointF pos) { mBbox.moveCenter(pos); }

    void setTopLeft(QPointF pos) { mBbox.setTopLeft(pos); }
    void setBottomRight(QPointF pos) { mBbox.setBottomRight(pos); }
};

#endif // KFPREDICTION_HPP
