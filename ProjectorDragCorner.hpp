#ifndef PROJECTORDRAGCORNER_HPP
#define PROJECTORDRAGCORNER_HPP

#include <QWidget>

/**
 * @brief The ProjectorDragCorner class
 * A draggable widget for managing where the projector corners
 * lie in the video frame
 */
class ProjectorDragCorner : public QWidget
{
    Q_OBJECT
    int cornerIndex;
public:
    explicit ProjectorDragCorner(QWidget *parent = 0);

signals:
    void moveCompleted(QPoint loc);

public slots:

protected:
    void paintEvent(QPaintEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
private:
    QPoint dragStartPos;
    bool selected;
    bool dragging;
    int radius;
};

#endif // PROJECTORDRAGCORNER_HPP
