#pragma once

#include "crow_all.h"

using Payload = std::initializer_list<std::pair<std::string const, crow::json::wvalue> >;

class success_response : public crow::returnable
{
public:
  success_response(Payload payload) : returnable("application/json")
  {
    m_payload["status"] = "success";
    m_payload["data"] = payload;
  }

  std::string
  dump() const override
  {
    return m_payload.dump();
  }

private:
  crow::json::wvalue m_payload = {};
};

class error_response final : public crow::returnable
{
public:
  error_response(const std::string& msg) : returnable("application/json")
  {
    m_payload["status"] = "error";
    m_payload["error"] = msg;
  }

  std::string
  dump() const override
  {
    return m_payload.dump();
  }

private:
  crow::json::wvalue m_payload = {};
};
