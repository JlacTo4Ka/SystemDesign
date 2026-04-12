#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include <storage/postgres_storage.hpp>

namespace jwt_auth::auth_handlers {

class LoginHandler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-login";

  LoginHandler(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& context) const override;

 private:
  storage::PostgresStorage& m_storage;
  std::string m_secret;
};

}  // namespace jwt_auth::auth_handlers