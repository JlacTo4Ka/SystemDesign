#include <views/auth/login_handler.hpp>

#include <jwt-cpp/jwt.h>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <auth/jwt_auth_checker.hpp>
#include <dto/auth/auth_dto.hpp>
#include <storage/storage_component.hpp>

namespace jwt_auth::auth_handlers {

LoginHandler::LoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(context.FindComponent<storage::StorageComponent>().GetStorage()),
      m_secret(context.FindComponent<auth::jwt::JwtAuthComponent>().GetSecret()) {}

std::string LoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  userver::formats::json::ValueBuilder error;

  userver::formats::json::Value json;
  try {
    json = userver::formats::json::FromString(request.RequestBody());
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "invalid json body";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto request_dto = dto::auth::LoginRequestFromJson(json);

  if (request_dto.login.empty() || request_dto.password.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "login and password are required";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto user =
      m_storage.GetUserByLoginAndPassword(request_dto.login, request_dto.password);

  if (!user.has_value()) {
    response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
    error["error"] = "invalid login or password";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto token = ::jwt::create()
                         .set_issuer("sample")
                         .set_type("JWT")
                         .set_payload_claim("login", ::jwt::claim(user->login))
                         .set_payload_claim("user_id", ::jwt::claim(std::to_string(user->id)))
                         .sign(::jwt::algorithm::hs256{m_secret});

  dto::auth::LoginResponseDto response_dto;
  response_dto.token = token;
  response_dto.user_id = user->id;
  response_dto.login = user->login;

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(
      dto::auth::LoginResponseToJson(response_dto));
}

}  // namespace jwt_auth::auth_handlers