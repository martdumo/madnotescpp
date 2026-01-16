#include <QApplication>
#include "ui/MainWindow.h"
#include "ui/ThemeEngine.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Apply the global theme
    ThemeEngine::apply();

    MainWindow w;
    w.show();

    return a.exec();
}
