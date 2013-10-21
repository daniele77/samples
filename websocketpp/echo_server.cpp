/* Copyright Daniele Pallastrelli 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */
 
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>
#include <set>
#include <memory>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::connection_hdl;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

class MyServer
{
    public:

        // Define a callback to handle incoming messages
        void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
            std::cout << "on_message called with hdl: " << hdl.lock().get()
                      << " and message: " << msg->get_payload()
                      << std::endl;

            try {
                s->send(hdl, msg->get_payload(), msg->get_opcode());
            } catch (const websocketpp::lib::error_code& e) {
                std::cout << "Echo failed because: " << e
                          << "(" << e.message() << ")" << std::endl;
            }
        }

        void on_timer(const websocketpp::lib::error_code & ec) {
            std::cout << "Timer expired: sending message" << std::endl;
            std::stringstream ss;
            ss << count++;
            for (con_list::iterator i = connections.begin(); i != connections.end(); ++i )
                echo_server.send( *i, ss.str(), websocketpp::frame::opcode::text );
            timer = echo_server.set_timer(1000, bind(&MyServer::on_timer,this,_1));
        }
        
        void on_open(connection_hdl hdl)
        {
            std::cout << "someone connected" << std::endl;
            connections.insert(hdl);
        }
    
        void on_close(connection_hdl hdl)
        {
            std::cout << "someone disconnected" << std::endl;
            connections.erase(hdl);
        }
        
        MyServer() : count( 0 )
        {
            try
            {
                // Set logging settings
                echo_server.set_access_channels(websocketpp::log::alevel::all);
                echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

                // Initialize ASIO
                echo_server.init_asio();
                
                timer = echo_server.set_timer(1000, bind(&MyServer::on_timer,this,_1));

                // Register our handlers
                echo_server.set_open_handler(bind(&MyServer::on_open,this,_1));
                echo_server.set_close_handler(bind(&MyServer::on_close,this,_1));
                echo_server.set_message_handler(bind(&MyServer::on_message,this,&echo_server,::_1,::_2));

                // Listen on port 9002
                echo_server.listen(boost::asio::ip::tcp::v4(),9002);

                // Start the server accept loop
                echo_server.start_accept();
            } catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            } catch (websocketpp::lib::error_code e) {
                std::cout << e.message() << std::endl;
            } catch (...) {
                std::cout << "other exception" << std::endl;
            }
        }
        
        void Run()
        {
            // Start the ASIO io_service run loop
            echo_server.run();
        }

    private:
    
        server echo_server;
        server::timer_ptr timer;
        size_t count;
        typedef std::set<connection_hdl/*,std::owner_less<connection_hdl>*/ > con_list;
        con_list connections;
};

int main() {
    MyServer s;
    s.Run();
}
