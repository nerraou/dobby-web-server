#pragma once

// 1xx informational response


// 2xx success

#define _200 200
#define _200_MESSAGE "OK"

// 3xx redirection


// 4xx client errors

#define _400 400
#define _400_MESSAGE "Bad Request"

#define _403 403
#define _403_MESSAGE "Forbidden"

#define _404 404
#define _404_MESSAGE "Not Found"

#define _408 408
#define _408_MESSAGE "Request Timeout"

#define _413 413
#define _413_MESSAGE "Payload Too Large"


// 5xx server errors

#define _500 500
#define _500_MESSAGE "Internal Server Error"

#define _505 505
#define _505_MESSAGE "HTTP Version Not Supported"