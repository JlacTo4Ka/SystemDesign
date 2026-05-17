#include <storage/mongo_folder_storage.hpp>

#include <chrono>

#include <userver/formats/bson.hpp>

namespace jwt_auth::storage {

namespace bson = userver::formats::bson;

MongoFolderStorage::MongoFolderStorage(userver::storages::mongo::PoolPtr pool)
    : pool_(std::move(pool)) {}

models::Folder MongoFolderStorage::CreateFolder(
    int owner_user_id,
    const std::string& name) {
  auto collection = pool_->GetCollection("folders");

  const auto now = std::chrono::system_clock::now();
  const int folder_id =
      static_cast<int>(std::chrono::system_clock::to_time_t(now));

  auto doc = bson::MakeDoc(
      "_id", folder_id,
      "owner_user_id", owner_user_id,
      "name", name,
      "category", "documents",
      "shared", false,
      "created_at", now,
      "last_accessed_at", now,
      "tags", bson::MakeArray(),
      "settings", bson::MakeDoc(
          "color", "blue",
          "pinned", false,
          "access_level", "private"
      ),
      "files", bson::MakeArray()
  );

  collection.InsertOne(doc);

  models::Folder folder;
  folder.id = folder_id;
  folder.owner_user_id = owner_user_id;
  folder.name = name;
  return folder;
}

std::vector<models::Folder> MongoFolderStorage::GetAllFolders() const {
  std::vector<models::Folder> folders;

  auto collection = pool_->GetCollection("folders");
  auto cursor = collection.Find(bson::MakeDoc());

  for (const auto& doc : cursor) {
    folders.push_back(ParseFolder(doc));
  }

  return folders;
}

std::optional<models::Folder> MongoFolderStorage::GetFolderById(int folder_id) const {
  auto collection = pool_->GetCollection("folders");
  auto doc = collection.FindOne(bson::MakeDoc("_id", folder_id));

  if (!doc) {
    return std::nullopt;
  }

  return ParseFolder(*doc);
}

bool MongoFolderStorage::DeleteFolder(int folder_id) {
  auto collection = pool_->GetCollection("folders");
  auto result = collection.DeleteOne(bson::MakeDoc("_id", folder_id));
  return result.DeletedCount() > 0;
}

models::Folder MongoFolderStorage::ParseFolder(const bson::Document& doc) {
  models::Folder folder;
  folder.id = doc["_id"].As<int>();
  folder.owner_user_id = doc["owner_user_id"].As<int>();
  folder.name = doc["name"].As<std::string>();
  return folder;
}

}  // namespace jwt_auth::storage