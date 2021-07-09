#include "..\include\server\server.hpp"

//----------------------------------------------------------------------

void chat_room::join(chat_participant_ptr participant, std::string username)
{
    participants_.insert(participant);
    username_lookup_table_[participant] = username;
    for (auto msg : recent_msgs_)
        participant->deliver(msg);
}


void chat_room::leave(chat_participant_ptr participant)
{
    participants_.erase(participant);
    username_lookup_table_.erase(participant);
}

void chat_room::deliver( chat_message& msg, chat_participant_ptr participant)
{
    std::string username = get_username(participant);
    msg.body()[msg.body_length()] = '\0';

    chat_message transmitted_message;
    size_t transmitted_message_size = username.size() + msg.body_length() + 2;
    transmitted_message.body_length(transmitted_message_size);
    std::memset(transmitted_message.body(), '\0', transmitted_message_size);

#if defined (_MSC_VER)
    strcpy_s(transmitted_message.body(), username.size() + 1, username.c_str());

    strcat_s(transmitted_message.body(), transmitted_message.body_length(), msg.body());
#else
    strcpy(transmitted_message.body(), username.c_str());

    strcat(transmitted_message.body(), msg.body());
#endif

    transmitted_message.encode_header();

    recent_msgs_.push_back(transmitted_message);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();

    for (auto participant : participants_)
        participant->deliver(transmitted_message);
}

std::string chat_room::get_username(std::shared_ptr<chat_participant> participant)
{
    return username_lookup_table_[participant];
}

//----------------------------------------------------------------------

void chat_session::start()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(username_.data(), MAX_USERNAME_LENGTH),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec && read_msg_.decode_header())
            {
                do_read_username();
            }
            else
            {
                room_.leave(shared_from_this());
            }
        });


}

void chat_session::deliver(const chat_message& msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
        do_write();
    }
}

void chat_session::do_read_username()
{
#if defined (_MSC_VER)
    strcat_s(username_.data(), MAX_USERNAME_LENGTH, ": ");
#else
   strcat(username_.data(), MAX_USERNAME_LENGTH, ": );
#endif
    room_.join(shared_from_this(), std::string(username_.data()));
    do_read_header();
}

void chat_session::do_read_header()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec && read_msg_.decode_header())
            {
                do_read_body();
            }
            else
            {
                room_.leave(shared_from_this());
            }
        });
}

void chat_session::do_read_body()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {

                room_.deliver(read_msg_, shared_from_this());
                do_read_header();
            }
            else
            {
                room_.leave(shared_from_this());
            }
        });
}

void chat_session::do_write()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
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
                room_.leave(shared_from_this());
            }
        });
}


//----------------------------------------------------------------------

class chat_server
{
public:
    chat_server(boost::asio::io_context& io_context,
        const tcp::endpoint& endpoint)
        : acceptor_(io_context, endpoint)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<chat_session>(std::move(socket), room_)->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    chat_room room_;
};


//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_context io_context;

    std::list<chat_server> servers;

    tcp::endpoint endpoint(tcp::v4(), std::atoi("8888"));
    servers.emplace_back(io_context, endpoint);


    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

