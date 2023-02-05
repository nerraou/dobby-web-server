NAME=dobby
CC_FLAGS = -g -Wall -Wextra -Werror -std=c++98
CC = c++

INCLUDE_ARGS=\
	-I src/lib/ -I src/parseConfig/ -I src/config/ \
	-I src/ -I src/url/ -I src/fileStat/ -I src/httpStatus/ \
	-I src/http/ -I src/socket -I src/typedefs/ \
	-I src/contentType/\

OBJECT_FILES=$(SOURCE_FILES:.cpp=.o)
SOURCE_FILES=\
	src/main.cpp\
	\
	src/config/ConfigHttp.cpp\
	src/config/ConfigLocation.cpp\
	src/config/ConfigServer.cpp\
	src/config/ConfigException.cpp\
	src/config/Config.cpp\
	src/config/Rewrite.cpp\
	\
	src/lib/lib.cpp\
	\
	src/parseConfig/ParseConfig.cpp\
	\
	src/url/Url.cpp\
	\
	src/fileStat/FileStat.cpp\
	\
	src/http/ServerGroup.cpp\
	\
	src/http/Http.cpp\
	src/http/Server.cpp\
	src/http/HttpException.cpp\
	src/http/HttpParser.cpp\
	src/http/HttpRequestHandler.cpp\
	\
	src/socket/ServerSocket.cpp\
	\
	src/contentType/ContentType.cpp\


HPP_FILES=\
	src/typedefs/typedefs.hpp\
	\
	src/config/ConfigHttp.hpp\
	src/config/ConfigLocation.hpp\
	src/config/ConfigServer.hpp\
	src/config/ConfigException.hpp\
	src/config/Config.hpp\
	src/config/Rewrite.hpp\
	\
	src/lib/lib.hpp\
	\
	src/parseConfig/ParseConfig.hpp\
	\
	src/url/Url.hpp\
	\
	src/fileStat/FileStat.hpp\
	\
	src/httpStatus/httpStatus.hpp\
	\
	src/http/ServerGroup.hpp\
	\
	src/http/Server.hpp\
	src/http/Http.hpp\
	src/http/HttpRequestHandler.hpp\
	src/http/HttpException.hpp\
	src/http/HttpParser.hpp\
	\
	src/socket/ServerSocket.hpp\
	\
	src/contentType/ContentType.hpp\

all: $(NAME)

$(NAME): $(OBJECT_FILES)
	$(CC) $(CC_FLAGS) -o $@ $^

%.o: %.cpp $(HPP_FILES)
	$(CC) $(CC_FLAGS) $(INCLUDE_ARGS) -c $< -o $@

clean:
	rm -f $(OBJECT_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all
