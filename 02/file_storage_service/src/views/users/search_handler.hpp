#pragma once

#include <userver/server/handlers/http_handler_base.hpp>

#include <storage/in_memory_storage.hpp>

namespace jwt_auth::users {

class SearchHandler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-search-users";

  SearchHandler(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& context) const override;

 private:
  storage::InMemoryStorageRepo& m_storage;
};

}  // namespace jwt_auth::users