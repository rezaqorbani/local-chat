#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPushButton * sendButton;
    QPushButton * leaveButton;

    sendButton = MainWindow::findChild<QPushButton *>("pushButton");
    leaveButton = MainWindow::findChild<QPushButton *>("pushButton_2");

    connect(sendButton, SIGNAL(released()), this, SLOT(on_pushButton_pressed()));
    connect(leaveButton, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void start_client(std::string, std::string, MainWindow *);

void MainWindow::on_pushButton_pressed()
{
    ui->textBrowser->append(ui->lineEdit->text());

}


void MainWindow::on_actionConnect_to_Localhost_triggered()
{
    //QString address = QInputDialog::getText(this, "Connect to Server", "Enter IP Address for Host");
    //QString port = QInputDialog::getText(this, "Connect to Server", "Enter the Port");
//  const char* address_string = address.toStdString().c_str();
//  const char* port_string = port.toStdString().c_str();
    //start_client("127.0.0.1", "8888", this);

}


bool MainWindow::on_pushButton_2_clicked()
{
    return true;
}

void start_client (std::string host,  std::string port, MainWindow* mainwindow)
{
    try
    {

        boost::asio::io_context io_context;


        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);
        chat_client c(io_context, endpoints);

        std::thread t([&io_context]()
            { io_context.run(); });

        QString line;
        bool notTrue = true;
        while (true)

        {
          //if (mainwindow->on_pushButton_2_clicked())
           // break;
          //else if (!mainwindow->on_pushButton_clicked())
            if(notTrue)
                {
              line = mainwindow->ui->lineEdit->text();
              std::string line_std = line.toStdString();
             c.write(line_std);
              notTrue = false;
                }

        }

        c.close();
        t.join();
    }

    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }


}








