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

////http response status code
//#define ZL_HTTP_STATUS_CONTINUE                        100
//#define ZL_HTTP_STATUS_SWITCHING_PROTOCOLS             101
//#define ZL_HTTP_STATUS_OK                              200
//#define ZL_HTTP_STATUS_CREATED                         201
//#define ZL_HTTP_STATUS_ACCEPTED                        202
//#define ZL_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION   203
//#define ZL_HTTP_STATUS_NO_CONTENT                      204
//#define ZL_HTTP_STATUS_RESET_CONTENT                   205
//#define ZL_HTTP_STATUS_PARTIAL_CONTENT                 206
//#define ZL_HTTP_STATUS_MULTIPLE_CHOICES                300
//#define ZL_HTTP_STATUS_MOVED_PERMANENTLY               301
//#define ZL_HTTP_STATUS_FOUND                           302
//#define ZL_HTTP_STATUS_SEE_OTHER                       303
//#define ZL_HTTP_STATUS_NOT_MODIFIED                    304
//#define ZL_HTTP_STATUS_USE_PROXY                       305
//#define ZL_HTTP_STATUS_TEMPORARY_REDIRECT              307
//#define ZL_HTTP_STATUS_BAD_REQUEST                     400
//#define ZL_HTTP_STATUS_UNAUTHORIZED                    401
//#define ZL_HTTP_STATUS_PAYMENT_REQUIRED                402
//#define ZL_HTTP_STATUS_FORBIDDEN                       403
//#define ZL_HTTP_STATUS_NOT_FOUND                       404
//#define ZL_HTTP_STATUS_METHOD_NOT_ALLOWED              405
//#define ZL_HTTP_STATUS_NOT_ACCEPTABLE                  406
//#define ZL_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED    407
//#define ZL_HTTP_STATUS_REQUEST_TIME_OUT                408
//#define ZL_HTTP_STATUS_CONFLICT                        409
//#define ZL_HTTP_STATUS_GONE                            410
//#define ZL_HTTP_STATUS_LENGTH_REQUIRED                 411
//#define ZL_HTTP_STATUS_PRECONDITION_FAILED             412
//#define ZL_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE        413
//#define ZL_HTTP_STATUS_REQUEST_URI_TOO_LARGE           414
//#define ZL_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE          415
//#define ZL_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE   416
//#define ZL_HTTP_STATUS_EXPECTATION_FAILED              417
//#define ZL_HTTP_STATUS_INTERNAL_SERVER_ERROR           500
//#define ZL_HTTP_STATUS_NOT_IMPLEMENTED                 501
//#define ZL_HTTP_STATUS_BAD_GATEWAY                     502
//#define ZL_HTTP_STATUS_SERVICE_UNAVAILABLE             503
//#define ZL_HTTP_STATUS_GATEWAY_TIME_OUT                504
//#define ZL_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED      505

//http response status
#define ZL_HTTP_STATUS_CONTINUE_S                       "100 Continue"
#define ZL_HTTP_STATUS_SWITCHING_PROTOCOLS_S            "101 Switching Protocols"
#define ZL_HTTP_STATUS_OK_S                             "200 OK"
#define ZL_HTTP_STATUS_CREATED_S                        "201 Created"
#define ZL_HTTP_STATUS_ACCEPTED_S                       "202 Accepted"
#define ZL_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION_S  "203 Non-Authoritative Information"
#define ZL_HTTP_STATUS_NO_CONTENT_S                     "204 No Content"
#define ZL_HTTP_STATUS_RESET_CONTENT_S                  "205 Reset Content"
#define ZL_HTTP_STATUS_PARTIAL_CONTENT_S                "206 Partial Content"
#define ZL_HTTP_STATUS_MULTIPLE_CHOICES_S               "300 Multiple Choices"
#define ZL_HTTP_STATUS_MOVED_PERMANENTLY_S              "301 Moved Permanently"
#define ZL_HTTP_STATUS_FOUND_S                          "302 Found"
#define ZL_HTTP_STATUS_SEE_OTHER_S                      "303 See Other"
#define ZL_HTTP_STATUS_NOT_MODIFIED_S                   "304 Not Modified"
#define ZL_HTTP_STATUS_USE_PROXY_S                      "305 Use Proxy"
#define ZL_HTTP_STATUS_TEMPORARY_REDIRECT_S             "307 Temporary Redirect"
#define ZL_HTTP_STATUS_BAD_REQUEST_S                    "400 Bad Request"
#define ZL_HTTP_STATUS_UNAUTHORIZED_S                   "401 Unauthorized"
#define ZL_HTTP_STATUS_PAYMENT_REQUIRED_S               "402 Payment Required"
#define ZL_HTTP_STATUS_FORBIDDEN_S                      "403 Forbidden"
#define ZL_HTTP_STATUS_NOT_FOUND_S                      "404 Not Found"
#define ZL_HTTP_STATUS_METHOD_NOT_ALLOWED_S             "405 Not Allowed"
#define ZL_HTTP_STATUS_NOT_ACCEPTABLE_S                 "406 Not Acceptable"
#define ZL_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED_S   "407 Authenication Required"
#define ZL_HTTP_STATUS_REQUEST_TIME_OUT_S               "408 Request Time-out"
#define ZL_HTTP_STATUS_CONFLICT_S                       "409 Conflict"
#define ZL_HTTP_STATUS_GONE_S                           "410 Gone"
#define ZL_HTTP_STATUS_LENGTH_REQUIRED_S                "411 Length Required"
#define ZL_HTTP_STATUS_PRECONDITION_FAILED_S            "412 Precondition Required"
#define ZL_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE_S       "413 Request Enity Too Large"
#define ZL_HTTP_STATUS_REQUEST_URI_TOO_LARGE_S          "414 Request-URI Too Large"
#define ZL_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE_S         "415 Unsupported Media Type"
#define ZL_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE_S  "416 Requested range not satisfiable"
#define ZL_HTTP_STATUS_EXPECTATION_FAILED_S             "417 Expectation Failed"
#define ZL_HTTP_STATUS_INTERNAL_SERVER_ERROR_S          "500 Internal Server Error"
#define ZL_HTTP_STATUS_NOT_IMPLEMENTED_S                "501 Not Implemented"
#define ZL_HTTP_STATUS_BAD_GATEWAY_S                    "502 Bad Gateway"
#define ZL_HTTP_STATUS_SERVICE_UNAVAILABLE_S            "503 Service Unavailable"
#define ZL_HTTP_STATUS_GATEWAY_TIME_OUT_S               "504 Gateway Time-out"
#define ZL_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED_S     "505 HTTP Version not supported"

NAMESPACE_ZL_NET_END
#endif /* ZL_HTTPPROTOCOL_H */