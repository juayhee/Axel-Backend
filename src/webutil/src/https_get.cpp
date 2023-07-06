#include "webutil/https_get.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <openssl/ssl.h>
#include <string>

namespace webutil {
    // Maximum size, in bytes, of the HTTP body the parser is willing to accept
    // TODO: Move this constant somewhere nicer so it doesn't have to affect all HTTP gets.
    constexpr int MAX_HTTP_BODY_SIZE = 15 * 1024 * 1024; // 15MB

    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    namespace ssl = net::ssl;
    using tcp = net::ip::tcp;

/**
* @internal
* Function to run HTTP Get to the specified host, service and path
* This function is used to run a HTTPS GET request using Boost Beast.
*
*
* @param host - The hostname or IP address of the server
* @param service - The service name or port number
* @param path - The path on the server
*
* @return The body of the HTTP response from the server as a string
*/
    std::string
    http_get(const std::string& host, const std::string& service, const std::string& path) {


        // IO and SSL context for stream
        net::io_context ioc;
        ssl::context ctx(ssl::context::tlsv12_client);
        ssl::stream<tcp::socket> stream(ioc, ctx); // Connection stream

        // Resolve hostname
        tcp::resolver resolver(ioc);
        const auto endpoints = resolver.resolve(host, service);

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            boost::system::error_code ec{static_cast<int>(ERR_get_error()), boost::asio::error::get_ssl_category()};
            throw boost::system::system_error{ec};
        }

        // Handshake
        net::connect(stream.next_layer(), endpoints);

        stream.handshake(ssl::stream_base::client);

        // Construct HTTP request
        http::request<http::string_body> request(http::verb::get, path, 11);
        request.set(http::field::host, host);

        http::write(stream, request);

        // Receive and print the HTTPS response
        beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> parser;
        parser.body_limit(MAX_HTTP_BODY_SIZE); // Bytes to megabytes
        read(stream, buffer, parser);
        http::response<http::dynamic_body> response = parser.release();

        // Gracefully close the stream
        beast::error_code ec;
        stream.shutdown(ec);
        stream.lowest_layer().close(ec);

        // Convert response to string for parsing
        std::string body_string = beast::buffers_to_string(response.body().data());

        return body_string;
    }
} // webutil