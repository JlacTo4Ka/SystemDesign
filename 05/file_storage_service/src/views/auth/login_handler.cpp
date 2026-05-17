#include <views/auth/login_handler.hpp>

#include <algorithm>
#include <chrono>
#include <string>

#include <jwt-cpp/jwt.h>

#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/redis/component.hpp>

#include <auth/jwt_auth_checker.hpp>
#include <dto/auth/auth_dto.hpp>
#include <storage/storage_component.hpp>

namespace jwt_auth::auth_handlers {
namespace {

constexpr int kLoginRateLimit = 10;
constexpr auto kLoginRateLimitTtl = std::chrono::seconds{60};

std::string GetClientIp(const userver::server::http::HttpRequest& request) {
  const auto x_real_ip = request.GetHeader("X-Real-IP");
  if (!x_real_ip.empty()) {
    return x_real_ip;
  }

  const auto x_forwarded_for = request.GetHeader("X-Forwarded-For");
  if (!x_forwarded_for.empty()) {
    return x_forwarded_for;
  }

  return "local-client";
}

std::string MakeRateLimitKey(const std::string& client_ip) {
  return "rate_limit:login:" + client_ip;
}

}  // namespace

LoginHandler::LoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      m_storage(context.FindComponent<storage::StorageComponent>().GetStorage()),
      redis_client_(
          context.FindComponent<userver::components::Redis>("redis-cache")
              .GetClient("cache-db")),
      redis_cc_(std::chrono::seconds{1}, std::chrono::seconds{3}, 2),
      m_secret(context.FindComponent<auth::jwt::JwtAuthComponent>().GetSecret()) {}

std::string LoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType(userver::http::content_type::kApplicationJson);

  userver::formats::json::ValueBuilder error;

  try {
    const auto client_ip = GetClientIp(request);
    const auto rate_limit_key = MakeRateLimitKey(client_ip);

    int current_requests = 0;

    const auto cached_counter = redis_client_->Get(rate_limit_key, redis_cc_).Get();
    if (cached_counter.has_value()) {
      try {
        current_requests = std::stoi(*cached_counter);
      } catch (const std::exception&) {
        current_requests = 0;
      }
    }

    current_requests += 1;

    redis_client_->Set(
        rate_limit_key,
        std::to_string(current_requests),
        kLoginRateLimitTtl,
        redis_cc_).Get();

    const auto remaining_requests =
        std::max(0, kLoginRateLimit - current_requests);

    const std::string limit_header = "X-RateLimit-Limit";
    const std::string remaining_header = "X-RateLimit-Remaining";
    const std::string reset_header = "X-RateLimit-Reset";

    response.SetHeader(limit_header, std::to_string(kLoginRateLimit));
    response.SetHeader(remaining_header, std::to_string(remaining_requests));
    response.SetHeader(reset_header, std::to_string(kLoginRateLimitTtl.count()));

    if (current_requests > kLoginRateLimit) {
      response.SetStatus(userver::server::http::HttpStatus::kTooManyRequests);
      error["error"] = "too many login requests";
      return userver::formats::json::ToString(error.ExtractValue());
    }
  } catch (const std::exception&) {
    // Любую другую ошибку Redis тоже не даём превратить в 500 для login
  }

  userver::formats::json::Value json;
  try {
    json = userver::formats::json::FromString(request.RequestBody());
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "invalid json body";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto request_dto = dto::auth::LoginRequestFromJson(json);

  if (request_dto.login.empty() || request_dto.password.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    error["error"] = "login and password are required";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto user =
      m_storage.GetUserByLoginAndPassword(request_dto.login, request_dto.password);

  if (!user.has_value()) {
    response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
    error["error"] = "invalid login or password";
    return userver::formats::json::ToString(error.ExtractValue());
  }

  const auto token = ::jwt::create()
                         .set_issuer("sample")
                         .set_type("JWT")
                         .set_payload_claim("login", ::jwt::claim(user->login))
                         .set_payload_claim("user_id", ::jwt::claim(std::to_string(user->id)))
                         .sign(::jwt::algorithm::hs256{m_secret});

  dto::auth::LoginResponseDto response_dto;
  response_dto.token = token;
  response_dto.user_id = user->id;
  response_dto.login = user->login;

  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::ToString(
      dto::auth::LoginResponseToJson(response_dto));
}

}  // namespace jwt_auth::auth_handlers