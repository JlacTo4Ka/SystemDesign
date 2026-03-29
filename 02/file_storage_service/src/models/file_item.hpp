#pragma once

#include <string>

namespace jwt_auth::models {

struct FileItem {
  int id{};
  int folder_id{};
  int owner_user_id{};
  std::string name;
  std::string content;
};

}  // namespace jwt_auth::models