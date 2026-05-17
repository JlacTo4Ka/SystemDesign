#include <cache/redis_cache_component.hpp>

#include <userver/storages/redis/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace jwt_auth::cache {

RedisCacheComponent::RedisCacheComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::components::ComponentBase(config, context),
      redis_client_(
          context.FindComponent<userver::components::Redis>("redis-cache")
              .GetClient("cache-db")),
      redis_cc_(std::chrono::seconds{2}, std::chrono::seconds{2}, 2) {}

std::optional<std::string> RedisCacheComponent::Get(
    const std::string& key) const {
  return redis_client_->Get(key, redis_cc_).Get();
}

void RedisCacheComponent::Set(
    const std::string& key,
    const std::string& value,
    std::chrono::seconds ttl) const {
  redis_client_->Set(
      key,
      value,
      std::chrono::duration_cast<std::chrono::milliseconds>(ttl),
      redis_cc_).Get();
}

void RedisCacheComponent::Delete(const std::string& key) const {
  redis_client_->Del(key, redis_cc_).Get();
}

userver::yaml_config::Schema RedisCacheComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
        type: object
        description: redis cache component
        additionalProperties: false
        properties: {}
        )");
}

}  // namespace jwt_auth::cache