#pragma once

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/redis/client.hpp>

#include <storage/mongo_storage_component.hpp>
#include <storage/storage_component.hpp>

namespace jwt_auth::mongo_folders {

class CreateHandler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-mongo-create-folder";

  CreateHandler(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& context) const override;

 private:
  storage::MongoFolderStorage& m_mongo_storage;
  storage::PostgresStorage& m_postgres_storage;
  userver::storages::redis::ClientPtr redis_client_;
  userver::storages::redis::CommandControl redis_cc_;
};

}  // namespace jwt_auth::mongo_folders