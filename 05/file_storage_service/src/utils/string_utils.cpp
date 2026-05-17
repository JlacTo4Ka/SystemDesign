#include <utils/string_utils.hpp>

#include <algorithm>
#include <cctype>

namespace jwt_auth::utils {
  std::string StringUtils::ToLowerCopy(const std::string& value) {
    std::string result = value;
    
    for (size_t i = 0; i < result.length(); i++) {
      result[i] = (char)tolower((unsigned char)result[i]);
    }
    
    return result;
  }
}  // namespace jwt_auth::utils