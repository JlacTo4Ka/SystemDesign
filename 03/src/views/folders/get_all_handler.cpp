#include <views/folders/get_all_handler.hpp>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

#include <dto/folders/folder_dto.hpp>
#include <storage/storage_component.hpp>

namespace jwt_auth::folders {

GetAllHandler::GetAllHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(context.FindComponent<storage::StorageComponent>().GetStorage()) {}

std::string GetAllHandler::HandleRequestThrow(const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);
  response.SetStatus(userver::server::http::HttpStatus::kOk);

  auto folders = m_storage.GetAllFolders();

  std::vector<dto::folders::FolderResponseDto> response_dtos;
  response_dtos.reserve(folders.size());

  for (const auto& folder : folders) {
    response_dtos.push_back(dto::folders::ToFolderResponseDto(folder));
  }

  return userver::formats::json::ToString(
      dto::folders::FoldersResponseToJson(response_dtos));
}

}  // namespace jwt_auth::folders