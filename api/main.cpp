#include <cstdlib>
#include "crow.h"
#include "crow/middlewares/cors.h"

#include "api_response.hpp"
#include "cat.hpp"

#define CROW_DISABLE_STATIC_DIR

// TODO: add response constructor accepting const payload
static auto INVALID_PAYLOAD_ERROR = error_response{ "Expected program in 'data' field" };

crow::response
transpile_and_execute(const crow::request& req)
{
  auto body{ crow::json::load(req.body) };

  if (!body.has("data"))
    return crow::response{ 400, INVALID_PAYLOAD_ERROR };

  auto program{ body["data"].s() };

  std::string transpilation_output{};

  if (!cat::transpile(program, transpilation_output))
    {
      CROW_LOG_INFO << "Transpilation had errors, omitting program execution\n";
      return crow::response{ 200, success_response{ { "transpilation_result", transpilation_output } } };
    }

  return crow::response{ 200,
                         success_response{ { "transpilation_result", transpilation_output },
                                           { "execution_result", cat::execute(transpilation_output) } } };
}

crow::response
transpile(const crow::request& req)
{
  auto body{ crow::json::load(req.body) };

  if (!body.has("data"))
    return crow::response{ 400, INVALID_PAYLOAD_ERROR };

  auto program{ body["data"].s() };
  std::string output{};
  cat::transpile(program, output);

  return crow::response{ 200, success_response{ { "transpilation_result", output } } };
}

crow::response
execute(const crow::request& req)
{
  auto body{ crow::json::load(req.body) };

  if (!body.has("data"))
    return crow::response{ 400, INVALID_PAYLOAD_ERROR };

  auto program{ body["data"].s() };

  std::string transpilation_output{};
  if (!cat::transpile(program, transpilation_output))
    {
      CROW_LOG_INFO << "Transpilation had errors, omitting program execution\n";
      return crow::response{ 200, success_response{ { "transpilation_result", transpilation_output } } };
    }

  return crow::response{ 200,
                         success_response{ { "execution_result", cat::execute(transpilation_output) } } };
}

int
main()
{
  // crow::SimpleApp app;
  crow::App<crow::CORSHandler> app;

  auto cors{ app.get_middleware<crow::CORSHandler>() };
  cors.global().methods(crow::HTTPMethod::POST).origin("*");

  CROW_ROUTE(app, "/api/v1/transpilation").methods(crow::HTTPMethod::POST)(transpile);
  CROW_ROUTE(app, "/api/v1/execution").methods(crow::HTTPMethod::POST)(execute);
  CROW_ROUTE(app, "/api/v1/transpilation_and_execution")
      .methods(crow::HTTPMethod::POST)(transpile_and_execute);

  std::string port{ "8080" };

  if (auto e_port{ std::getenv("PORT") }; e_port)
    port = e_port;

  app.port(std::stoi(port)).multithreaded().run();

  return 0;
}
