/**
 * Author : Netanel Shoshan
 * Description :
 * This is the main file for the client program.
 * I implemented binary protocol over TCP.
 * I used a bytes vector in order to represent the the bytes that we get from and to the server.
 */
#include <iostream>
#include <boost/asio.hpp>
#include "constants.h"
#include "responses.h"
#include "client.h"
#include <boost/filesystem/fstream.hpp>
using boost::asio::ip::tcp;


int main() {
    std::string s_address, s_port;
    boost::filesystem::ifstream file;
    boost::filesystem::path path(boost::filesystem::current_path());
    const std::string serverInfo = "server.info";
    try {
        if (!boost::filesystem::exists(serverInfo))
            throw std::runtime_error("server.info doesn't exist");
        path.append(serverInfo);
        file.open(path, std::ios::out);
        if (!file)
            throw std::runtime_error("Can't open file" + serverInfo);
        getline(file, s_address, ':');
        getline(file, s_port);
    } catch (std::exception &e) {
        std::cerr << "Exception in main: " << e.what() << std::endl;
        exit(1);
    }
    client client;

    //client.io - returns true until program is terminated / 0 .
    while (client.io()) {
        unsigned char buf[BUFFER_SIZE];
        boost::asio::io_context io_context;
        boost::system::error_code ec;
        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        try {
            // connect to the server and send the request.
            boost::asio::connect(s, resolver.resolve(s_address, s_port));
            boost::asio::write(s, boost::asio::buffer(client.getRequest()));

            // read the header from the server the buffer.
            boost::asio::read(s, boost::asio::buffer(buf, RESPONSE_HEADER_SIZE));

            // parse the header and start initialize the payload.
            responses response(buf);

            // read the payload from the server.
            for (int i = 0; i < response.getPayloadSIze(); i += BUFFER_SIZE) {
                // until weve reached the last chunk, that should smaller that 1024, read 1024.
                int delta = response.getPayloadSIze() - i;
                int size = (BUFFER_SIZE > delta) ? delta : BUFFER_SIZE;
                // read a chunk of calculated remaining size from the socket into chunk.
                boost::asio::read(s, boost::asio::buffer(buf, size), ec);
                response.push(buf, size);
                if (ec == boost::asio::error::eof || ec) {
                    std::cout << std::endl << "Connection closed" << std::endl;
                    break; // Connection closed cleanly by peer.
                }

            }
            client.responseParser(&response);
        }
        catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}