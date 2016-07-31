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
  bool selected;
  /** Whether this corner is being dragged */
  bool dragging;
  /** Display/click radius of this widget */
  int radius;
};

#endif // PROJECTORDRAGCORNER_HPP
