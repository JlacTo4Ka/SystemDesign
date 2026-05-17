#include <dto/users/user_dto.hpp>

namespace jwt_auth::dto::users {

CreateUserRequestDto CreateUserRequestFromJson(
    const userver::formats::json::Value& json) {
  CreateUserRequestDto dto;
  dto.login = json["login"].As<std::string>("");
  dto.password = json["password"].As<std::string>("");
  dto.first_name = json["first_name"].As<std::string>("");
  dto.last_name = json["last_name"].As<std::string>("");
  return dto;
}

userver::formats::json::Value UserResponseToJson(
    const UserResponseDto& dto) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = dto.id;
  builder["login"] = dto.login;
  builder["first_name"] = dto.first_name;
  builder["last_name"] = dto.last_name;
  return builder.ExtractValue();
}

UserResponseDto ToUserResponseDto(const models::User& user) {
  UserResponseDto dto;
  dto.id = user.id;
  dto.login = user.login;
  dto.first_name = user.first_name;
  dto.last_name = user.last_name;
  return dto;
}

}  // namespace jwt_auth::dto::users