#ifndef PROJECTORWINDOW_HPP
#define PROJECTORWINDOW_HPP

#include <QOpenGLWidget>
#include <QWidget>

class ProjectorWindow : public QOpenGLWidget
{
    Q_OBJECT
    QSize projSize;
public:
    explicit ProjectorWindow(QWidget* parent = 0);
};

#endif // PROJECTORWINDOW_HPP
