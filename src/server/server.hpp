#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
    void join(chat_participant_ptr, std::string username);

    void leave(chat_participant_ptr);

    void deliver( chat_message& msg, chat_participant_ptr participant);

    std::string get_username(std::shared_ptr<chat_participant> participant);


private:
    std::set<chat_participant_ptr> participants_;
    std::map<chat_participant_ptr, std::string> username_lookup_table_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;

};

//----------------------------------------------------------------------

class chat_session
    : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
    chat_session(tcp::socket socket, chat_room& room)
        : socket_(std::move(socket)),
        room_(room)
    {
    }

    void start();

    void deliver(const chat_message& msg);


private:
    void do_read_username();

    void do_read_header();

    void do_read_body();

    void do_write();

    tcp::socket socket_;
    chat_room& room_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
    std::array<char, MAX_USERNAME_LENGTH> username_;
};


#endif
