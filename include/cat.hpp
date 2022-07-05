#pragma once

#include <string>

namespace cat
{

std::string execute(const std::string& program);
bool transpile(const std::string& source, std::string& result, const std::string& file = "<repl>");

}
