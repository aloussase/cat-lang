#include <cstdlib>

#include "crow_all.h"

#include "cat.hpp"

#define CROW_DISABLE_STATIC_DIR

int
main()
{
  crow::SimpleApp app;

  CROW_ROUTE(app, "/api/v1/transpilation").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
    if (auto body{ crow::json::load(req.body) }; body && body.has("data"))
      return crow::response(cat::transpile(body["data"].s()));
    return crow::response(400);
  });

  std::string port{ "8080" };

  if (auto e_port{ std::getenv("PORT") }; e_port)
    port = e_port;

  app.port(std::stoi(port)).multithreaded().run();

  return 0;
}
