// Reza Qorbani
// 26/09/2020
// Client side of an asynchronous chat application.
// Last updated 09/02/2021

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
//Handles the initial connection and asynchronous read and write to the server
class chat_client
{
public:
  typedef std::deque<std::string> chat_message_queue;
  chat_client(boost::asio::io_context &io_context,
              const tcp::resolver::results_type &endpoints)
      : io_context_(io_context),
        socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(std::string msg)
  {

    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

  void close()
  {
    socket_.close();
  }

private:
  void do_connect(const tcp::resolver::results_type &endpoints)
  {
    boost::asio::async_connect(socket_, endpoints,
                               [this](boost::system::error_code ec, tcp::endpoint)
                               {
                                 if (!ec)
                                 {
                                   do_read();
                                 }
                               });
  }

  void do_read()
  {
    boost::asio::async_read_until(socket_,
                                  boost::asio::dynamic_buffer(read_message_, max_message_size),
                                  '\n',
                                  [this](boost::system::error_code ec, std::size_t bytes)
                                  {
                                    if (!ec)
                                    {
                                      std::string server_message = read_message_.substr(0, bytes);
                                      std::cout << "Server: " << server_message;

                                      read_message_.erase(0, bytes);

                                      do_read();
                                    }

                                    else
                                    {
                                      socket_.close();
                                    }
                                  });
  }

  void do_write()
  {
    std::string message_sent_ = write_msgs_.front() + "\n";
    boost::asio::async_write(socket_,
                             boost::asio::buffer(message_sent_),
                             [this](boost::system::error_code ec, std::size_t /*length*/)
                             {
                               if (!ec)
                               {
                                 write_msgs_.pop_front();
                                 if (!write_msgs_.empty())
                                 {
                                   do_write();
                                 }
                               }
                               else
                               {
                                 socket_.close();
                               }
                             });
  }

  boost::asio::io_context &io_context_;
  tcp::socket socket_;
  chat_message_queue write_msgs_;
  std::string read_message_;
  boost::asio::streambuf buffer;
  int max_message_size = 1024;
};
//-------------------------------------------------------------------------------------------------------------
// Multi threaded client for asynchronous read and write operation with the server.
int main(int argc, char *argv[])
{
  try
  {
    if (argc < 3)
    {
      std::cerr << "Usage: chat_server <host> <port> \n";
      return 1;
    }
    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    chat_client c(io_context, endpoints);

    std::thread t([&io_context]()
                  { io_context.run(); });

    std::string line;
    while (true)

    {
      if (!std::getline(std::cin, line))
        break;
      c.write(line);
    }

    c.close();
    t.join();
  }

  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
