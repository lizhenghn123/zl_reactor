#include "HttpKeyValue.h"
#include "HttpProtocol.h"
NAMESPACE_ZL_NET_START

HttpKeyValue::HttpKeyValue()
{
    initialise();
}

std::string HttpKeyValue::getReasonDesc(int code)
{
    auto iter = code_desc_.find(code);
    return iter != code_desc_.end() ? iter->second : "";
}

std::string HttpKeyValue::getContentType(const std::string& file_type)
{
    auto iter = content_type_.find(file_type);
    return iter != content_type_.end() ? iter->second : "";
}

void HttpKeyValue::initialise()
{ 
    content_type_["htm"]  = "text/html";
    content_type_["html"] = "text/html";
    content_type_["css"]  = "text/css";
    content_type_["txt"]  = "text/plain";
    content_type_["c"]    = "text/plain";
    content_type_["cpp"]  = "text/plain";
    content_type_["cxx"]  = "text/plain";
    content_type_["h"]    = "text/plain";
    content_type_["jpeg"] = "image/jpeg";
    content_type_["jpg"]  = "image/jpeg";
    content_type_["png"]  = "image/png";
    content_type_["bmp"]  = "image/bmp";
    content_type_["gif"]  = "image/gif";
    content_type_["ico"]  = "image/x-icon";
    content_type_["mpg"]  = "video/mpeg";
    content_type_["asf"]  = "video/x-ms-asf";
    content_type_["avi"]  = "video/x-msvideo";
    content_type_["doc"]  = "application/msword";
    content_type_["exe"]  = "application/octet-stream";
    content_type_["rar"]  = "application/octet-stream";
    content_type_["zip"]  = "application/zip";
    content_type_["*"]    = "application/octet-stream";


    /* 1xx Infomational */
    code_desc_[HttpStatusCode::HttpStatusContinue]                     = ZL_HTTP_STATUS_CONTINUE_S;
    code_desc_[HttpStatusCode::HttpStatusSwichingProtocols]            = ZL_HTTP_STATUS_SWITCHING_PROTOCOLS_S;

    /* 2xx Succesful */
    code_desc_[HttpStatusCode::HttpStatusOk]                           = ZL_HTTP_STATUS_OK_S;
    code_desc_[HttpStatusCode::HttpStatsuCreated]                      = ZL_HTTP_STATUS_CREATED_S;
    code_desc_[HttpStatusCode::HttpStatusAccepted]                     = ZL_HTTP_STATUS_ACCEPTED_S;
    code_desc_[HttpStatusCode::HttpStatusNonAuthorizedInformation]     = ZL_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION_S;
    code_desc_[HttpStatusCode::HttpStatusNoContent]                    = ZL_HTTP_STATUS_NO_CONTENT_S;
    code_desc_[HttpStatusCode::HttpStatusResetContent]                 = ZL_HTTP_STATUS_RESET_CONTENT_S;
    code_desc_[HttpStatusCode::HttpStatusPartialContent]               = ZL_HTTP_STATUS_PARTIAL_CONTENT_S;

    /* 3xx Redirection */
    code_desc_[HttpStatusCode::HttpStatusMultipleChoices]              = ZL_HTTP_STATUS_MULTIPLE_CHOICES_S;
    code_desc_[HttpStatusCode::HttpStatusMovedPermanetly]              = ZL_HTTP_STATUS_MOVED_PERMANENTLY_S;
    code_desc_[HttpStatusCode::HttpStatusFound]                        = ZL_HTTP_STATUS_FOUND_S;
    code_desc_[HttpStatusCode::HttpStatusSeeOther]                     = ZL_HTTP_STATUS_SEE_OTHER_S;
    code_desc_[HttpStatusCode::HttpStatusNotModified]                  = ZL_HTTP_STATUS_NOT_MODIFIED_S;
    code_desc_[HttpStatusCode::HttpStatusUseProxy]                     = ZL_HTTP_STATUS_USE_PROXY_S;
    code_desc_[HttpStatusCode::HttpStatusTemporaryRedirection]         = ZL_HTTP_STATUS_TEMPORARY_REDIRECT_S;

    /* 4xx Client Error */
    code_desc_[HttpStatusCode::HttpStatusBadRequest]                   = ZL_HTTP_STATUS_BAD_REQUEST_S;
    code_desc_[HttpStatusCode::HttpStatusUnauthorized]                 = ZL_HTTP_STATUS_UNAUTHORIZED_S;
    code_desc_[HttpStatusCode::HttpStatusPaymentRequired]              = ZL_HTTP_STATUS_PAYMENT_REQUIRED_S;
    code_desc_[HttpStatusCode::HttpStatusForbidden]                    = ZL_HTTP_STATUS_FORBIDDEN_S;
    code_desc_[HttpStatusCode::HttpStatusNotFound]                     = ZL_HTTP_STATUS_NOT_FOUND_S;
    code_desc_[HttpStatusCode::HttpStatusMethodNotAllowed]             = ZL_HTTP_STATUS_METHOD_NOT_ALLOWED_S;
    code_desc_[HttpStatusCode::HttpStatusNotAcceptable]                = ZL_HTTP_STATUS_NOT_ACCEPTABLE_S;
    code_desc_[HttpStatusCode::HttpStatusProxyAuthenticationRequired]  = ZL_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED_S;
    code_desc_[HttpStatusCode::HttpStatusRequestTimeOut]               = ZL_HTTP_STATUS_REQUEST_TIME_OUT_S;
    code_desc_[HttpStatusCode::HttpStatusConflict]                     = ZL_HTTP_STATUS_CONFLICT_S;
    code_desc_[HttpStatusCode::HttpStatusGone]                         = ZL_HTTP_STATUS_GONE_S;
    code_desc_[HttpStatusCode::HttpStatusLengthRequired]               = ZL_HTTP_STATUS_LENGTH_REQUIRED_S;
    code_desc_[HttpStatusCode::HttpStatusProconditionFailed]           = ZL_HTTP_STATUS_PRECONDITION_FAILED_S;
    code_desc_[HttpStatusCode::HttpStatusRequestEntityTooLarge]        = ZL_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE_S;
    code_desc_[HttpStatusCode::HttpStatusRequestURITooLarge]           = ZL_HTTP_STATUS_REQUEST_URI_TOO_LARGE_S;
    code_desc_[HttpStatusCode::HttpStatusUnsupportedMediaType]         = ZL_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE_S;
    code_desc_[HttpStatusCode::HttpStatusRequestedRangeNotSatisfiable] = ZL_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE_S;
    code_desc_[HttpStatusCode::HttpStatusExpectationFailed]            = ZL_HTTP_STATUS_EXPECTATION_FAILED_S;

    /* 5xx Server Error */
    code_desc_[HttpStatusCode::HttpStatusInternalServerError]          = ZL_HTTP_STATUS_INTERNAL_SERVER_ERROR_S;
    code_desc_[HttpStatusCode::HttpStatusNotImplemented]               = ZL_HTTP_STATUS_NOT_IMPLEMENTED_S;
    code_desc_[HttpStatusCode::HttpStatusBadGateway]                   = ZL_HTTP_STATUS_BAD_GATEWAY_S;
    code_desc_[HttpStatusCode::HttpStatusServiceUnavaliable]           = ZL_HTTP_STATUS_SERVICE_UNAVAILABLE_S;
    code_desc_[HttpStatusCode::HttpStatusGatewayTimeOut]               = ZL_HTTP_STATUS_GATEWAY_TIME_OUT_S;
    code_desc_[HttpStatusCode::HttpStatusHttpVersionNotSupported]      = ZL_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED_S;
}

NAMESPACE_ZL_NET_END
