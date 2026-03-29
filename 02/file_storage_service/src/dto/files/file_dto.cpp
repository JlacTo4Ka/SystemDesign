#include <dto/files/file_dto.hpp>

namespace jwt_auth::dto::files {

CreateFileRequestDto CreateFileRequestFromJson(const userver::formats::json::Value& json) {
  CreateFileRequestDto dto;
  dto.name = json["name"].As<std::string>("");
  dto.content = json["content"].As<std::string>("");
  dto.owner_user_id = json["owner_user_id"].As<int>(0);
  return dto;
}

FileResponseDto ToFileResponseDto(const models::FileItem& file) {
  FileResponseDto dto;
  dto.id = file.id;
  dto.folder_id = file.folder_id;
  dto.owner_user_id = file.owner_user_id;
  dto.name = file.name;
  dto.content = file.content;
  return dto;
}

userver::formats::json::Value FileResponseToJson(const FileResponseDto& dto) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = dto.id;
  builder["folder_id"] = dto.folder_id;
  builder["owner_user_id"] = dto.owner_user_id;
  builder["name"] = dto.name;
  builder["content"] = dto.content;
  return builder.ExtractValue();
}

}  // namespace jwt_auth::dto::files