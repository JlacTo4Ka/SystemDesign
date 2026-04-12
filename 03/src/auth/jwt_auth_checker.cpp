#include "jwt_auth_checker.hpp"

#include <jwt-cpp/jwt.h>

#include <userver/http/common_headers.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace auth::jwt {

namespace {
static constexpr std::string_view kSecret = "secret";
static constexpr std::string_view kAlgorithm = "Bearer ";
static constexpr const char* kServiceName = "sample";
}  // namespace

JwtChecker::JwtChecker(const std::string& secret) : secret_(secret) {}

const std::string& JwtChecker::GetSecret() const { return secret_; }

JwtChecker::AuthCheckResult JwtChecker::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
  const std::string_view auth_header =
      request.GetHeader(userver::http::headers::kAuthorization);

  if (auth_header.empty()) {
    return AuthCheckResult{AuthCheckResult::Status::kTokenNotFound,
                           "Missing Authorization header"};
  }

  if (!auth_header.starts_with(kAlgorithm)) {
    return AuthCheckResult{AuthCheckResult::Status::kInvalidToken,
                           "Invalid authorization type"};
  }

  const std::string token{auth_header.substr(kAlgorithm.length())};

  try {
    auto decoded = ::jwt::decode(token);

    auto verifier = ::jwt::verify()
                        .allow_algorithm(::jwt::algorithm::hs256{secret_})
                        .with_issuer(kServiceName);

    verifier.verify(decoded);

    if (decoded.has_payload_claim("login")) {
      context.SetData("login", decoded.get_payload_claim("login").as_string());
    }

    if (decoded.has_payload_claim("user_id")) {
      context.SetData("user_id", decoded.get_payload_claim("user_id").as_string());
    }

    return {};
  } catch (const ::jwt::error::token_verification_exception& exc) {
    return AuthCheckResult{
        AuthCheckResult::Status::kInvalidToken,
        "Token verification failed: " + std::string{exc.what()}};
  } catch (const std::exception& exc) {
    return AuthCheckResult{
        AuthCheckResult::Status::kForbidden,
        "Token processing error: " + std::string{exc.what()}};
  }
}

JwtAuthComponent::JwtAuthComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context) {
  authorizer_ = std::make_shared<JwtChecker>(config[kSecret].As<std::string>());
}

JwtCheckerPtr JwtAuthComponent::Get() const { return authorizer_; }

const std::string& JwtAuthComponent::GetSecret() const {
  return authorizer_->GetSecret();
}

userver::yaml_config::Schema JwtAuthComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<LoggableComponentBase>(R"(
type: object
description: JWT Auth Checker Component
additionalProperties: false
properties:
    secret:
        type: string
        description: secret key for JWT validation
)");
}

}  // namespace auth::jwt