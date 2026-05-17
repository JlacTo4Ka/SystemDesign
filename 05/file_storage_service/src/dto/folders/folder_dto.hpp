#pragma once

#include <string>
#include <vector>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <models/folder.hpp>

namespace jwt_auth::dto::folders {

struct CreateFolderRequestDto {
  std::string name;
  int owner_user_id{};
};

struct FolderResponseDto {
  int id{};
  std::string name;
  int owner_user_id{};
};

CreateFolderRequestDto CreateFolderRequestFromJson(
    const userver::formats::json::Value& json);

FolderResponseDto ToFolderResponseDto(const models::Folder& folder);

userver::formats::json::Value FolderResponseToJson(
    const FolderResponseDto& dto);

userver::formats::json::Value FoldersResponseToJson(
    const std::vector<FolderResponseDto>& dtos);

}  // namespace jwt_auth::dto::folders