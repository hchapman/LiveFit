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

#ifndef PROJECTORDRAGCORNER_HPP
#define PROJECTORDRAGCORNER_HPP

#include <QWidget>

/**
 * @brief A draggable widget for managing where the projector corners lie in the
 * video frame
 */
class ProjectorDragCorner : public QWidget
{
  Q_OBJECT

  /** The (extrinsic) index of this corner in [UL,BL,BR,UL] */
  int cornerIndex;
public:
  /** Default constructor */
  explicit ProjectorDragCorner(QWidget *parent = 0);

signals:
  /** Sent when a "move" action of this drag corner is finished */
  void moveCompleted(QPoint loc);

public slots:

protected:
  /** Paint this drag corner */
  void paintEvent(QPaintEvent* event);

  /** Called when a mouse press happens; "select" and consider drag */
  void mousePressEvent(QMouseEvent* event);
  /** Called when a mouse move happens; consider drag if selected and moved */
  void mouseMoveEvent(QMouseEvent* event);
  /** Called when a mouse release happens; consider drop after drag */
  void mouseReleaseEvent(QMouseEvent* event);
private:

  /** Where this drag was initiated */
  QPoint dragStartPos;
  /** Whether this corner is selected */
  bool selected = false;
  /** Whether this corner is being dragged */
  bool dragging = false;
  /** Display/click radius of this widget */
  int radius;
};

#endif // PROJECTORDRAGCORNER_HPP
