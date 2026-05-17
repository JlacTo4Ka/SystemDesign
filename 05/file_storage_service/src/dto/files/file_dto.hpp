#pragma once

#include <string>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <models/file_item.hpp>

namespace jwt_auth::dto::files {

struct CreateFileRequestDto {
  std::string name;
  std::string content;
  std::string mime_type;
  int owner_user_id{};
};

struct FileResponseDto {
  int id{};
  int folder_id{};
  int owner_user_id{};
  std::string name;
  std::string storage_key;
  std::string download_url;
  std::string mime_type;
  long long size_bytes{};
};

CreateFileRequestDto CreateFileRequestFromJson(
    const userver::formats::json::Value& json);

FileResponseDto ToFileResponseDto(const models::FileItem& file);

userver::formats::json::Value FileResponseToJson(
    const FileResponseDto& dto);

}  // namespace jwt_auth::dto::files
