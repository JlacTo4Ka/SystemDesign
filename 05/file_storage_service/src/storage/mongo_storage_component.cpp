#include <storage/mongo_storage_component.hpp>

#include <userver/storages/mongo/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace jwt_auth::storage {

MongoStorageComponent::MongoStorageComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::components::ComponentBase(config, context),
      m_storage(
          context.FindComponent<userver::components::Mongo>("mongo-folder-db")
              .GetPool()) {
}

MongoFolderStorage& MongoStorageComponent::GetStorage() {
  return m_storage;
}

const MongoFolderStorage& MongoStorageComponent::GetStorage() const {
  return m_storage;
}

userver::yaml_config::Schema MongoStorageComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: mongo storage component
additionalProperties: false
properties: {}
)");
}

}  // namespace jwt_auth::storage