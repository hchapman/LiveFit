#include "ColorComboWidget.hpp"
#include "ui_ColorComboWidget.h"

ColorComboWidget::ColorComboWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ColorComboWidget)
{
  ui->setupUi(this);
}

ColorComboWidget::~ColorComboWidget()
{
  delete ui;
}
