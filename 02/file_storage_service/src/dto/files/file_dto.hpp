#pragma once

#include <string>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <models/file_item.hpp>

namespace jwt_auth::dto::files {

struct CreateFileRequestDto {
  std::string name;
  std::string content;
  int owner_user_id{};
};

struct FileResponseDto {
  int id{};
  int folder_id{};
  int owner_user_id{};
  std::string name;
  std::string content;
};

CreateFileRequestDto CreateFileRequestFromJson(
    const userver::formats::json::Value& json);

FileResponseDto ToFileResponseDto(const models::FileItem& file);

userver::formats::json::Value FileResponseToJson(
    const FileResponseDto& dto);

}  // namespace jwt_auth::dto::files