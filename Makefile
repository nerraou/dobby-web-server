NAME=dobby
CC_FLAGS = -Wall -Wextra -Werror -std=c++98
CC = c++

INCLUDE_ARGS= -I src/lib -I src/parseConfig/ -I src/config/ -I src/ -I src/url -I src/filestat -I src/httpStatus -I src/ServerGroup

OBJECT_FILES=$(SOURCE_FILES:.cpp=.o)
SOURCE_FILES=\
	src/main.cpp\
	src/ServerSocket.cpp\
	src/Connection.cpp\
	src/Server.cpp\
	src/Http.cpp\
	\
	src/config/ConfigHttp.cpp\
	src/config/ConfigLocation.cpp\
	src/config/ConfigServer.cpp\
	\
	src/HttpRequestHandler.cpp\
	\
	src/lib/lib.cpp\
	\
	src/parseConfig/ParseConfig.cpp\
	\
	src/url/Url.cpp\
	\
	src/filestat/FileStat.cpp\
	\
	src/ServerGroup/ServerGroup.hpp

HPP_FILES=\
	src/ServerSocket.hpp\
	src/Connection.hpp\
	src/Server.hpp\
	src/Http.hpp\
	src/typedefs.hpp\
	\
	src/config/ConfigHttp.hpp\
	src/config/ConfigLocation.hpp\
	src/config/ConfigServer.hpp\
	\
	src/HttpRequestHandler.hpp\
	\
	src/lib/lib.hpp\
	\
	src/parseConfig/ParseConfig.hpp\
	\
	src/url/Url.hpp\
	\
	src/filestat/FileStat.hpp\
	\
	src/httpStatus/httpStatus.hpp\
	\
	src/ServerGroup/ServerGroup.hpp

all: $(NAME)

$(NAME): $(OBJECT_FILES)
	$(CC) -o $@ $^

%.o: %.cpp $(HPP_FILES)
	$(CC) $(CC_FLAGS) $(INCLUDE_ARGS) -c $< -o $@

clean:
	rm -f $(OBJECT_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all
