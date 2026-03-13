#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("RandomWalk");
    app.setApplicationDisplayName("Случайное блуждание — Нетрезвый пешеход");
    app.setOrganizationName("StochasticLab");

    RandomWalk::MainWindow window;
    window.show();

    return app.exec();
}
