#include <dto/auth/auth_dto.hpp>

namespace jwt_auth::dto::auth {

LoginRequestDto LoginRequestFromJson(const userver::formats::json::Value& json) {
  LoginRequestDto dto;
  dto.login = json["login"].As<std::string>("");
  dto.password = json["password"].As<std::string>("");
  return dto;
}

userver::formats::json::Value LoginResponseToJson(const LoginResponseDto& dto) {
  userver::formats::json::ValueBuilder builder;
  builder["token"] = dto.token;
  builder["user_id"] = dto.user_id;
  builder["login"] = dto.login;
  return builder.ExtractValue();
}

}  // namespace jwt_auth::dto::auth