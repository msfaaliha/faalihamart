#include "GlobalExceptionHandler.h"
#include "../dto/ApiResponse.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::exception {

FormattedError GlobalExceptionHandler::HandleException(const std::exception& ex, const std::string& request_id) {
    if (const auto* api_ex = dynamic_cast<const ApiException*>(&ex)) {
        spdlog::warn("[{}] Handled API exception: code={}, status={}, msg={}",
                     request_id, api_ex->GetCode(), api_ex->GetHttpStatus(), api_ex->GetApiMessage());

        return FormattedError{
            api_ex->GetHttpStatus(),
            dto::ApiResponse::Error(api_ex->GetCode(), api_ex->GetApiMessage(), api_ex->GetDetails())
        };
    }

    spdlog::error("[{}] Unhandled std::exception: {}", request_id, ex.what());
    return FormattedError{
        500,
        dto::ApiResponse::Error("INTERNAL_ERROR", "An unexpected internal server error occurred.")
    };
}

FormattedError GlobalExceptionHandler::HandleUnknownException(const std::string& request_id) {
    spdlog::error("[{}] Unknown non-std exception caught!", request_id);
    return FormattedError{
        500,
        dto::ApiResponse::Error("INTERNAL_ERROR", "An internal system error occurred.")
    };
}

} // namespace faaliha::faalihamart::exception
