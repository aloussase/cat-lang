#include <crow.h>

#include "mmt.hpp"

int
main()
{
  crow::SimpleApp app;

  // Fetches the home screen.
  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)([]() {
    auto page{ crow::mustache::load("index.mustache") };
    return page.render();
  });

  // Handler for JSON requests
  CROW_ROUTE(app, "/api/v1/traspilation")
      .methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        if (auto body{ crow::json::load(req.body) }; body && body.has("data"))
          return crow::response(mmt::transpile(body["data"].s()));

        return crow::response(400);
      });

  // Handler for form requests
  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::POST)([](crow::request req) {
    auto body{ req.get_body_params() };
    CROW_LOG_INFO << "Received payload:" << body;

    if (auto data{ body.get("data") }; data)
      return crow::response(mmt::transpile(data));

    return crow::response(400);
  });

  app.port(8080).multithreaded().run();

  return 0;
}
