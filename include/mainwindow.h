#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boost/asio.hpp>
#include <QInputDialog>
#include "client.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:

    void on_actionConnect_to_Localhost_triggered();

    bool on_pushButton_2_clicked();

    void on_pushButton_pressed();

private:
    Ui::MainWindow * ui;
};


#endif // MAINWINDOW_H
