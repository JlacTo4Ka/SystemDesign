#include <views/files/get_by_name_handler.hpp>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <dto/files/file_dto.hpp>
#include <storage/storage_component.hpp>

namespace jwt_auth::files {

GetByNameHandler::GetByNameHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string GetByNameHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  const auto name = request.GetPathArg("name");

  userver::formats::json::ValueBuilder error;

  if (name.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "name is required";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto file = m_storage.GetFileByName(name);
  if (!file.has_value()) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    error["error"] = "file not found";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto response_dto = dto::files::ToFileResponseDto(*file);

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(
      dto::files::FileResponseToJson(response_dto));
}

}  // namespace jwt_auth::files