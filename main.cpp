#include <iostream>
#include <boost/asio.hpp>
#include <sstream>
#include <array>
#include "database.hpp"

using namespace boost::asio;
using ip::tcp;

std::string route(const std::string &req) {
    std::istringstream ss(req);
    std::string method, path, version;
    ss >> method >> path >> version;

    // HTML Swagger-like
    if (path == "/") {
        std::string html = R"(<!DOCTYPE html>
<html lang="pt">
  <head>
    <meta charset="UTF-8">
    <title>Webservice C++ + SQLite</title>
    <style>
      body { font-family: 'Segoe UI', sans-serif; padding: 2rem; background: #f5f5f5; }
      code, pre { background: #eee; padding: 0.2rem 0.4rem; border-radius: 4px; }
      .route { margin-bottom: 0.8rem; }
      h1 { color: #2c3e50; }
    </style>
  </head>
  <body>
    <h1>🚀 Webservice C++ + SQLite</h1>
    <p>Rotas disponíveis:</p>
    <div class="route"><code>GET /items</code> – lista todos os itens</div>
    <div class="route"><code>GET /items/{id}</code> – retorna um item específico</div>
    <div class="route"><code>POST /items</code> – cria um novo item (body = nome)</div>
    <div class="route"><code>PUT /items/{id}</code> – atualiza um item existente</div>
    <div class="route"><code>DELETE /items/{id}</code> – remove um item</div>
    <hr>
    <p>🧪 Use <code>curl</code> ou Postman para testar a API.</p>
    <pre>
curl -X POST http://localhost:8080/items -d "Notebook"
curl http://localhost:8080/items
    </pre>
  </body>
</html>)";

        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html; charset=UTF-8\r\n"
               "Content-Length: " + std::to_string(html.size()) + "\r\n\r\n" + html;
    }

    // GET /items
    if (method == "GET" && path == "/items") {
        auto items = getAllItems();
        std::ostringstream body;
        for (auto &item : items)
            body << item.id << ": " << item.name << "\n";
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + body.str();
    }

    // POST /items
    if (method == "POST" && path == "/items") {
        size_t pos = req.find("\r\n\r\n");
        std::string body = (pos != std::string::npos) ? req.substr(pos + 4) : "";
        body.erase(body.find_last_not_of(" \r\n") + 1);
        int id = createItem(body);
        return "HTTP/1.1 201 Created\r\n\r\nCreated item with ID " + std::to_string(id) + "\n";
    }

    // GET /items/{id}
    if (method == "GET" && path.find("/items/") == 0) {
        int id = std::stoi(path.substr(7));
        auto item = getItemById(id);
        if (item.id == -1)
            return "HTTP/1.1 404 Not Found\r\n\r\n";
        return "HTTP/1.1 200 OK\r\n\r\n" + item.name + "\n";
    }

    // PUT /items/{id}
    if (method == "PUT" && path.find("/items/") == 0) {
        int id = std::stoi(path.substr(7));
        size_t pos = req.find("\r\n\r\n");
        std::string body = (pos != std::string::npos) ? req.substr(pos + 4) : "";
        body.erase(body.find_last_not_of(" \r\n") + 1);
        if (updateItem(id, body))
            return "HTTP/1.1 200 OK\r\n\r\nUpdated\n";
        return "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    // DELETE /items/{id}
    if (method == "DELETE" && path.find("/items/") == 0) {
        int id = std::stoi(path.substr(7));
        if (deleteItem(id))
            return "HTTP/1.1 200 OK\r\n\r\nDeleted\n";
        return "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    return "HTTP/1.1 404 Not Found\r\n\r\n";
}

void server() {
    boost::asio::io_context ctx;
    boost::asio::ip::tcp::acceptor acceptor(ctx, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));

    std::cout << "Servidor iniciado em http://localhost:8080\n";

    while (true) {
        try {
            boost::asio::ip::tcp::socket socket(ctx);
            acceptor.accept(socket);

            std::array<char, 4096> buffer;
            boost::system::error_code ec;

            size_t bytes = socket.read_some(boost::asio::buffer(buffer), ec);

            if (ec == boost::asio::error::eof) {
                std::cerr << "[INFO] Cliente fechou a conexão.\n";
                continue;
            } else if (ec) {
                std::cerr << "[ERRO] Erro na leitura: " << ec.message() << "\n";
                continue;
            }

            std::string req(buffer.data(), bytes);
            std::string res = route(req);

            boost::asio::write(socket, boost::asio::buffer(res));
        } catch (const std::exception &ex) {
            std::cerr << "[FATAL] Exceção no ciclo do servidor: " << ex.what() << "\n";
        }
    }
}

int main() {
    std::cout << "Starting webservice at http://localhost:8080\n";
    if (!initDatabase()) {
        std::cerr << "Failed to init DB\n";
        return 1;
    }
    server();
}