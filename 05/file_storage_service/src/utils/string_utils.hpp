#pragma once

#include <string>

namespace jwt_auth::utils {
class StringUtils final {
public:
static std::string ToLowerCopy(const std::string& value);
};
}  // namespace jwt_auth::utils