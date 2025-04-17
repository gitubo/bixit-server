#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>

#include "http_router.hpp"
#include "handlers.hpp"
#include "datacarder.hpp"

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
using tcp = boost::asio::ip::tcp;       

// Report a failure
void
fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}


// Handles an HTTP server connection
void
do_session(
    beast::tcp_stream& stream,
//    std::shared_ptr<std::string const> const& doc_root,
    datacarder::SchemaCatalog* catalog,
    net::yield_context yield)
{

    beast::error_code ec;
    beast::flat_buffer buffer;

    for(;;)
    {
        stream.expires_after(std::chrono::seconds(30));

        // Read a request
        http::request<http::string_body> req;
        http::async_read(stream, buffer, req, yield[ec]);
        if(ec == http::error::end_of_stream)
            break;
        if(ec)
            return fail(ec, "read");

        // Handle the request
        HttpRouter router;
        router.add_route(http::verb::get, boost::beast::string_view("/"), handle_get_root);
        router.add_route(http::verb::post, boost::beast::string_view("/bitstream_to_json"), handle_post_bitstream_to_json);
        router.add_route(http::verb::post, boost::beast::string_view("/json_to_bitstream"), handle_post_json_to_bitstream);
        router.add_route(http::verb::post, boost::beast::string_view("/bitstream_to_json_with_schema"), handle_post_bitstream_to_json_with_schema);
        router.add_route(http::verb::post, boost::beast::string_view("/json_to_bitstream_with_schema"), handle_post_json_to_bitstream_with_schema);
        http::message_generator msg = router.handle_request(req, catalog);
        
        bool keep_alive = msg.keep_alive();
        beast::async_write(stream, std::move(msg), yield[ec]);

        if(ec)
            return fail(ec, "write");

        if(! keep_alive)
        {
            break;
        }
    }

    stream.socket().shutdown(tcp::socket::shutdown_send, ec);
}

void
do_listen(
    net::io_context& ioc,
    tcp::endpoint endpoint,
    datacarder::SchemaCatalog* catalog,
    net::yield_context yield)
{
    beast::error_code ec;

    // Open the acceptor
    tcp::acceptor acceptor(ioc);
    acceptor.open(endpoint.protocol(), ec);
    if(ec)
        return fail(ec, "open");

    // Allow address reuse
    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if(ec)
        return fail(ec, "set_option");

    // Bind to the server address
    acceptor.bind(endpoint, ec);
    if(ec)
        return fail(ec, "bind");

    // Start listening for connections
    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if(ec)
        return fail(ec, "listen");

    for(;;)
    {
        tcp::socket socket(ioc);
        acceptor.async_accept(socket, yield[ec]);
        if(ec)
            fail(ec, "accept");
        else
            boost::asio::spawn(
                acceptor.get_executor(),
                std::bind(
                    &do_session,
                    beast::tcp_stream(std::move(socket)),
//                    doc_root,
                    catalog,
                    std::placeholders::_1),
                    // we ignore the result of the session,
                    // most errors are handled with error_code
                    boost::asio::detached);
    }

}


int main(int argc, char* argv[])
{

    // Check command line arguments.
    if (argc != 5)
    {
        std::cerr <<
            "Usage: server_http <address> <port> <doc_root> <threads>\n" <<
            "Example:\n" <<
            "    server_http 0.0.0.0 8080 . 1\n";
        return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const doc_root = std::make_shared<std::string>(argv[3]);
    auto const threads = std::max<int>(1, std::atoi(argv[4]));

    datacarder::SchemaCatalog catalog(*doc_root, datacarder::Logger::Level::DEBUG);

    net::io_context ioc{threads};

    // Spawn a listening port
    boost::asio::spawn(ioc,
        std::bind(
            &do_listen,
            std::ref(ioc),
            tcp::endpoint{address, port},
//            doc_root,
            &catalog,
            std::placeholders::_1),
        [](std::exception_ptr ex)
        {
            if (ex)
                std::rethrow_exception(ex);
        });

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
        [&ioc]
        {
            ioc.run();
        });
    ioc.run();

    return EXIT_SUCCESS;
}