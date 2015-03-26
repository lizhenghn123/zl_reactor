// ***********************************************************************
// Filename         : HttpProtocol.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : Http协议定义
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-01
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPPROTOCOL_H
#define ZL_HTTPPROTOCOL_H
#include "Define.h"
NAMESPACE_ZL_NET_START

#define CRLF                  "\r\n"
#define DOUBLE_CRLF           "\r\n\r\n"

enum HttpVersion
{
    HTTP_VERSION_0_0,
    HTTP_VERSION_1_0,
    HTTP_VERSION_1_1
};

#define HTTP_VERSION_0_0_S      "unknown"
#define HTTP_VERSION_1_0_S      "HTTP/1.0"
#define HTTP_VERSION_1_1_S      "HTTP/1.1"

enum HttpMethod
{
    HttpInvalid = 0,
    HttpHead    = 1,
    HttpGet     = 2,
    HttpPost    = 3,
    HttpPut     = 4,
    HttpDelete  = 5,
    HttpTrace   = 6,
    HttpOptions = 7,
    HttpConnect = 8,
    HttpPatch   = 9,
    HttpMethodNum = 10
};

#define ZL_HTTPMETHOD_HEAD_S          "HEAD"
#define ZL_HTTPMETHOD_GET_S           "GET"
#define ZL_HTTPMETHOD_POST_S          "POST"
#define ZL_HTTPMETHOD_PUT_S           "PUT"
#define ZL_HTTPMETHOD_DELETE_S        "DELETE"
#define ZL_HTTPMETHOD_TRACE_S         "TRACE"
#define ZL_HTTPMETHOD_OPTIONS_S       "OPTIONS"
#define ZL_HTTPMETHOD_CONNECT_S       "CONNECT"
#define ZL_HTTPMETHOD_PATCH_S         "PATCH"

enum HttpHeader
{
    HttpHeaderUserAgent = 0,
    HttpHeaderAccept,
    HttpHeaderAcceptLanguage,
    HttpHeaderAcceptEncoding,
    HttpHeaderAcceptCharset,
    HttpHeaderProxyConnection,
    HttpHeaderConnection,
    HttpHeaderCookie,
    HttpHeaderHost,
    HttpHeaderCacheControl,
    HttpHeaderContentType,
    HttpHeaderContentLength,
    HttpHeaderContentEncoding,
    HttpHeaderServer,
    HttpHeaderDate,
    HttpHeaderRange,
    HttpHeaderEtag,
    HttpHeaderExpires,
    HttpHeaderPererer,
    HttpHeaderLastModified,
    HttpHeaderIfModifiedSince,
    HttpHeaderIfNoneMatch,
    HttpHeaderAcceptRanges,
    HttpHeaderTransferEncoding,
    HttpHeaderAuthorzation
};

//http header
#define ZL_HTTP_HEADER_USER_AGENT_S                     "User-Agent:"
#define ZL_HTTP_HEADER_ACCEPT_S                         "Accept:"
#define ZL_HTTP_HEADER_ACCEPT_LANGUAGE_S                "Accept-Language:"
#define ZL_HTTP_HEADER_ACCEPT_ENCODING_S                "Accept-Encoding:"
#define ZL_HTTP_HEADER_ACCEPT_CHARSET_S                 "Accept-Charset:"
#define ZL_HTTP_HEADER_PROXY_CONNECTION_S               "Proxy-Connection:"
#define ZL_HTTP_HEADER_CONNECTION_S                     "Connection:"
#define ZL_HTTP_HEADER_COOKIE_S                         "Cookie:"
#define ZL_HTTP_HEADER_HOST_S                           "Host:"
#define ZL_HTTP_HEADER_CACHE_CONTROL_S                  "Cache-Control:"
#define ZL_HTTP_HEADER_CONTENT_TYPE_S                   "Content-Type:"
#define ZL_HTTP_HEADER_CONTENT_LENGTH_S                 "Content-Length:"
#define ZL_HTTP_HEADER_CONTENT_ENCODING_S               "Content-Encoding:"
#define ZL_HTTP_HEADER_SERVER_S                         "Server:"
#define ZL_HTTP_HEADER_DATE_S                           "Date:"
#define ZL_HTTP_HEADER_RANGE_S                          "Range:"
#define ZL_HTTP_HEADER_ETAG_S                           "Etag:"
#define ZL_HTTP_HEADER_EXPIRES_S                        "Expires:"
#define ZL_HTTP_HEADER_REFERER_S                        "Referer:"
#define ZL_HTTP_HEADER_LAST_MODIFIED_S                  "Last-Modified:"
#define ZL_HTTP_HEADER_IF_MOD_SINCE_S                   "If-Modified-Since:"
#define ZL_HTTP_HEADER_IF_NONE_MATCH_S                  "If-None-Match:"
#define ZL_HTTP_HEADER_ACCEPT_RANGES_S                  "Accept-Ranges:"
#define ZL_HTTP_HEADER_TRANSFER_ENCODING_S              "Transfer-Encoding:"
#define ZL_HTTP_HEADER_AUTHORIZATION_S                  "Authorization:"

enum HttpStatusCode
{
    /* 1xx Infomational */
    HttpStatusContinue                                  = 100,
    HttpStatusSwichingProtocols                         = 101,

    /* 2xx Succesful */
    HttpStatusOk                                        = 200,
    HttpStatsuCreated                                   = 201,
    HttpStatusAccepted                                  = 202,
    HttpStatusNonAuthorizedInformation                  = 203,
    HttpStatusNoContent                                 = 204,
    HttpStatusResetContent                              = 205,
    HttpStatusPartialContent                            = 206,

    /* 3xx Redirection */
    HttpStatusMultipleChoices                           = 300,
    HttpStatusMovedPermanetly                           = 301,
    HttpStatusFound                                     = 302,
    HttpStatusSeeOther                                  = 303,
    HttpStatusNotModified                               = 304,
    HttpStatusUseProxy                                  = 305,
    HttpStatusTemporaryRedirection                      = 307,

    /* 4xx Client Error */
    HttpStatusBadRequest                                = 400,
    HttpStatusUnauthorized                              = 401,
    HttpStatusPaymentRequired                           = 402,
    HttpStatusForbidden                                 = 403,
    HttpStatusNotFound                                  = 404,
    HttpStatusMethodNotAllowed                          = 405,
    HttpStatusNotAcceptable                             = 406,
    HttpStatusProxyAuthenticationRequired               = 407,
    HttpStatusRequestTimeOut                            = 408,
    HttpStatusConflict                                  = 409,
    HttpStatusGone                                      = 410,
    HttpStatusLengthRequired                            = 411,
    HttpStatusProconditionFailed                        = 412,
    HttpStatusRequestEntityTooLarge                     = 413,
    HttpStatusRequestURITooLarge                        = 414,
    HttpStatusUnsupportedMediaType                      = 415,
    HttpStatusRequestedRangeNotSatisfiable              = 416,
    HttpStatusExpectationFailed                         = 417,

    /* 5xx Server Error */
    HttpStatusInternalServerError                       = 500,
    HttpStatusNotImplemented                            = 501,
    HttpStatusBadGateway                                = 502,
    HttpStatusServiceUnavaliable                        = 503,
    HttpStatusGatewayTimeOut                            = 504,
    HttpStatusHttpVersionNotSupported                   = 505
};

//http response status
#define ZL_HTTP_STATUS_CONTINUE_S                       "Continue"      //"100 Continue"
#define ZL_HTTP_STATUS_SWITCHING_PROTOCOLS_S            "Switching Protocols"
#define ZL_HTTP_STATUS_OK_S                             "OK"            //"200 OK"
#define ZL_HTTP_STATUS_CREATED_S                        "Created "
#define ZL_HTTP_STATUS_ACCEPTED_S                       "Accepted"
#define ZL_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION_S  "Non-Authoritative Information"
#define ZL_HTTP_STATUS_NO_CONTENT_S                     "No Content"
#define ZL_HTTP_STATUS_RESET_CONTENT_S                  "Reset Content"
#define ZL_HTTP_STATUS_PARTIAL_CONTENT_S                "Partial Content"
#define ZL_HTTP_STATUS_MULTIPLE_CHOICES_S               "Multiple Choices"
#define ZL_HTTP_STATUS_MOVED_PERMANENTLY_S              "Moved Permanently"
#define ZL_HTTP_STATUS_FOUND_S                          "Found"
#define ZL_HTTP_STATUS_SEE_OTHER_S                      "See Other"
#define ZL_HTTP_STATUS_NOT_MODIFIED_S                   "Not Modified"
#define ZL_HTTP_STATUS_USE_PROXY_S                      "Use Proxy"
#define ZL_HTTP_STATUS_TEMPORARY_REDIRECT_S             "Temporary Redirect"
#define ZL_HTTP_STATUS_BAD_REQUEST_S                    "Bad Request"
#define ZL_HTTP_STATUS_UNAUTHORIZED_S                   "Unauthorized"
#define ZL_HTTP_STATUS_PAYMENT_REQUIRED_S               "Payment Required"
#define ZL_HTTP_STATUS_FORBIDDEN_S                      "Forbidden"
#define ZL_HTTP_STATUS_NOT_FOUND_S                      "Not Found"
#define ZL_HTTP_STATUS_METHOD_NOT_ALLOWED_S             "Not Allowed"
#define ZL_HTTP_STATUS_NOT_ACCEPTABLE_S                 "Not Acceptable"
#define ZL_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED_S   "Authenication Required"
#define ZL_HTTP_STATUS_REQUEST_TIME_OUT_S               "Request Time-out"
#define ZL_HTTP_STATUS_CONFLICT_S                       "Conflict"
#define ZL_HTTP_STATUS_GONE_S                           "Gone"
#define ZL_HTTP_STATUS_LENGTH_REQUIRED_S                "Length Required"
#define ZL_HTTP_STATUS_PRECONDITION_FAILED_S            "Precondition Required"
#define ZL_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE_S       "Request Enity Too Large"
#define ZL_HTTP_STATUS_REQUEST_URI_TOO_LARGE_S          "Request-URI Too Large"
#define ZL_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE_S         "Unsupported Media Type"
#define ZL_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE_S  "Requested range not satisfiable"
#define ZL_HTTP_STATUS_EXPECTATION_FAILED_S             "Expectation Failed"
#define ZL_HTTP_STATUS_INTERNAL_SERVER_ERROR_S          "Internal Server Error"
#define ZL_HTTP_STATUS_NOT_IMPLEMENTED_S                "Not Implemented"
#define ZL_HTTP_STATUS_BAD_GATEWAY_S                    "Bad Gateway"
#define ZL_HTTP_STATUS_SERVICE_UNAVAILABLE_S            "Service Unavailable"
#define ZL_HTTP_STATUS_GATEWAY_TIME_OUT_S               "Gateway Time-out"
#define ZL_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED_S     "HTTP Version not supported"

NAMESPACE_ZL_NET_END
#endif /* ZL_HTTPPROTOCOL_H */