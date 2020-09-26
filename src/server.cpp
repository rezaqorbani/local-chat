// Reza Qorbani
// 26/09/2020
// Server side of an asynchronous chat application.

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <cstdlib>

using boost::asio::ip::tcp;

//Used for polymorphism purposes in order to resolve issues with forward declaration and incomplete types
class participants
{
public:
    virtual ~participants(){}
    virtual void write_messages(std::string)=0;
};

typedef std::shared_ptr<participants> participants_ptr;

//---------------------------------------------------------------------

//chat room for grouping participants, adding new participants and removing leaving participants in a chat session.
//Handles broadcasting messages to all participants by calling chat_connection.
class chat_room
{
public:
  void join(participants_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->write_messages(msg);
  }

  void leave(participants_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const std::string& msg)
  {
    recent_msgs_.push_back(msg);

    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->write_messages(msg);
  }

private:
  std::set<participants_ptr> participants_;
  enum { max_recent_msgs = 100 };
  std::deque<std::string> recent_msgs_;
};


//-----------------------------------------------------------------------------------------
//Handles new connections, reading incoming messages and delivers outgoing messages
class chat_connection
      : public std::enable_shared_from_this<chat_connection>,
        public participants
{
public:
    typedef boost::shared_ptr<chat_connection> pointer;

    chat_connection(boost::asio::io_context& context)
    :socket_(context)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {

        room_.join(shared_from_this());
        start_read();
    }

    void write_messages(std::string message)
    {
        bool write_in_progress = !send_messages_.empty();
        send_messages_.push_back(message);
        if(!write_in_progress)
        {
            start_write();
        }
    }

private:
    void start_read()
    {
        auto self = shared_from_this(); //will be used in lambda expression to keep this shared object alive and avoid its destruction until connection is closed
        boost::asio::async_read_until(socket_,
                                      boost::asio::dynamic_buffer(message_received_,max_message_size_),
                                      '\n',
                                      [this,self](const boost::system::error_code& error_code_,
                                                         size_t bytes_transferred)
                                      {


                                          self->handle_read_header(error_code_,bytes_transferred);
                                      });
    }

    void handle_read_header(const boost::system::error_code& error_code_, size_t bytes_transferred )
    {
          if(error_code_) {return;}

          std::string message_sent_ = message_received_.substr(0,bytes_transferred);
          message_received_.erase(0,bytes_transferred);
          room_.deliver(message_sent_);
          start_read();
    }

    void start_write()
    {

          auto self = shared_from_this();
          boost::asio::async_write(socket_,
                                   boost::asio::buffer(send_messages_.front()),
                                   [this,self](const boost::system::error_code & error_code_,
                                               size_t /*bytes_transferred*/)
           {
               if(error_code_) {return;}
               send_messages_.pop_front();
               if(!send_messages_.empty())
               {
                    start_write();
               }

           });
    }

    tcp::socket socket_;
    std::string message_received_;
    std::deque<std::string> send_messages_;
    chat_room room_;
    int max_message_size_ = 1024;
};

//----------------------------------------------------------------------------
//Chat server used for establishing new connections and initianting asynchronous I/O
class chat_server
{
public:
    using shared_handler_t = std::shared_ptr<chat_connection>;
    chat_server(boost::asio::io_context& io_context, int port)
               : io_context_(io_context),
                 acceptor_{io_context, tcp::endpoint(tcp::v4(), port)}
    {
        start_server();
    }

private:
    void start_server()
    {

    auto handler = std::make_shared<chat_connection>(io_context_);
    acceptor_.async_accept(handler->socket()
                           ,[=](auto ec)
                           {
                                handle_new_connection(handler,ec);
                           });
    }

    void handle_new_connection(shared_handler_t handler, const boost::system::error_code & error_code_)
    {

        if(error_code_){return;}
        handler->start();
        auto new_handler = std::make_shared<chat_connection>(io_context_);

        acceptor_.async_accept(new_handler->socket()
                             , [=](auto ec)
                             {
                             handle_new_connection(new_handler,
                                                   error_code_);
                             }
                             );
    }

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;

};

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port>\n";
            return 1;
        }
        boost::asio::io_context io_context;

        int listning_port = std::atoi(argv[1]);
        chat_server server(io_context, listning_port);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

//-----------------------------------------------------------------
