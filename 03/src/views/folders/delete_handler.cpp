#include <views/folders/delete_handler.hpp>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <storage/storage_component.hpp>

namespace jwt_auth::folders {

DeleteHandler::DeleteHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string DeleteHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  const auto folder_id_str = request.GetPathArg("folder_id");

  userver::formats::json::ValueBuilder builder;

  if (folder_id_str.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    builder["error"] = "folder_id is required";
    return userver::formats::json::ToString(builder.ExtractValue());
  }

  int folder_id = 0;
  try {
    folder_id = std::stoi(folder_id_str);
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    builder["error"] = "folder_id must be integer";
    return userver::formats::json::ToString(builder.ExtractValue());
  }

  if (!m_storage.DeleteFolder(folder_id)) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    builder["error"] = "folder not found";
    return userver::formats::json::ToString(builder.ExtractValue());
  }

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  builder["status"] = "folder deleted";

  return userver::formats::json::ToString(builder.ExtractValue());
}

}  // namespace jwt_auth::folders