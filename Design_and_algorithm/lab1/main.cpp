#include <QApplication>
#include "canvas.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Canvas window;
    window.setWindowTitle("Counter Example");
    window.resize(300, 200);
    window.show();

    return app.exec();
}
