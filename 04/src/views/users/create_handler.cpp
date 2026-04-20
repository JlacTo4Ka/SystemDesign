#include <views/users/create_handler.hpp>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <dto/users/user_dto.hpp>
#include <storage/storage_component.hpp>

#include <userver/logging/log.hpp>

namespace jwt_auth::users {

CreateHandler::CreateHandler(const userver::components::ComponentConfig& config, 
        const userver::components::ComponentContext& context) : HttpHandlerBase(config, context),
        m_storage(context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string CreateHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
        
  LOG_INFO() << "create user: start";

  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  LOG_INFO() << "create user: before parse json";
  const auto json = userver::formats::json::FromString(request.RequestBody());

  LOG_INFO() << "create user: after parse json";
  const auto request_dto = dto::users::CreateUserRequestFromJson(json);

  LOG_INFO() << "create user: login=" << request_dto.login;

  userver::formats::json::ValueBuilder error;

  if (request_dto.login.empty() || request_dto.password.empty() ||
      request_dto.first_name.empty() || request_dto.last_name.empty()) {
    LOG_INFO() << "create user: bad request";
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "login, password, first_name and last_name are required";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  LOG_INFO() << "create user: before LoginExists";
  if (m_storage.LoginExists(request_dto.login)) {
    LOG_INFO() << "create user: login exists";
    response.SetStatus(userver::server::http::HttpStatus::kConflict);
    error["error"] = "user with this login already exists";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  LOG_INFO() << "create user: before CreateUser";
  const auto user = m_storage.CreateUser(
      request_dto.login,
      request_dto.password,
      request_dto.first_name,
      request_dto.last_name);

  LOG_INFO() << "create user: after CreateUser id=" << user.id;

  const auto response_dto = dto::users::ToUserResponseDto(user);

  LOG_INFO() << "create user: before response";
  response.SetStatus(userver::server::http::HttpStatus::kCreated);

  auto result = userver::formats::json::ToString(
      dto::users::UserResponseToJson(response_dto));

  LOG_INFO() << "create user: finish";
  return result;
}

}  // namespace jwt_auth::users