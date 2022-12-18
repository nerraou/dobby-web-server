- http

  1. root
  2. autoindex
  3. client_max_body_size
  4. error_page

- server

  1. root
  2. listen
  3. server_name
  4. error_page

- location

  1. root
  1. autoindex
  1. client_max_body_size
  1. accepted_http_methods
  1. rewrite

| Attribute                                                     | Type          | Required in context | Default | Inherited | Description                                                                                                                                               |
| ------------------------------------------------------------- | ------------- | ------------------- | ------- | --------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| root[<sup>[1]</sup>](#autoindex)                              | string        | http                | -       | `true`    | Specifies the root directory that will be used to search for a file. syntax: `root: /var/www/html;`                                                       |
| autoindex[<sup>[2]</sup>](#autoindex)                         | enum(on, off) | -                   | `off`   | `true`    | Used to enable/disable directory listing. example: autoindex: on;                                                                                         |
| client_max_body_size[<sup>[3]</sup>](#client_max_body_size)   | string        | http                | `1M`    | `true`    | Sets the maximum allowed size of the client request body. syntax: `client_max_body_size: 1M;`                                                             |
| error_page[<sup>[3]</sup>](#error_page)                       | string        | http                | -       | `true`    | Syntax: `error_page code ... uri;`. example: `error_page 400 404 /40x.html;`                                                                              |
| listen[<sup>[4]</sup>](#listen)                               | number        | server              | -       | `false`   | Sets port which the server will accept requests. syntax: `listen port_number;`                                                                            |
| server_name[<sup>[5]</sup>](#server_name)                     | string        | server              | -       | `flase`   | Sets names of a virtual server. syntax: `server_name: example.com www.example.com;`                                                                       |
| accepted_http_methods[<sup>[6]</sup>](#accepted_http_methods) | string        | -                   | \*      | `false`   | Sets accpeted http methods. syntax: `accepted_http_methods method ...;`. example: `accepted_http_methods GET HEAD POST PUT DELETE CONNECT OPTIONS TRACE;` |
| rewrite[<sup>[7]</sup>](#rewrite)                             | string        | -                   | -       | `false`   | URI changed as specified in the replacement string. syntax: `rewrite replacement`. example `rewrite /images`                                              |

## Examples

### Basic

```
http {
  root /var/www/html;
  error_page 404 404.html

  server {
    listen 80;
    server_name example.org www.example.org;
  }
}

```

### Full example

```

http {
  root /var/www/html;
  client_max_body_size 10M;
  autoindex on;
  error_page 404 404.html

  server {
    listen 80;
    server_name example.org www.example.org;
    autoindex on;
    error_page 404 404.html

    location /images {
      root /data;
      autoindex off;
      client_max_body_size 10M;
      accepted_http_methods GET PUT POST DELETE PATCH;
      rewrite /new/images/route
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
