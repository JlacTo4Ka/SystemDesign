#pragma once

#include <optional>
#include <string>
#include <vector>

#include <userver/storages/mongo/pool.hpp>

#include <models/folder.hpp>

namespace jwt_auth::storage {

class MongoFolderStorage final {
 public:
  explicit MongoFolderStorage(userver::storages::mongo::PoolPtr pool);

  models::Folder CreateFolder(int owner_user_id, const std::string& name);
  std::vector<models::Folder> GetAllFolders() const;
  std::optional<models::Folder> GetFolderById(int folder_id) const;
  bool DeleteFolder(int folder_id);

 private:
  static models::Folder ParseFolder(const userver::formats::bson::Document& doc);

  userver::storages::mongo::PoolPtr pool_;
};

}  // namespace jwt_auth::storage