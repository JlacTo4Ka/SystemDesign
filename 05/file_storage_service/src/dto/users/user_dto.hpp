#pragma once

#include <string>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <models/user.hpp>

namespace jwt_auth::dto::users {

struct CreateUserRequestDto {
  std::string login;
  std::string password;
  std::string first_name;
  std::string last_name;
};

struct UserResponseDto {
  int id{};
  std::string login;
  std::string first_name;
  std::string last_name;
};

CreateUserRequestDto CreateUserRequestFromJson(
    const userver::formats::json::Value& json);

userver::formats::json::Value UserResponseToJson(
    const UserResponseDto& dto);

UserResponseDto ToUserResponseDto(const models::User& user);

}  // namespace jwt_auth::dto::users