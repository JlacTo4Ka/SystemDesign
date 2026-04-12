#pragma once

#include <string>

namespace jwt_auth::models {

struct FileItem {
  int id{};
  int folder_id{};
  int owner_user_id{};
  std::string name;
  std::string storage_key;
  std::string download_url;
  std::string mime_type;
  long long size_bytes{};
};

}  // namespace jwt_auth::models