#include "crow_all.h"

#include "mmt.hpp"

int
main()
{
  crow::SimpleApp app;

  // Fetches the home screen.
  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)(
      []() { return crow::mustache::load("index.mustache").render(); });

  // Handler for JSON requests
  CROW_ROUTE(app, "/api/v1/traspilation")
      .methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        if (auto body{ crow::json::load(req.body) }; body && body.has("data"))
          return crow::response(mmt::transpile(body["data"].s()));
        return crow::response(400);
      });

  // Handler for form requests
  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::POST)([](crow::request req) -> crow::response {
    auto body{ req.get_body_params() };
    CROW_LOG_INFO << "Received payload:" << body;

    auto data{ body.get("data") };
    if (!data)
      return crow::response(400);

    auto page{ crow::mustache::load("index.mustache") };

    if (auto result{ mmt::transpile(data) }; result != "")
      return page.render({ { "did-transpile", true }, { "transpilation-result", result } });

    return page.render({ { "error", "Transpilation failed, please enter a valid expression" } });
  });

  app.port(8080).multithreaded().run();

  return 0;
}
