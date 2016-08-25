#include <QApplication>
#include "window.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    Window window;
    window.show();

    return app.exec();
}
