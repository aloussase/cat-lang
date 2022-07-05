#include <cstdlib>

#include "crow_all.h"

#include "cat.hpp"

#define CROW_DISABLE_STATIC_DIR

int
main()
{
  // crow::SimpleApp app;
  crow::App<crow::CORSHandler> app;

  auto cors{ app.get_middleware<crow::CORSHandler>() };
  cors.global().methods(crow::HTTPMethod::POST).origin("*");

  CROW_ROUTE(app, "/api/v1/transpilation").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
    int status_code{ 200 };
    crow::json::wvalue payload{};

    if (auto body{ crow::json::load(req.body) }; body && body.has("data"))
      {
        std::string output{};
        cat::transpile(body["data"].s(), output);
        payload["data"] = output;
      }
    else
      {
        payload["error"] = "Invalid payload, expected expression in 'data' key";
        status_code = 400;
      }

    return crow::response{ status_code, payload };
  });

  std::string port{ "8080" };

  if (auto e_port{ std::getenv("PORT") }; e_port)
    port = e_port;

  app.port(std::stoi(port)).multithreaded().run();

  return 0;
}
