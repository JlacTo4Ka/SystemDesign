#pragma once

#include <string>

namespace jwt_auth::models {

struct User {
  int id{};
  std::string login;
  std::string password;
  std::string first_name;
  std::string last_name;
};

}  // namespace jwt_auth::models