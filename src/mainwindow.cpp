#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      resolver(io_context)

{
    ui->setupUi(this);
    QPushButton * sendButton;
    QPushButton * leaveButton;
    QLineEdit * messageLine;

    sendButton = MainWindow::findChild<QPushButton *>("pushButton");
    leaveButton = MainWindow::findChild<QPushButton *>("pushButton_2");
    messageLine = MainWindow::findChild<QLineEdit * > ("lineEdit");

    connect(sendButton, SIGNAL(pressed()), this, SLOT(pushButton_pressed()));
    connect(leaveButton, SIGNAL(clicked()), this, SLOT(pushButton_2_clicked()));
    connect(messageLine, SIGNAL(returnPressed()), this, SLOT(lineEdit_returnPressed()));
}

MainWindow::~MainWindow()
{
    client->close();
    execution_thread.join();
    delete client;
    delete ui;
}

void MainWindow::pushButton_pressed()
{
    if(connected_to_server == true)
    {
        std::string user_text_std = ui->lineEdit->text().toStdString();
        const char * user_text = user_text_std.c_str();
        chat_message msg;
        size_t length = std::strlen(user_text);
        msg.body_length(length);
        std::memcpy(msg.body(), user_text, msg.body_length());
        msg.encode_header();
        client->write(msg);
        ui->lineEdit->clear();
    }
    else
    {
        QMessageBox message;
        message.information(this, "Connection to server failed", "Please connect to the server before sending any message");

    }
}

void MainWindow::on_actionConnect_to_Localhost_triggered()
{

    QString address = QInputDialog::getText(this, "Connect to Server", "Enter IP Address for Host");
    std::string address_string = address.toStdString();
    const char * address_c = address_string.c_str();
    QString port = QInputDialog::getText(this, "Connect to Server", "Enter the Port");
    std::string port_string = port.toStdString();
    const char * port_c = port_string.c_str();

    try
     {

      endpoints = resolver.resolve(address_c, port_c);
      client  = new chat_client(io_context, endpoints, this->ui);
      execution_thread = std::thread([this](){ io_context.run();});
      connected_to_server = true;
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
          char * received_message = new char[read_msg_.body_length()+1];
          std::memcpy(received_message, read_msg_.body(), read_msg_.body_length());
          received_message[read_msg_.body_length()] = '\0';
          ui_->textBrowser->append(received_message);


          do_read_header();
        }
        else
        {
          socket_.close();
        }
      });
}


void MainWindow::pushButton_2_clicked()
{
    //client->close();
    //ui->textBrowser->clear();
}



void MainWindow::lineEdit_returnPressed()
{
    if(connected_to_server == true)
    {
        std::string user_text_std = ui->lineEdit->text().toStdString();
        const char * user_text = user_text_std.c_str();
        chat_message msg;
        size_t length = std::strlen(user_text);
        msg.body_length(length);
        std::memcpy(msg.body(), user_text, msg.body_length());
        msg.encode_header();
        client->write(msg);
        ui->lineEdit->clear();
    }
    else
    {
        QMessageBox message;
        message.information(this, "Connection to server failed", "Please connect to the server before sending any message");

    }

}

