#pragma once
#ifndef CLIENT_H
#define CLIENT_H

// Reza Qorbani
// 26/09/2020
// Client side of an asynchronous chat application.
// Last updated 09/02/2021


#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <array>
#include "chat_message.hpp"
#include "mainwindow.h"


namespace Ui { class MainWindow; }

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client( const std::array<char, MAX_USERNAME_LENGTH> username,
          boost::asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints,
      Ui::MainWindow * ui)
    : io_context_(io_context),
      socket_(io_context),
      ui_(ui),
      username_(username)
  {
    do_connect(endpoints);
  }

  void write(const chat_message& msg)
  {
    boost::asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    boost::asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    boost::asio::async_connect(socket_, endpoints,
        [this](boost::system::error_code ec, tcp::endpoint)
        {
          if (!ec)
          {
              send_username();
            //do_read_header();
          }
        });
  }

  void send_username()
  {
      boost::asio::async_write(socket_,
          boost::asio::buffer(username_,
            username_.size()),
          [this](boost::system::error_code ec, std::size_t /*length*/)
          {
            if (!ec)
            {

               do_read_header();

            }
            else
            {
              socket_.close();
            }
          });
  }

  void do_read_header()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body();

  void do_write()
  {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
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
  inline std::array<char, MAX_USERNAME_LENGTH> get_username() {return username_;}


private:
  boost::asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  Ui::MainWindow * ui_;
  const std::array<char, MAX_USERNAME_LENGTH> username_;

};

//-------------------------------------------------------------------------------------------------------------

#endif
