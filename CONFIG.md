- http

  1. root
  2. index
  3. autoindex
  4. client_max_body_size
  5. error_page
  6. cgi

- server

  1. root
  2. index
  3. listen
  4. server_name
  5. error_page
  6. autoindex
  7. client_max_body_size
  8. cgi

- location

  1. root
  2. index
  3. autoindex
  4. client_max_body_size
  5. accepted_http_methods
  6. rewrite
  7. path
  8. error_page
  9. cgi

| Attribute                                                                            | Type          | Required in context | Default | Inherited | Description                                                                                                                                             |
| ------------------------------------------------------------------------------------ | ------------- | ------------------- | ------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------- |
| root[<sup>[1]</sup>](#autoindex)                                                     | string        | http                | -       | `true`    | Specifies the root directory that will be used to search for a file. syntax: `root /var/www/html`                                                       |
| autoindex[<sup>[2]</sup>](#autoindex)                                                | enum(on, off) | -                   | `off`   | `true`    | Used to enable/disable directory listing. example: `autoindex on`                                                                                       |
| client_max_body_size[<sup>[3]</sup>](#client_max_body_size)[<sup>[10]</sup>](#units) | string        | http                | `1M`    | `true`    | Sets the maximum allowed size of the client request body. syntax: `client_max_body_size 1M`                                                             |
| error_page[<sup>[4]</sup>](#error_page)                                              | string        | http                | -       | `true`    | Syntax: `error_page code ... uri`. example: `error_page 400 404 /40x.html`                                                                              |
| listen[<sup>[5]</sup>](#listen)                                                      | number        | server              | -       | `false`   | Sets port which the server will accept requests. syntax: `listen port_number`                                                                           |
| server_name[<sup>[6]</sup>](#server_name)                                            | string        | server              | -       | `false`   | Sets names of a virtual server. syntax: `server_name example.com www.example.com`                                                                       |
| accepted_http_methods[<sup>[7]</sup>](#accepted_http_methods)                        | string        | -                   | \*      | `false`   | Sets accpeted http methods. syntax: `accepted_http_methods method ...`. example: `accepted_http_methods GET HEAD POST PUT DELETE CONNECT OPTIONS TRACE` |
| rewrite[<sup>[8]</sup>](#rewrite)                                                    | string        | -                   | -       | `false`   | URI changed as specified in the replacement string. syntax: `rewrite replacement`. example `rewrite /images`                                            |
| index[<sup>[9]</sup>](#index)                                                        | string        | -                   | -       | `true`    | Defines files that will be used as an index. syntax: `index file ...`. example `index index.html index.htm`                                             |
| path[<sup>[11]</sup>](#path_1)[<sup>[12]</sup>](#path_2)                             | string        | location            | -       | `false`   | Defines the location path. syntax: `path location_path`. example `path /`                                                                               |
| cgi[<sup>[13]</sup>](#cgi)                                                           | string        | -                   | -       | `true`    | Defines the php cgi path. syntax: `cgi path/to/php/cgi extension`. example `cgi /Users/nerraou/bin/php-7.4.33/sapi/cgi/php-cgi php`                     |

## Examples

### Basic

```
http {
  root /var/www/html
  error_page 404 404.html

  server {
    listen 80
    server_name example.org www.example.org
  }
}

```

### Full example

```

http {
  root /var/www/html
  client_max_body_size 10M
  autoindex on
  error_page 404 404.html
  index index.html index.htm
  cgi php/CGI/path php

  server {
    listen 80
    server_name example.org www.example.org
    autoindex on
    error_page 404 404.html
    cgi python/CGI/path python

    location {
      path /images
      root /data
      autoindex off
      client_max_body_size 10M
      accepted_http_methods GET PUT POST DELETE PATCH
      rewrite /new/images/route
      error_page 404 404.html
      cgi ruby/CGI/path ruby
    }
  }
}

```

## References

1. <a nane="root">[root](https://nginx.org/en/docs/http/ngx_http_core_module.html#root)
2. <a name="autoindex">[autoindex](http://nginx.org/en/docs/http/ngx_http_autoindex_module.html#autoindex)</a>
3. <a name="client_max_body_size">[client max body size](https://nginx.org/en/docs/http/ngx_http_core_module.html#client_max_body_size)</a>
4. <a name="error_page">[error page](https://nginx.org/en/docs/http/ngx_http_core_module.html#error_page)</a>
5. <a name="listen">[listen](https://nginx.org/en/docs/http/ngx_http_core_module.html#listen)</a>
6. <a name="server_name">[server name](https://nginx.org/en/docs/http/ngx_http_core_module.html#server_name)</a>
7. <a name="accepted_http_methods">[accepted http methods](https://datatracker.ietf.org/doc/html/rfc7231#section-4)</a>
8. <a name="rewrite">[rewrite](https://nginx.org/en/docs/http/ngx_http_rewrite_module.html#rewrite)</a>
9. <a name="index">[index](https://nginx.org/en/docs/http/ngx_http_index_module.html#index)</a>
10. <a name="units">[units](https://nginx.org/en/docs/syntax.html)</a>
11. <a name="path_1">[path](https://nginx.org/en/docs/http/ngx_http_core_module.html#location)</a>
12. <a name="path_2">[path](https://www.digitalocean.com/community/tutorials/nginx-location-directive)</a>
13. <a name="cgi">[fastcgi](https://www.nginx.com/resources/wiki/start/topics/examples/fastcgiexample)</a>
