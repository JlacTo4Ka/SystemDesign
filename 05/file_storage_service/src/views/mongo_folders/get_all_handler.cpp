#include <views/mongo_folders/get_all_handler.hpp>

#include <chrono>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/redis/component.hpp>

#include <dto/folders/folder_dto.hpp>
#include <storage/mongo_storage_component.hpp>
#include <userver/storages/redis/component.hpp>

namespace jwt_auth::mongo_folders {

namespace {
constexpr auto kFoldersCacheTtl = std::chrono::minutes{1};
const std::string kCacheKey = "cache:folders:mongo:all";
}  // namespace

GetAllHandler::GetAllHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(context.FindComponent<storage::MongoStorageComponent>().GetStorage()),
      redis_client_(
          context.FindComponent<userver::components::Redis>("redis-cache")
              .GetClient("cache-db")),
      redis_cc_(std::chrono::seconds{1}, std::chrono::seconds{3}, 2) {}

std::string GetAllHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  const auto cached = redis_client_->Get(kCacheKey, redis_cc_).Get();
  if (cached) {
    response.SetStatus(userver::server::http::HttpStatus::kOk);
    return *cached;
  }

  const auto folders = m_storage.GetAllFolders();

  std::vector<dto::folders::FolderResponseDto> response_dtos;
  response_dtos.reserve(folders.size());

  for (const auto& folder : folders) {
    response_dtos.push_back(dto::folders::ToFolderResponseDto(folder));
  }

  const auto json = dto::folders::FoldersResponseToJson(response_dtos);
  const auto result = userver::formats::json::ToStableString(json);

  redis_client_->Set(kCacheKey, result, kFoldersCacheTtl, redis_cc_).Get();

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return result;
}

}  // namespace jwt_auth::mongo_folders