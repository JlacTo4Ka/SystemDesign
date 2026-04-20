#pragma once

#include <userver/components/component_base.hpp>
#include <userver/components/component_context.hpp>
#include <userver/yaml_config/schema.hpp>

#include <storage/mongo_folder_storage.hpp>

namespace jwt_auth::storage {

class MongoStorageComponent final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "mongo-storage-component";

  MongoStorageComponent(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

  MongoFolderStorage& GetStorage();
  const MongoFolderStorage& GetStorage() const;

  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  MongoFolderStorage m_storage;
};

}  // namespace jwt_auth::storage