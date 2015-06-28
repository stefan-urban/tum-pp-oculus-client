
#include "TcpClient.hpp"
#include "vendor/oculus-server/TcpMessage.hpp"
#include "vendor/oculus-server/Message_EventCollection.hpp"
#include "vendor/dispatcher/Dispatcher.hpp"

#include <string>

void TcpClient::deliver(TcpMessage& msg)
{
    msg.encode();

  io_service_.post(
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

void TcpClient::close()
{
  io_service_.post([this]() { socket_.close(); });
}

void TcpClient::do_connect(tcp::resolver::iterator endpoint_iterator)
{
  boost::asio::async_connect(socket_, endpoint_iterator,
      [this](boost::system::error_code ec, tcp::resolver::iterator)
      {
        if (!ec)
        {
          do_read_header();
        }
      });
}

void TcpClient::do_read_header()
{
  boost::asio::async_read(socket_,
      boost::asio::buffer(read_msg_.data(), TcpMessage::header_length),
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

void TcpClient::do_read_body()
{
  boost::asio::async_read(socket_,
      boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
      [this](boost::system::error_code ec, std::size_t /*length*/)
      {
        if (!ec)
        {
            std::string const data = std::string(read_msg_.body());

            // Determine type
            size_t pos = data.find('|');

            if (pos == std::string::npos || pos < 2)
            {
                return;
            }

            std::string type = data.substr(0, pos);

            // Pack new event and dispatch it
            auto e = DispatcherEvent(type, data.substr(pos + 1));
            dispatcher_->dispatch(e);

            // And restart with waiting for the next header
            do_read_header();
        }
        else
        {
          socket_.close();
        }
      });
}

void TcpClient::do_write()
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
