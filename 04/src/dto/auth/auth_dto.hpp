#pragma once

#include <string>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace jwt_auth::dto::auth {

struct LoginRequestDto {
  std::string login;
  std::string password;
};

struct LoginResponseDto {
  std::string token;
  int user_id{};
  std::string login;
};

LoginRequestDto LoginRequestFromJson(
    const userver::formats::json::Value& json);

userver::formats::json::Value LoginResponseToJson(
    const LoginResponseDto& dto);

}  // namespace jwt_auth::dto::auth