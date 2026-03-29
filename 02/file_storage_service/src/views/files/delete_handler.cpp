#include <views/files/delete_handler.hpp>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <storage/storage_component.hpp>

namespace jwt_auth::files {

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

  const auto file_id_str = request.GetPathArg("file_id");

  userver::formats::json::ValueBuilder builder;

  if (file_id_str.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    builder["error"] = "file_id is required";
    return userver::formats::json::ToString(builder.ExtractValue());
  }

  int file_id = 0;
  try {
    file_id = std::stoi(file_id_str);
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    builder["error"] = "file_id must be integer";
    return userver::formats::json::ToString(builder.ExtractValue());
  }

  if (!m_storage.DeleteFile(file_id)) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    builder["error"] = "file not found";
    return userver::formats::json::ToString(builder.ExtractValue());
  }

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  builder["status"] = "file deleted";

  return userver::formats::json::ToString(builder.ExtractValue());
}

}  // namespace jwt_auth::files