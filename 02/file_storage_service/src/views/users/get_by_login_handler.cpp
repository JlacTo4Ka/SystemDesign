#include <views/users/get_by_login_handler.hpp>

#include <userver/formats/json/serialize.hpp>
#include <userver/server/http/http_status.hpp>

#include <storage/storage_component.hpp>

namespace jwt_auth::users {

namespace {

userver::formats::json::Value MakeErrorResponse(const std::string& message) {
  userver::formats::json::ValueBuilder builder;
  
  builder["error"] = message;
  
  return builder.ExtractValue();
}

userver::formats::json::Value MakeUserResponse(const models::User& user) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = user.id;
  builder["login"] = user.login;
  builder["first_name"] = user.first_name;
  builder["last_name"] = user.last_name;
  return builder.ExtractValue();
}

}  // namespace

GetByLoginHandler::GetByLoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage( context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string GetByLoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  const auto login = request.GetPathArg("login");

  if (login.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    
    return userver::formats::json::ToString(
        MakeErrorResponse("login is required"));
  }

  const auto user = m_storage.GetUserByLogin(login);
  if (!user.has_value()) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    
    return userver::formats::json::ToString(
        MakeErrorResponse("user not found"));
  }

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  
  return userver::formats::json::ToString(MakeUserResponse(*user));
}

}  // namespace jwt_auth::users