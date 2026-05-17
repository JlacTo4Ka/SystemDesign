#pragma once

#include <userver/components/component_base.hpp>
#include <userver/components/component_context.hpp>
#include <userver/yaml_config/schema.hpp>

#include <storage/postgres_storage.hpp>

namespace jwt_auth::storage {

class StorageComponent final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "storage-component";

  StorageComponent(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

  PostgresStorage& GetStorage();
  const PostgresStorage& GetStorage() const;

  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  PostgresStorage m_storage;
};

}  // namespace jwt_auth::storage