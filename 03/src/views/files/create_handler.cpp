#include <views/files/create_handler.hpp>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <dto/files/file_dto.hpp>
#include <storage/storage_component.hpp>

namespace jwt_auth::files {

CreateHandler::CreateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string CreateHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  userver::formats::json::ValueBuilder error;

  const auto folder_id_str = request.GetPathArg("folder_id");
  if (folder_id_str.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "folder_id is required";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  int folder_id = 0;
  try {
    folder_id = std::stoi(folder_id_str);
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "folder_id must be integer";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  userver::formats::json::Value json;
  try {
    json = userver::formats::json::FromString(request.RequestBody());
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "invalid json body";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto request_dto = dto::files::CreateFileRequestFromJson(json);

  if (request_dto.name.empty() || request_dto.owner_user_id <= 0) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "name and owner_user_id are required";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto folder = m_storage.GetFolderById(folder_id);
  if (!folder.has_value()) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    error["error"] = "folder not found";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto user = m_storage.GetUserById(request_dto.owner_user_id);
  if (!user.has_value()) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    error["error"] = "owner user not found";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto file = m_storage.CreateFile(
      folder_id,
      request_dto.owner_user_id,
      request_dto.name,
      request_dto.content,
      request_dto.mime_type);

  const auto response_dto = dto::files::ToFileResponseDto(file);

  response.SetStatus(userver::server::http::HttpStatus::kCreated);
  return userver::formats::json::ToString(
      dto::files::FileResponseToJson(response_dto));
}
}  // namespace jwt_auth::files