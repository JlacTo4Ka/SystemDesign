#include <storage/postgres_storage.hpp>

#include <chrono>

#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/query.hpp>

namespace pg = userver::storages::postgres;

namespace jwt_auth::storage {
namespace {

const pg::Query kCreateUser{
    "INSERT INTO users(email, password_hash, first_name, last_name) "
    "VALUES($1, $2, $3, $4) "
    "RETURNING id, email, password_hash, first_name, last_name"};

const pg::Query kGetUserByLogin{
    "SELECT id, email, password_hash, first_name, last_name "
    "FROM users WHERE email = $1 LIMIT 1"};

const pg::Query kGetUserById{
    "SELECT id, email, password_hash, first_name, last_name "
    "FROM users WHERE id = $1 LIMIT 1"};

const pg::Query kFindUsersByMask{
    "SELECT id, email, password_hash, first_name, last_name "
    "FROM users "
    "WHERE first_name ILIKE '%' || $1 || '%' "
    "   OR last_name ILIKE '%' || $1 || '%' "
    "ORDER BY id"};

const pg::Query kCreateFolder{
    "INSERT INTO folders(owner_user_id, name) "
    "VALUES($1, $2) "
    "RETURNING id, owner_user_id, name"};

const pg::Query kGetAllFolders{
    "SELECT id, owner_user_id, name "
    "FROM folders ORDER BY id"};

const pg::Query kGetFolderById{
    "SELECT id, owner_user_id, name "
    "FROM folders WHERE id = $1 LIMIT 1"};

const pg::Query kDeleteFolder{
    "DELETE FROM folders WHERE id = $1"};

const pg::Query kCreateFile{
    "INSERT INTO files(folder_id, owner_user_id, name, storage_key, download_url, mime_type, size_bytes) "
    "VALUES($1, $2, $3, $4, $5, $6, $7) "
    "RETURNING id, folder_id, owner_user_id, name, storage_key, download_url, mime_type, size_bytes"};

const pg::Query kGetFileByName{
    "SELECT id, folder_id, owner_user_id, name, storage_key, download_url, mime_type, size_bytes "
    "FROM files WHERE name = $1 ORDER BY id LIMIT 1"};

const pg::Query kGetFileById{
    "SELECT id, folder_id, owner_user_id, name, storage_key, download_url, mime_type, size_bytes "
    "FROM files WHERE id = $1 LIMIT 1"};

const pg::Query kDeleteFile{
    "DELETE FROM files WHERE id = $1"};

const pg::Query kGetUserByLoginAndPassword{
    "SELECT id, email, password_hash, first_name, last_name "
    "FROM users WHERE email = $1 AND password_hash = $2 LIMIT 1"};

}  // namespace

PostgresStorage::PostgresStorage(pg::ClusterPtr cluster)
    : cluster_(std::move(cluster)) {}

models::User PostgresStorage::CreateUser(
    const std::string& login,
    const std::string& password,
    const std::string& first_name,
    const std::string& last_name) {
  auto res = cluster_->Execute(
      pg::ClusterHostType::kMaster,
      kCreateUser,
      login,
      password,
      first_name,
      last_name);

  return ParseUser(res.Front());
}

std::optional<models::User> PostgresStorage::GetUserByLogin(
    const std::string& login) const {
  auto res = cluster_->Execute(pg::ClusterHostType::kSlave, kGetUserByLogin, login);
  if (res.IsEmpty()) {
    return std::nullopt;
  }

  return ParseUser(res.Front());
}

std::vector<models::User> PostgresStorage::FindUsersByNameMask(
    const std::string& mask) const {
  std::vector<models::User> users;

  auto res = cluster_->Execute(pg::ClusterHostType::kSlave, kFindUsersByMask, mask);
  users.reserve(res.Size());

  for (const auto& row : res) {
    users.push_back(ParseUser(row));
  }

  return users;
}

std::optional<models::User> PostgresStorage::GetUserById(int user_id) const {
  auto res = cluster_->Execute(pg::ClusterHostType::kSlave, kGetUserById, user_id);
  if (res.IsEmpty()) {
    return std::nullopt;
  }

  return ParseUser(res.Front());
}

models::Folder PostgresStorage::CreateFolder(
    int owner_user_id,
    const std::string& name) {
  auto res = cluster_->Execute(
      pg::ClusterHostType::kMaster,
      kCreateFolder,
      owner_user_id,
      name);

  return ParseFolder(res.Front());
}

std::vector<models::Folder> PostgresStorage::GetAllFolders() const {
  std::vector<models::Folder> folders;

  auto res = cluster_->Execute(pg::ClusterHostType::kSlave, kGetAllFolders);
  folders.reserve(res.Size());

  for (const auto& row : res) {
    folders.push_back(ParseFolder(row));
  }

  return folders;
}

std::optional<models::Folder> PostgresStorage::GetFolderById(int folder_id) const {
  auto res = cluster_->Execute(pg::ClusterHostType::kSlave, kGetFolderById, folder_id);
  if (res.IsEmpty()) {
    return std::nullopt;
  }

  return ParseFolder(res.Front());
}

bool PostgresStorage::DeleteFolder(int folder_id) {
  auto res = cluster_->Execute(pg::ClusterHostType::kMaster, kDeleteFolder, folder_id);
  return res.RowsAffected() > 0;
}

models::FileItem PostgresStorage::CreateFile(
    int folder_id,
    int owner_user_id,
    const std::string& name,
    const std::string& content,
    const std::string& mime_type) {
  const auto storage_key = MakeStorageKey(owner_user_id, folder_id, name);
  const auto download_url = MakeDownloadUrl(storage_key);
  const auto size_bytes = static_cast<long long>(content.size());

  auto res = cluster_->Execute(
      pg::ClusterHostType::kMaster,
      kCreateFile,
      folder_id,
      owner_user_id,
      name,
      storage_key,
      download_url,
      mime_type,
      size_bytes);

  return ParseFile(res.Front());
}

std::optional<models::FileItem> PostgresStorage::GetFileByName(
    const std::string& name) const {
  auto res = cluster_->Execute(pg::ClusterHostType::kSlave, kGetFileByName, name);
  if (res.IsEmpty()) {
    return std::nullopt;
  }

  return ParseFile(res.Front());
}

std::optional<models::FileItem> PostgresStorage::GetFileById(int file_id) const {
  auto res = cluster_->Execute(pg::ClusterHostType::kSlave, kGetFileById, file_id);
  if (res.IsEmpty()) {
    return std::nullopt;
  }

  return ParseFile(res.Front());
}

bool PostgresStorage::DeleteFile(int file_id) {
  auto res = cluster_->Execute(pg::ClusterHostType::kMaster, kDeleteFile, file_id);
  return res.RowsAffected() > 0;
}

bool PostgresStorage::LoginExists(const std::string& login) const {
  return GetUserByLogin(login).has_value();
}

bool PostgresStorage::FolderExists(int folder_id) const {
  return GetFolderById(folder_id).has_value();
}

std::optional<models::User> PostgresStorage::GetUserByLoginAndPassword(
    const std::string& login,
    const std::string& password) const {
  auto res = cluster_->Execute(
      pg::ClusterHostType::kSlave,
      kGetUserByLoginAndPassword,
      login,
      password);

  if (res.IsEmpty()) {
    return std::nullopt;
  }

  return ParseUser(res.Front());
}

models::User PostgresStorage::ParseUser(const pg::Row& row) {
  models::User user;
  user.id = row["id"].As<int>();
  user.login = row["email"].As<std::string>();
  user.password = row["password_hash"].As<std::string>();
  user.first_name = row["first_name"].As<std::string>();
  user.last_name = row["last_name"].As<std::string>();
  return user;
}

models::Folder PostgresStorage::ParseFolder(const pg::Row& row) {
  models::Folder folder;
  folder.id = row["id"].As<int>();
  folder.owner_user_id = row["owner_user_id"].As<int>();
  folder.name = row["name"].As<std::string>();
  return folder;
}

models::FileItem PostgresStorage::ParseFile(const pg::Row& row) {
  models::FileItem file;
  file.id = row["id"].As<int>();
  file.folder_id = row["folder_id"].As<int>();
  file.owner_user_id = row["owner_user_id"].As<int>();
  file.name = row["name"].As<std::string>();
  file.storage_key = row["storage_key"].As<std::string>();
  file.download_url = row["download_url"].As<std::string>();
  file.mime_type = row["mime_type"].Coalesce<std::string>("application/octet-stream");
  file.size_bytes = row["size_bytes"].As<long long>();
  return file;
}

std::string PostgresStorage::MakeStorageKey(
    int owner_user_id,
    int folder_id,
    const std::string& name) {
  const auto now = std::chrono::system_clock::now().time_since_epoch();
  const auto stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

  return std::to_string(owner_user_id) + "/" +
         std::to_string(folder_id) + "/" +
         std::to_string(stamp) + "_" + name;
}

std::string PostgresStorage::MakeDownloadUrl(const std::string& storage_key) {
  return "https://storage.local/" + storage_key;
}

}  // namespace jwt_auth::storage
