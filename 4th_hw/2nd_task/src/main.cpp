#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("PointMotion");
    app.setApplicationDisplayName("Движение материальной точки");

    PointMotion::MainWindow w;
    w.show();
    return app.exec();
}
