#pragma once

#include <string>

namespace jwt_auth::models {

struct Folder {
  int id{};
  std::string name;
  int owner_user_id{};
};

}  // namespace jwt_auth::models