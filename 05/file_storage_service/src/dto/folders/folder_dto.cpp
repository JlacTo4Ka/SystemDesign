#include <dto/folders/folder_dto.hpp>

namespace jwt_auth::dto::folders {

CreateFolderRequestDto CreateFolderRequestFromJson(
    const userver::formats::json::Value& json) {
  CreateFolderRequestDto dto;
  dto.name = json["name"].As<std::string>("");
  dto.owner_user_id = json["owner_user_id"].As<int>(0);
  return dto;
}

FolderResponseDto ToFolderResponseDto(const models::Folder& folder) {
  FolderResponseDto dto;
  dto.id = folder.id;
  dto.name = folder.name;
  dto.owner_user_id = folder.owner_user_id;
  return dto;
}

userver::formats::json::Value FolderResponseToJson(
    const FolderResponseDto& dto) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = dto.id;
  builder["name"] = dto.name;
  builder["owner_user_id"] = dto.owner_user_id;
  return builder.ExtractValue();
}

userver::formats::json::Value FoldersResponseToJson(
    const std::vector<FolderResponseDto>& dtos) {
  userver::formats::json::ValueBuilder builder(userver::formats::common::Type::kArray);

  for (const auto& dto : dtos) {
    builder.PushBack(FolderResponseToJson(dto));
  }

  return builder.ExtractValue();
}

}  // namespace jwt_auth::dto::folders