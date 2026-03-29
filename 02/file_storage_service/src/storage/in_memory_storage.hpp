#pragma once

#include <optional>
#include <string>
#include <mutex>

#include <vector>

#include "../models/file_item.hpp"
#include "../models/folder.hpp"
#include "../models/user.hpp"

namespace jwt_auth::storage {

class InMemoryStorageRepo final {
 public:
  InMemoryStorageRepo() = default;

  // user
  models::User CreateUser(
      const std::string& login,
      const std::string& password,
      const std::string& first_name,
      const std::string& last_name);

  std::optional<models::User> GetUserByLogin(const std::string& login) const;
  std::vector<models::User> FindUsersByNameMask(const std::string& mask) const;
  std::optional<models::User> GetUserById(int user_id) const;

  // folder
  models::Folder CreateFolder(int owner_user_id, const std::string& name);
  std::vector<models::Folder> GetAllFolders() const;
  std::optional<models::Folder> GetFolderById(int folder_id) const;
  bool DeleteFolder(int folder_id);

  // files
  models::FileItem CreateFile(
      int folder_id,
      int owner_user_id,
      const std::string& name,
      const std::string& content);

  std::optional<models::FileItem> GetFileByName(const std::string& name) const;
  std::optional<models::FileItem> GetFileById(int file_id) const;
  bool DeleteFile(int file_id);
  bool LoginExists(const std::string& login) const;
  bool FolderExists(int folder_id) const;

  std::optional<models::User> GetUserByLoginAndPassword(
    const std::string& login,
    const std::string& password) const;
 private:
  static bool ContainsMask(const std::string& value, const std::string& mask);

  mutable std::mutex m_mutex;

  std::vector<models::User> m_users;
  std::vector<models::Folder> m_folders;
  std::vector<models::FileItem> m_files;

  int next_user_id_{1};
  int next_folder_id_{1};
  int next_file_id_{1};
};

}  // namespace jwt_auth::storage