#pragma once

#include <memory>
#include <QMainWindow>

#include "ui_mainwindow.h"

namespace Ui {

    class MainWindow;

} // namespace Ui

class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        virtual ~MainWindow() = default;

    private slots:
        void quit_button_clicked(bool val) {
            this->ui->quit_button->setText("Clicked");
            printf("Clicked: %d\n", val);
        }

    private:
        std::unique_ptr<Ui::MainWindow> ui;
};
