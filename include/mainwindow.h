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

    bool pushButton_2_clicked();

    void pushButton_pressed();

private:

    Ui::MainWindow * ui;
    boost::asio::io_context io_context;
    tcp::resolver resolver;
    boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> endpoints;
    chat_client * client;
    std::thread execution_thread;

};


#endif // MAINWINDOW_H
