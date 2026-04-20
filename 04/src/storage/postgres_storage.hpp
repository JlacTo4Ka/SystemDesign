#pragma once

#include <optional>
#include <string>
#include <vector>

#include <userver/storages/postgres/cluster.hpp>

#include "../models/file_item.hpp"
#include "../models/folder.hpp"
#include "../models/user.hpp"

namespace jwt_auth::storage {

class PostgresStorage final {
 public:
  explicit PostgresStorage(userver::storages::postgres::ClusterPtr cluster);

  models::User CreateUser(
      const std::string& login,
      const std::string& password,
      const std::string& first_name,
      const std::string& last_name);

  std::optional<models::User> GetUserByLogin(const std::string& login) const;
  std::vector<models::User> FindUsersByNameMask(const std::string& mask) const;
  std::optional<models::User> GetUserById(int user_id) const;

  models::Folder CreateFolder(int owner_user_id, const std::string& name);
  std::vector<models::Folder> GetAllFolders() const;
  std::optional<models::Folder> GetFolderById(int folder_id) const;
  bool DeleteFolder(int folder_id);

  models::FileItem CreateFile(
      int folder_id,
      int owner_user_id,
      const std::string& name,
      const std::string& content,
      const std::string& mime_type = "application/octet-stream");

  std::optional<models::FileItem> GetFileByName(const std::string& name) const;
  std::optional<models::FileItem> GetFileById(int file_id) const;
  bool DeleteFile(int file_id);
  bool LoginExists(const std::string& login) const;
  bool FolderExists(int folder_id) const;

  std::optional<models::User> GetUserByLoginAndPassword(
      const std::string& login,
      const std::string& password) const;

 private:
  static models::User ParseUser(const userver::storages::postgres::Row& row);
  static models::Folder ParseFolder(const userver::storages::postgres::Row& row);
  static models::FileItem ParseFile(const userver::storages::postgres::Row& row);
  static std::string MakeStorageKey(
      int owner_user_id,
      int folder_id,
      const std::string& name);
  static std::string MakeDownloadUrl(const std::string& storage_key);

  userver::storages::postgres::ClusterPtr cluster_;
};

}  // namespace jwt_auth::storage
