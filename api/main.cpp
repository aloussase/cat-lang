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
    auto body{ crow::json::load(req.body) };

    if (!body.has("data"))
      return crow::response{ 400 };

    auto program{ body["data"].s() };
    std::string output{};

    cat::transpile(program, output);

    return crow::response{ 200, output };
  });

  CROW_ROUTE(app, "/api/v1/execution").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
    auto body{ crow::json::load(req.body) };

    if (!body.has("data"))
      return crow::response{ 400 };

    auto program{ body["data"].s() };

    std::string transpilation_output{};
    bool ok = cat::transpile(program, transpilation_output);

    if (!ok)
      {
        CROW_LOG_INFO << "Transpilation had errors, omitting program execution\n";
        return crow::response{ 200, transpilation_output };
      }

    // FIXME: this returns empty for some reason.
    // Probably the file descriptor is not getting correctly duped due to being
    // running as part of a server.
    return crow::response{ 200, cat::execute(transpilation_output) };
  });

  std::string port{ "8080" };

  if (auto e_port{ std::getenv("PORT") }; e_port)
    port = e_port;

  app.port(std::stoi(port)).multithreaded().run();

  return 0;
}
