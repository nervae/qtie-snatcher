#include <QApplication>

#include "mainwindow.hpp"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QIcon icon;
    icon.addFile(":/Icons/AppIcon32");
    icon.addFile(":/Icons/AppIcon128");
    app.setWindowIcon(icon);

    MainWindow main;
    main.show();

    return app.exec();
}
