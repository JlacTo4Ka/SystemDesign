#include <views/mongo_folders/create_handler.hpp>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <dto/folders/folder_dto.hpp>
#include <storage/mongo_storage_component.hpp>
#include <storage/storage_component.hpp>

namespace jwt_auth::mongo_folders {

CreateHandler::CreateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_mongo_storage(
          context.FindComponent<storage::MongoStorageComponent>().GetStorage()),
      m_postgres_storage(
          context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string CreateHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  userver::formats::json::ValueBuilder error;

  userver::formats::json::Value json;
  try {
    json = userver::formats::json::FromString(request.RequestBody());
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "invalid json body";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto request_dto = dto::folders::CreateFolderRequestFromJson(json);

  if (request_dto.name.empty() || request_dto.owner_user_id <= 0) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "name and owner_user_id are required";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto user = m_postgres_storage.GetUserById(request_dto.owner_user_id);
  if (!user.has_value()) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    error["error"] = "owner user not found";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto folder =
      m_mongo_storage.CreateFolder(request_dto.owner_user_id, request_dto.name);

  response.SetStatus(userver::server::http::HttpStatus::kCreated);
  return userver::formats::json::ToString(
      dto::folders::FolderResponseToJson(dto::folders::ToFolderResponseDto(folder)));
}

}  // namespace jwt_auth::mongo_folders