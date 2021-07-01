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

void MainWindow::on_pushButton_pressed()
{
    ui->textBrowser->append(ui->lineEdit->text());

}


void MainWindow::on_actionConnect_to_Localhost_triggered()
{
   // QString address = QInputDialog::getText(this, "Connect to Server", "Enter IP Address for Host");
   // QString port = QInputDialog::getText(this, "Connect to Server", "Enter the Port");

    try
     {


       boost::asio::io_context io_context;

       tcp::resolver resolver(io_context);
       auto endpoints = resolver.resolve("127.0.0.1", "8888");
       chat_client c(io_context, endpoints, this->ui);

       std::thread t([&io_context](){ io_context.run(); });

         const char * hello = "Hello!";
         chat_message msg;
         size_t length = std::strlen(hello);
         msg.body_length(length);
         std::memcpy(msg.body(), hello, msg.body_length());
         msg.encode_header();
         c.write(msg);



       c.close();
       t.join();
     }
     catch (std::exception& e)
     {
       std::cerr << "Exception: " << e.what() << "\n";
     }


}


void chat_client::do_read_body()
{
  boost::asio::async_read(socket_,
      boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
      [this](boost::system::error_code ec, std::size_t /*length*/)
      {
        if (!ec)
        {
          ui_->textBrowser->append(read_msg_.body());

          do_read_header();
        }
        else
        {
          socket_.close();
        }
      });
}


bool MainWindow::on_pushButton_2_clicked()
{
    return true;
}









