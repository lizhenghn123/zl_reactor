#include "zlreactor/net/http/HttpKeyValue.h"
NAMESPACE_ZL_NET_START

HttpKeyValue::HttpKeyValue()
{
    initialise();
}

std::string HttpKeyValue::getStatusDesc(HttpStatusCode code) const
{
    auto iter = code_desc_.find(code);
    return iter != code_desc_.end() ? iter->second : "";
}

std::string HttpKeyValue::getContentType(const std::string& file_type) const
{
    auto iter = content_type_.find(file_type);
    return iter != content_type_.end() ? iter->second : "";
}

std::string HttpKeyValue::getMethodStr(HttpMethod method) const
{
    auto iter = method_str_.find(method);
    return iter != method_str_.end() ? iter->second : "Invalid";
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

    method_str_[HttpHead]       = ZL_HTTPMETHOD_HEAD_S;
    method_str_[HttpGet]        = ZL_HTTPMETHOD_GET_S;
    method_str_[HttpPost]       = ZL_HTTPMETHOD_POST_S;
    method_str_[HttpPut]        = ZL_HTTPMETHOD_PUT_S;
    method_str_[HttpDelete]     = ZL_HTTPMETHOD_DELETE_S;
    method_str_[HttpTrace]      = ZL_HTTPMETHOD_TRACE_S;
    method_str_[HttpOptions]    = ZL_HTTPMETHOD_OPTIONS_S;
    method_str_[HttpConnect]    = ZL_HTTPMETHOD_OPTIONS_S;
    method_str_[HttpPatch]      = ZL_HTTPMETHOD_PATCH_S;

    /* 1xx Infomational */
    code_desc_[HttpStatusContinue]                     = ZL_HTTP_STATUS_CONTINUE_S;
    code_desc_[HttpStatusSwichingProtocols]            = ZL_HTTP_STATUS_SWITCHING_PROTOCOLS_S;

    /* 2xx Succesful */
    code_desc_[HttpStatusOk]                           = ZL_HTTP_STATUS_OK_S;
    code_desc_[HttpStatsuCreated]                      = ZL_HTTP_STATUS_CREATED_S;
    code_desc_[HttpStatusAccepted]                     = ZL_HTTP_STATUS_ACCEPTED_S;
    code_desc_[HttpStatusNonAuthorizedInformation]     = ZL_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION_S;
    code_desc_[HttpStatusNoContent]                    = ZL_HTTP_STATUS_NO_CONTENT_S;
    code_desc_[HttpStatusResetContent]                 = ZL_HTTP_STATUS_RESET_CONTENT_S;
    code_desc_[HttpStatusPartialContent]               = ZL_HTTP_STATUS_PARTIAL_CONTENT_S;

    /* 3xx Redirection */
    code_desc_[HttpStatusMultipleChoices]              = ZL_HTTP_STATUS_MULTIPLE_CHOICES_S;
    code_desc_[HttpStatusMovedPermanetly]              = ZL_HTTP_STATUS_MOVED_PERMANENTLY_S;
    code_desc_[HttpStatusFound]                        = ZL_HTTP_STATUS_FOUND_S;
    code_desc_[HttpStatusSeeOther]                     = ZL_HTTP_STATUS_SEE_OTHER_S;
    code_desc_[HttpStatusNotModified]                  = ZL_HTTP_STATUS_NOT_MODIFIED_S;
    code_desc_[HttpStatusUseProxy]                     = ZL_HTTP_STATUS_USE_PROXY_S;
    code_desc_[HttpStatusTemporaryRedirection]         = ZL_HTTP_STATUS_TEMPORARY_REDIRECT_S;

    /* 4xx Client Error */
    code_desc_[HttpStatusBadRequest]                   = ZL_HTTP_STATUS_BAD_REQUEST_S;
    code_desc_[HttpStatusUnauthorized]                 = ZL_HTTP_STATUS_UNAUTHORIZED_S;
    code_desc_[HttpStatusPaymentRequired]              = ZL_HTTP_STATUS_PAYMENT_REQUIRED_S;
    code_desc_[HttpStatusForbidden]                    = ZL_HTTP_STATUS_FORBIDDEN_S;
    code_desc_[HttpStatusNotFound]                     = ZL_HTTP_STATUS_NOT_FOUND_S;
    code_desc_[HttpStatusMethodNotAllowed]             = ZL_HTTP_STATUS_METHOD_NOT_ALLOWED_S;
    code_desc_[HttpStatusNotAcceptable]                = ZL_HTTP_STATUS_NOT_ACCEPTABLE_S;
    code_desc_[HttpStatusProxyAuthenticationRequired]  = ZL_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED_S;
    code_desc_[HttpStatusRequestTimeOut]               = ZL_HTTP_STATUS_REQUEST_TIME_OUT_S;
    code_desc_[HttpStatusConflict]                     = ZL_HTTP_STATUS_CONFLICT_S;
    code_desc_[HttpStatusGone]                         = ZL_HTTP_STATUS_GONE_S;
    code_desc_[HttpStatusLengthRequired]               = ZL_HTTP_STATUS_LENGTH_REQUIRED_S;
    code_desc_[HttpStatusProconditionFailed]           = ZL_HTTP_STATUS_PRECONDITION_FAILED_S;
    code_desc_[HttpStatusRequestEntityTooLarge]        = ZL_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE_S;
    code_desc_[HttpStatusRequestURITooLarge]           = ZL_HTTP_STATUS_REQUEST_URI_TOO_LARGE_S;
    code_desc_[HttpStatusUnsupportedMediaType]         = ZL_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE_S;
    code_desc_[HttpStatusRequestedRangeNotSatisfiable] = ZL_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE_S;
    code_desc_[HttpStatusExpectationFailed]            = ZL_HTTP_STATUS_EXPECTATION_FAILED_S;

    /* 5xx Server Error */
    code_desc_[HttpStatusInternalServerError]          = ZL_HTTP_STATUS_INTERNAL_SERVER_ERROR_S;
    code_desc_[HttpStatusNotImplemented]               = ZL_HTTP_STATUS_NOT_IMPLEMENTED_S;
    code_desc_[HttpStatusBadGateway]                   = ZL_HTTP_STATUS_BAD_GATEWAY_S;
    code_desc_[HttpStatusServiceUnavaliable]           = ZL_HTTP_STATUS_SERVICE_UNAVAILABLE_S;
    code_desc_[HttpStatusGatewayTimeOut]               = ZL_HTTP_STATUS_GATEWAY_TIME_OUT_S;
    code_desc_[HttpStatusHttpVersionNotSupported]      = ZL_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED_S;
}

NAMESPACE_ZL_NET_END
