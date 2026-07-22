#include <QApplication>
#include <QIcon>
#include "calculator.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("CalcX");
    app.setApplicationDisplayName("CalcX");
    app.setApplicationVersion("1.0.0");

    app.setWindowIcon(QIcon(":/src/icons/Calcx.svg"));

    Calculator calc;
    calc.show();

    return app.exec();
}
