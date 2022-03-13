#include "QHexWidget.h"
#include <QApplication>

int main(int argc, char* argv[]) {

    QApplication a(argc, argv);

    QHexWidget w;
    w.show();

    return a.exec();
}

