# webservice-cpp

Simples webservice RESTful em C++ com Boost.Asio e SQLite — feito para estudos e experimentos com baixo nível e alta performance.

---

## Dependências

- C++17
- [Boost](https://www.boost.org/) (`boost-system`, `boost-asio`)
- [SQLite3](https://sqlite.org)

No macOS:

```bash
brew install boost sqlite3
```

No Ubuntu:
```bash
sudo apt install libboost-all-dev libsqlite3-dev
```

## Compilar

```bash
g++ main.cpp database.cpp -std=c++17 -lsqlite3 -lboost_system -o server
```

## Correr

```bash
./server
```

## Rotas

```bash
curl -X POST http://localhost:8080/items -d "Notebook"
curl http://localhost:8080/items
curl http://localhost:8080/items/1
curl -X PUT http://localhost:8080/items/1 -d "Caderno"
curl -X DELETE http://localhost:8080/items/1
```