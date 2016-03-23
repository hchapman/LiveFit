#include "ProjectorDragCorner.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QApplication>

ProjectorDragCorner::ProjectorDragCorner(QWidget *parent) : QWidget(parent)
{
    selected = false;
    dragging = false;
    radius = 10;

    resize(QSize(radius*2, radius*2));
}

void ProjectorDragCorner::paintEvent(QPaintEvent *event) {
    QPainter painter;
    QPen drawPen;

    if (dragging) {
        drawPen = QPen(QColor(255, 0, 0));
    } else {
        drawPen = QPen(QColor(0, 255, 0));
    }

    painter.begin(this);

    painter.setPen(drawPen);
    painter.drawEllipse(rect().adjusted(1,1,-1,-1));
    painter.drawPie(rect().adjusted(1,1,-1,-1),
                    -90*16+90*16*(property("cornerIndex").toInt()),
                    90*16);

    painter.end();
}

void ProjectorDragCorner::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (!selected) {
            dragStartPos = event->pos();
            selected = true;
        } else {
            selected = false;
        }
        update();
    } else {
        QWidget::mousePressEvent(event);
        return;
    }
}

void ProjectorDragCorner::mouseMoveEvent(QMouseEvent *event) {
    if (!selected ||
            !(event->buttons() & Qt::LeftButton)) {
        QWidget::mouseMoveEvent(event);
        return;
    } else if (dragging || (event->pos() - dragStartPos).manhattanLength() >=
               QApplication::startDragDistance()) {
        dragging = true;
        move(mapToParent(event->pos()) - QPoint(radius, radius));
    }
}

void ProjectorDragCorner::mouseReleaseEvent(QMouseEvent *event) {
    if (dragging && event->button() == Qt::LeftButton) {
        dragging = false;
        selected = false;

        // Alert any listeners that we have 'dropped'
        emit moveCompleted(pos() - QPoint(radius, radius));
        update();
    }
}
