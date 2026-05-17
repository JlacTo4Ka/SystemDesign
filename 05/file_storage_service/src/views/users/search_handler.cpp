#include <views/users/search_handler.hpp>

#include <userver/formats/json/serialize.hpp>
#include <userver/server/http/http_status.hpp>

#include <storage/storage_component.hpp>

namespace jwt_auth::users {

namespace {

userver::formats::json::Value MakeUsersResponse(
    const std::vector<models::User>& users) {
  userver::formats::json::ValueBuilder builder(userver::formats::common::Type::kArray);

  for (const auto& user : users) {
    userver::formats::json::ValueBuilder item;
    item["id"] = user.id;
    item["login"] = user.login;
    item["first_name"] = user.first_name;
    item["last_name"] = user.last_name;
    builder.PushBack(item.ExtractValue());
  }

  return builder.ExtractValue();
}

}  // namespace

SearchHandler::SearchHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(
          context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string SearchHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  const auto mask = request.GetArg("mask");
  const auto users = m_storage.FindUsersByNameMask(mask);

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(MakeUsersResponse(users));
}

}  // namespace jwt_auth::users