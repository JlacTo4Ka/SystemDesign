#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include <storage/mongo_folder_storage.hpp>

namespace jwt_auth::mongo_folders {

class DeleteHandler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-mongo-delete-folder";

  DeleteHandler(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& context) const override;

 private:
  storage::MongoFolderStorage& m_storage;
};

}  // namespace jwt_auth::mongo_folders