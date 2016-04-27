#ifndef KFPREDICTION_HPP
#define KFPREDICTION_HPP

#include <opencv2/core.hpp>

#include <QPoint>
#include <QRect>

class KFPrediction
{
    QRectF mBbox;
    QPointF mJet;

    double mDt;
    bool mSeen;

public:
    KFPrediction();
    explicit KFPrediction(cv::Mat kfStatePre, double dt, bool seen);
    KFPrediction(const KFPrediction &k);

    QRectF bbox() const { return mBbox; }
    QPointF jet() const { return mJet; }
    double dt() const { return mDt; }
    bool seen() const { return mSeen; }

    void setCenter(QPointF pos) { mBbox.moveCenter(pos); }

    void setTopLeft(QPointF pos) { mBbox.setTopLeft(pos); }
    void setBottomRight(QPointF pos) { mBbox.setBottomRight(pos); }
};

#endif // KFPREDICTION_HPP
