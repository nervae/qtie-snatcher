#pragma once

#include <cstdint>
#include <chrono>
#include <string_view>
#include <utility>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>

namespace snatch {

template <typename Stream>
class HttpSyncClientBase {
    public:
        template <typename ...Args>
        HttpSyncClientBase(std::chrono::milliseconds timeout = std::chrono::seconds(30), Args &&...args):
                resolver(ioctx), stream(ioctx, std::forward<Args>(args)...) {
            this->set_timeout(timeout);
        }

        virtual ~HttpSyncClientBase() {
            if (boost::beast::get_lowest_layer(this->stream).socket().is_open())
                this->shutdown();
        }

        void set_timeout(std::chrono::milliseconds timeout) {
            boost::beast::get_lowest_layer(this->stream).expires_after(timeout);
        }

        auto resolve(std::string_view host, std::string_view service) {
            this->host = host;
            boost::system::error_code ec;
            return std::make_pair(std::move(ec), resolver.resolve(host, service, ec));
        }

        virtual boost::system::error_code connect(boost::asio::ip::tcp::resolver::results_type res) {
            boost::system::error_code ec;
            boost::beast::get_lowest_layer(this->stream).connect(res, ec);
            return ec;
        }

        boost::system::error_code connect(std::string_view host, std::string_view service = "http") {
            auto &&[ec, res] = this->resolve(host, service);
            return ec ?: this->connect(res);
        }

        boost::system::error_code connect(std::string_view host, int port) {
            auto &&[ec, res] = this->resolve(host, std::to_string(port));
            return ec ?: this->connect(res);
        }

        virtual boost::system::error_code shutdown() {
            boost::system::error_code ec;
            boost::beast::get_lowest_layer(this->stream).socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            return ec;
        }

        template <typename ReqBody = boost::beast::http::string_body>
        auto make_request(boost::beast::http::verb method, std::string_view host, std::string_view target, unsigned version = 11) {
            boost::beast::http::request<ReqBody> request;
            request.version(version);
            request.method(method);
            request.set(boost::beast::http::field::host, boost::string_view(host.data(), host.size()));
            request.target(boost::string_view(target.data(), target.size()));
            request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            return request;
        }

        template <typename R>
        auto write_request(const R &request) {
            boost::system::error_code ec;
            boost::beast::http::write(this->stream, request, ec);
            return ec;
        }

        template <typename RespBody = boost::beast::http::string_body>
        auto read_response(boost::beast::http::response<RespBody> &&response) {
            boost::beast::flat_buffer buffer;

            boost::system::error_code ec;
            boost::beast::http::read(this->stream, buffer, response, ec);

            return std::make_pair(std::move(ec), std::move(response));
        }

        template <typename RespBody = boost::beast::http::empty_body>
        auto read_response(boost::beast::http::response_parser<RespBody> &&parser) {
            boost::beast::flat_buffer buffer;
            parser.skip(true);

            boost::system::error_code ec;
            boost::beast::http::read(this->stream, buffer, parser, ec);

            return std::make_pair(std::move(ec), std::move(parser.get()));
        }

        template <typename RespBody = boost::beast::http::string_body>
        auto get(std::string_view host, std::string_view target, unsigned version = 11) {
            auto ec = this->write_request(this->make_request(boost::beast::http::verb::get, host, target, version));
            if (!ec)
                return this->read_response(boost::beast::http::response<RespBody>());
            return std::make_pair(std::move(ec), boost::beast::http::response<RespBody>());
        }

        auto get(std::string_view target, unsigned version = 11) {
            return this->get(this->host, target, version);
        }

        template <typename RespBody = boost::beast::http::empty_body>
        auto head(std::string_view host, std::string_view target, unsigned version = 11) {
            auto ec = this->write_request(this->make_request(boost::beast::http::verb::head, host, target, version));
            if (!ec)
                return this->read_response(boost::beast::http::response_parser<RespBody>());
            return std::make_pair(std::move(ec), boost::beast::http::response<RespBody>());
        }

        auto head(std::string_view target, unsigned version = 11) {
            return this->head(this->host, target, version);
        }

        template <typename RespBody = boost::beast::http::string_body>
        auto post(std::string_view body, std::string_view host, std::string_view target, unsigned version = 11) {
            auto request = this->make_request<boost::beast::http::string_body>(boost::beast::http::verb::post, host, target, version);
            request.set(boost::beast::http::field::content_type, "text/plain");
            request.body() = body;
            request.prepare_payload();

            auto ec = this->write_request(request);
            if (!ec)
                return this->read_response(boost::beast::http::response_parser<RespBody>());
            return std::make_pair(std::move(ec), boost::beast::http::response<RespBody>());
        }

        template <typename RespBody = boost::beast::http::string_body>
        auto post(std::string_view body, std::string_view target, unsigned version = 11) {
            return this->post(body, this->host, target, version);
        }

    public:
        std::string host;

    protected:
        static inline boost::asio::io_context ioctx;

        boost::asio::ip::tcp::resolver resolver;
        Stream                         stream;
};

class HttpSyncClient final: public HttpSyncClientBase<boost::beast::tcp_stream>
{ };

class HttpsSyncClient final: public HttpSyncClientBase<boost::beast::ssl_stream<boost::beast::tcp_stream>> {
    public:
        HttpsSyncClient(std::chrono::milliseconds timeout = std::chrono::seconds(30)): HttpSyncClientBase(timeout, sslctx) {
            this->sslctx.set_verify_mode(boost::asio::ssl::verify_peer);
        }

        virtual boost::system::error_code connect(boost::asio::ip::tcp::resolver::results_type res) override {
            boost::system::error_code ec;
            boost::beast::get_lowest_layer(this->stream).connect(res, ec);
            if (!ec)
                this->stream.handshake(boost::asio::ssl::stream_base::client, ec);
            return ec;
        }

        boost::system::error_code connect(std::string_view host) {
            return HttpSyncClientBase::connect(host, "https");
        }

        using HttpSyncClientBase::connect;

        virtual boost::system::error_code shutdown() override {
            boost::system::error_code ec;
            this->stream.shutdown(ec);
            return ec;
        }

    protected:
        static inline boost::asio::ssl::context sslctx{boost::asio::ssl::context::tlsv12_client};
};

} // namespace snatch
