#include <QPixmap>

#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()) {
   this->ui->setupUi(this);

   this->ui->label->setPixmap(QPixmap(""));
   this->ui->label->setScaledContents(true);
}
