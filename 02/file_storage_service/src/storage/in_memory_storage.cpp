#include <storage/in_memory_storage.hpp>

#include <algorithm>

#include <utils/string_utils.hpp>

namespace jwt_auth::storage {

models::User InMemoryStorageRepo::CreateUser(
    const std::string& login,
    const std::string& password,
    const std::string& first_name,
    const std::string& last_name) {

  std::lock_guard lock(m_mutex);

  models::User user;
  user.id = next_user_id_++;
  user.login = login;
  user.password = password;
  user.first_name = first_name;
  user.last_name = last_name;

  m_users.push_back(user);

  return user;
}

std::optional<models::User> InMemoryStorageRepo::GetUserByLogin(
    const std::string& login) const {
  
  std::lock_guard lock(m_mutex);

  for (const auto& user : m_users) {
    if (user.login == login) {
      return user;
    }
  }

  return std::nullopt;
}

std::vector<models::User> InMemoryStorageRepo::FindUsersByNameMask(
    const std::string& mask) const {
  std::lock_guard lock(m_mutex);

  std::vector<models::User> result;
  for (const auto& user : m_users) {
    if (ContainsMask(user.first_name, mask) || ContainsMask(user.last_name, mask)) {
      result.push_back(user);
    }
  }

  return result;
}

std::optional<models::User> InMemoryStorageRepo::GetUserById(int user_id) const {
  std::lock_guard lock(m_mutex);

  for (const auto& user : m_users) {
    if (user.id == user_id) {
      return user; 
    }
  }

  return std::nullopt; 
}

models::Folder InMemoryStorageRepo::CreateFolder(
    int owner_user_id,
    const std::string& name) {
  std::lock_guard lock(m_mutex);

  models::Folder folder;
  folder.id = next_folder_id_++;
  folder.name = name;
  folder.owner_user_id = owner_user_id;

  m_folders.push_back(folder);
  return folder;
}

std::vector<models::Folder> InMemoryStorageRepo::GetAllFolders() const {
  std::lock_guard lock(m_mutex);
  return m_folders;
}

std::optional<models::Folder> InMemoryStorageRepo::GetFolderById(int folder_id) const {
  std::lock_guard lock(m_mutex);

  for (const auto& folder : m_folders) {
    if (folder.id == folder_id) {
      return folder;
    }
  }

  return std::nullopt;
}

bool InMemoryStorageRepo::DeleteFolder(int folder_id) {
  std::lock_guard lock(m_mutex);

  bool folder_removed = false;

  for (auto it = m_folders.begin(); it != m_folders.end(); ) {
    if (it->id == folder_id) {

      it = m_folders.erase(it); 
      folder_removed = true;
      break; 

    } else {
      ++it;
    }
  }

  if (!folder_removed) {
    return false;
  }

  for (auto it = m_files.begin(); it != m_files.end(); ) {
    if (it->folder_id == folder_id) {
        
      it = m_files.erase(it); 
    } else {
      ++it;
    }
  }

  return true;
}

models::FileItem InMemoryStorageRepo::CreateFile(
    int folder_id,
    int owner_user_id,
    const std::string& name,
    const std::string& content) {
  std::lock_guard lock(m_mutex);

  models::FileItem file;
  file.id = next_file_id_++;
  file.folder_id = folder_id;
  file.owner_user_id = owner_user_id;
  file.name = name;
  file.content = content;

  m_files.push_back(file);
  return file;
}

std::optional<models::FileItem> InMemoryStorageRepo::GetFileByName(
    const std::string& name) const {
  std::lock_guard lock(m_mutex);

  for (const auto& file : m_files) {
    if (file.name == name) {
      return file;
    }
  }

  return std::nullopt;
}

std::optional<models::FileItem> InMemoryStorageRepo::GetFileById(int file_id) const {
  std::lock_guard lock(m_mutex);

  for (const auto& file : m_files) {
    if (file.id == file_id) {
      return file;
    }
  }

  return std::nullopt;
}
bool InMemoryStorageRepo::DeleteFile(int file_id) {
  std::lock_guard lock(m_mutex);

  for (auto it = m_files.begin(); it != m_files.end(); ++it) {
    if (it->id == file_id) {
      m_files.erase(it);
      return true; 
    }
  }

  return false; 
}

bool InMemoryStorageRepo::LoginExists(const std::string& login) const {
  std::lock_guard lock(m_mutex);

  for (const auto& user : m_users) {
    if (user.login == login) {
      return true;
    }
  }

  return false;
}

bool InMemoryStorageRepo::FolderExists(int folder_id) const {
  std::lock_guard lock(m_mutex);

  for (const auto& folder : m_folders) {
    if (folder.id == folder_id) {
      return true;
    }
  }

  return false;
}

bool InMemoryStorageRepo::ContainsMask(
    const std::string& value,
    const std::string& mask) {
  if (mask.empty()) {
    return true;
  }

  const auto lower_value = utils::StringUtils::ToLowerCopy(value);
  const auto lower_mask = utils::StringUtils::ToLowerCopy(mask);

  return lower_value.find(lower_mask) != std::string::npos;
}

std::optional<models::User> InMemoryStorageRepo::GetUserByLoginAndPassword(
    const std::string& login,
    const std::string& password) const {
  std::lock_guard lock(m_mutex);

  for (const auto& user : m_users) {
    if (user.login == login && user.password == password) {
      return user;
    }
  }

  return std::nullopt;
}

}  // namespace jwt_auth::storage