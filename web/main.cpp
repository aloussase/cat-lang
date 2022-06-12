#include "crow_all.h"

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

    // FIXME: auto data{ body.get("data") };
    // Waiting for this fix to be merged into master.
    auto data{ body.get("ata") };
    if (!data)
      return crow::response(400);

    auto result{ mmt::transpile(data) };
    if (result == "")
      return crow::response(400);

    crow::mustache::context ctx;
    ctx["did-transpile"] = true;
    ctx["transpilation-result"] = result;

    auto page{ crow::mustache::load("index.mustache") };
    return crow::response(page.render(ctx));
  });

  app.port(8080).multithreaded().run();

  return 0;
}
