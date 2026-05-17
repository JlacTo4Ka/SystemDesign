#include <storage/storage_component.hpp>

#include <userver/storages/postgres/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace jwt_auth::storage {

StorageComponent::StorageComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::components::ComponentBase(config, context),
      m_storage(
          context.FindComponent<userver::components::Postgres>("postgres-db")
              .GetCluster()) {}

PostgresStorage& StorageComponent::GetStorage() {
  return m_storage;
}

const PostgresStorage& StorageComponent::GetStorage() const {
  return m_storage;
}

userver::yaml_config::Schema StorageComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: storage component that works with postgres
    additionalProperties: false
    properties: {}
    )");
}

}  // namespace jwt_auth::storage