#pragma once

#include <chrono>
#include <optional>
#include <string>

#include <userver/components/component_base.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/redis/client.hpp>
#include <userver/yaml_config/schema.hpp>

namespace jwt_auth::cache {

class RedisCacheComponent final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "redis-cache-component";

  RedisCacheComponent(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

  std::optional<std::string> Get(const std::string& key) const;
  void Set(const std::string& key, const std::string& value,
           std::chrono::seconds ttl) const;
  void Delete(const std::string& key) const;

  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  userver::storages::redis::ClientPtr redis_client_;
  userver::storages::redis::CommandControl redis_cc_;
};

}  // namespace jwt_auth::cache