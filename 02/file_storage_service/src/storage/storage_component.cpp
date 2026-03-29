#include <storage/storage_component.hpp>

#include <userver/yaml_config/merge_schemas.hpp>

namespace jwt_auth::storage {

StorageComponent::StorageComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::components::ComponentBase(config, context) {}

InMemoryStorageRepo& StorageComponent::GetStorage() {
  return m_storage;
}

const InMemoryStorageRepo& StorageComponent::GetStorage() const {
  return m_storage;
}

userver::yaml_config::Schema StorageComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
    type: object
    description: in-memory storage component
    additionalProperties: false
    properties: {}
    )");
}

}  // namespace jwt_auth::storage