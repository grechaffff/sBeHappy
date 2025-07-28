#include "./tcp_server.h"

tcp_server::tcp_server(asio::io_context& io_context, unsigned short port, std::function<invoker_t> invoker)
    : context_(ssl::context::tlsv13)
    , acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    , invoker(std::move(invoker)) {
        
    // Настройка SSL контекста
    context_.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::single_dh_use);
        
    // Установка сертификата и приватного ключа
    context_.use_certificate_chain_file("server.crt");
    context_.use_private_key_file("server.key", ssl::context::pem);
        
    // Начать принимать соединения
    do_accept();
}

void tcp_server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                // Создаем SSL сокет и выполняем handshake
                auto ssl_socket = std::make_shared<ssl::stream<tcp::socket>>(
                    std::move(socket), context_);
                
                ssl_socket->async_handshake(
                    ssl::stream_base::server,
                    [this, ssl_socket](boost::system::error_code ec) {
                        if (!ec) {
                            // После успешного handshake обрабатываем соединение
                            handle_connection(ssl_socket);
                        } else {
                            std::cerr << "Handshake failed: " << ec.message() << "\n";
                        }
                    });
            }
                
            // Принимаем следующее соединение
            do_accept();
        }
    );
}

void tcp_server::handle_connection(std::shared_ptr<ssl::stream<tcp::socket>> ssl_socket) {
    auto buffer = std::make_shared<beast::flat_buffer>();
    auto request = std::make_shared<beast::http::request<beast::http::string_body>>();
        
    // Читаем HTTP запрос
    beast::http::async_read(
        *ssl_socket,
        *buffer,
        *request,
        [this, ssl_socket, buffer, request](boost::system::error_code ec, size_t) {
            if (!ec) {
                auto response = invoker(request);
                if (!response)
                    return;

                // Отправляем ответ
                beast::http::async_write(
                    *ssl_socket,
                    *response,
                    [ssl_socket, response](boost::system::error_code ec, size_t) {
                        // Закрываем соединение после отправки ответа
                        if (ec) {
                            std::cerr << "Write failed: " << ec.message() << "\n";
                        }
                    });
            }
        });
    }