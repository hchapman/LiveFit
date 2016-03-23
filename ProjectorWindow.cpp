#include "ProjectorWindow.hpp"

ProjectorWindow::ProjectorWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setStyleSheet("* {background-color: rgb(0,0,0);}");

    projSize = QSize(800, 600);
}

