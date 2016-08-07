#ifndef COLORCOMBOWIDGET_HPP
#define COLORCOMBOWIDGET_HPP

#include <QWidget>

namespace Ui {
class ColorComboWidget;
}

class ColorComboWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ColorComboWidget(QWidget *parent = 0);
  ~ColorComboWidget();

private:
  Ui::ColorComboWidget *ui;
};

#endif // COLORCOMBOWIDGET_HPP
