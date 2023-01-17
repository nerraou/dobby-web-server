#pragma once

// 1xx informational response

// 2xx success

#define HTTP_OK 200
#define HTTP_OK_MESSAGE "OK"

// 3xx redirection

// 4xx client errors

#define HTTP_BAD_REQUEST 400
#define HTTP_BAD_REQUEST_MESSAGE "Bad Request"

#define HTTP_FORBIDDEN 403
#define HTTP_FORBIDDEN_MESSAGE "Forbidden"

#define HTTP_NOT_FOUND 404
#define HTTP_NOT_FOUND_MESSAGE "Not Found"

#define HTTP_REQUEST_TIMEOUT 408
#define HTTP_REQUEST_TIMEOUT_MESSAGE "Request Timeout"

#define HTTP_PAYLOAD_TOO_LARGE 413
#define HTTP_PAYLOAD_TOO_LARGE_MESSAGE "Payload Too Large"

// 5xx server errors

#define HTTP_INTERNAL_SERVER_ERROR 500
#define HTTP_INTERNAL_SERVER_ERROR_MESSAGE "Internal Server Error"

#define HTTP_VERSION_NOT_SUPPORTED 505
#define HTTP_VERSION_NOT_SUPPORTED_MESSAGE "HTTP Version Not Supported"
