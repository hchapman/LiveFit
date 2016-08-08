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

#include "TrackVideoWidget.hpp"

#include <QMouseEvent>
#include <QPainter>

TrackVideoWidget::TrackVideoWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_OpaquePaintEvent);

    connect(ui.ulCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(ulCornerDropped(QPoint)));
    connect(ui.blCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(blCornerDropped(QPoint)));
    connect(ui.brCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(brCornerDropped(QPoint)));
    connect(ui.urCorner,
            SIGNAL(moveCompleted(QPoint)),
            SLOT(urCornerDropped(QPoint)));
}
#include <QDebug>
void TrackVideoWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    QPen ballPen = QPen((QColor(0, 0, 255)));
    QPen kfSeenPen = QPen((QColor(255,0,0)));
    QPen kfMissPen = QPen((QColor(255,255,0)));

    painter.begin(this);

    painter.drawImage(0, 0, currentFrame);

    painter.setPen(ballPen);
    QList<TrackingBall>::const_iterator ballsIter;
    for (ballsIter = mBalls.begin(); ballsIter != mBalls.end(); ++ballsIter) {
        painter.drawEllipse(
                    (QPoint)(*ballsIter).center()*2, (int)(*ballsIter).r()*2, (int)(*ballsIter).r()*2);
    }

    QList<KFPrediction>::const_iterator predsIter;
    for (predsIter = mPreds.begin(); predsIter != mPreds.end(); ++predsIter) {
        if ((*predsIter).seen()) {
            //qDebug() << sqrt((*predsIter).confidence());
            painter.setPen(QPen(QColor(255*fmin(1,0.3+sqrt((*predsIter).confidence())),0,0)));
            //painter.setPen(kfSeenPen);
        } else {
            painter.setPen(kfMissPen);
        }
        painter.drawRect(QRectF((*predsIter).bbox().topLeft()*2,
                               (*predsIter).bbox().bottomRight()*2));
        painter.drawLine(
                    (*predsIter).bbox().center()*2,
                    ((*predsIter).bbox().center()*2+
                     (QPointF)(*predsIter).jet()*(*predsIter).dt()*2));
    }

    painter.end();
}

void TrackVideoWidget::resizeEvent(QResizeEvent* event)
{
    emit resized(size());
}

void TrackVideoWidget::setImage(const QImage& img)
{
    currentFrame = img;
    update();
}

void TrackVideoWidget::pushBall(TrackingBall ball)
{
    mBalls.append(ball);
    if (mBalls.size() > 10) {
        mBalls.removeFirst();
    }
}

void TrackVideoWidget::setCorners(QList<QPoint> corners)
{
    if (corners.size() < 4) {
        return;
    }

    ui.ulCorner->move(corners[0]);
    ui.urCorner->move(corners[1]);
    ui.brCorner->move(corners[2]);
    ui.blCorner->move(corners[3]);
    updateCorners();
}

void TrackVideoWidget::pushPred(KFPrediction pred) {
    mPreds.append(pred);
    if (mPreds.size() > 10) {
        mPreds.removeFirst();
    }
}

void TrackVideoWidget::ulCornerDropped(QPoint point)
{
    updateCorners();
}

void TrackVideoWidget::blCornerDropped(QPoint point)
{
    updateCorners();
}

void TrackVideoWidget::brCornerDropped(QPoint point)
{
    updateCorners();
}

void TrackVideoWidget::urCornerDropped(QPoint point)
{
    updateCorners();
}
#include <iostream>
#include <QDebug>
void TrackVideoWidget::updateCorners()
{
    std::vector<cv::Point2f> corners;

    //corners.push_back(cv::Point2f(0,0));
    //corners.push_back(cv::Point2f(0,240));
    //corners.push_back(cv::Point2f(320,240));
    //corners.push_back(cv::Point2f(320,0));

    corners.push_back(cv::Point2f(ui.ulCorner->geometry().center().x()/2,
                                  ui.ulCorner->geometry().center().y()/2));
    corners.push_back(cv::Point2f(ui.blCorner->geometry().center().x()/2,
                                  ui.blCorner->geometry().center().y()/2));
    corners.push_back(cv::Point2f(ui.brCorner->geometry().center().x()/2,
                                  ui.brCorner->geometry().center().y()/2));
    corners.push_back(cv::Point2f(ui.urCorner->geometry().center().x()/2,
                                  ui.urCorner->geometry().center().y()/2));
    //qDebug() << ui.brCorner->geometry();

    emit cornersChanged(corners);
}

QList<QPoint> TrackVideoWidget::getCorners()
{
    QList<QPoint> c;
    c.append(ui.ulCorner->geometry().topLeft());
    c.append(ui.urCorner->geometry().topLeft());
    c.append(ui.brCorner->geometry().topLeft());
    c.append(ui.blCorner->geometry().topLeft());
    return c;
}

