# Dobby Web Server

## Getting started

```bash
make && ./dobby config_file
```

## Project file structure

```
.
├── README.md
├── CONFIG.md
├── .gitignore
├── Makefile
├── wwww/
└── src
    └── config
    │   ├── ConfigHttp.cpp
    │   ├── ConfigHttp.hpp
    │   ├── ConfigLocation.cpp
    │   ├── ConfigLocation.hpp
    │   ├── ConfigServer.cpp
    │   └── ConfigServer.hpp
    ├── fileStat
    │   ├── FileStat.cpp
    │   └── FileStat.hpp
    ├── http
    │   ├── Http.cpp
        ├── Http.hpp
    │   ├── HttpException.cpp
    │   ├── HttpException.hpp
    │   ├── HttpParser.cpp
    │   ├── HttpParser.hpp
    │   ├── HttpRequestHandler.cpp
    │   ├── HttpRequestHandler.hpp
    │   ├── Server.hpp
    │   └── Server.hpp
    ├── httpStatus
    │   └── httpStatus.hpp
    ├── lib
    │   ├── lib.cpp
    │   └── lib.hpp
    ├── parseConfig
    │   ├── ParseConfig.cpp
    │   └── ParseConfig.hpp
    ├── serverGroup
    │   ├── ServerGroup.cpp
    │   └── ServerGroup.hpp
    ├── socket
    │   ├── ServerSocket.cpp
    │   └── ServerSocket.hpp
    ├── url
    │   ├── Url.cpp
    │   └── Url.hpp
    ├── typedefs
    │   └── typedefs.hpp
    └── main.cpp
```

## Naming conventions

we will be using the c++ naming convention as descrived [here](https://manual.gromacs.org/documentation/5.1-current/dev-manual/naming.html), with some changes.

- Use CamelCase for all names. Start types (such as classes, structs, and typedefs) with a capital letter, other names (functions, variables) with a lowercase letter.
- C++ interfaces are named with a `I` prefix, and abstract base classes with an `A` prefix.
- Member variables are named with an underscore(`_`) prefix.
- Accessors for a variable foo\* are named foo() and setFoo().
- Global variables are named with a g\* prefix.
- Static class variables are named with a s\* prefix.
- Global constants are often named with a c\* prefix.
- If the main responsibility of a file is to implement a particular class, then the name of the file should match that class.
