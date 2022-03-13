#include "Example.h"

#include "QHexWidget.h"

Example::Example(QWidget *parent) : QWidget(parent) {

    setFixedSize(800, 400);

    QHexWidget* widget = new QHexWidget(this);
    widget->setGeometry(16, 16, 64, 32);
}

Example::~Example() {
}

