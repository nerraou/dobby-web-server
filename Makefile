NAME=dobby
CC_FLAGS = -Wall -Wextra -Werror -std=c++98
CC = c++

OBJECT_FILES=$(SOURCE_FILES:.cpp=.o)
SOURCE_FILES=\
	src/main.cpp\
	src/ServerSocket.cpp\
	src/Connection.cpp\
	src/Server.cpp\
	\
	src/config/ConfigHttp.cpp\
	src/config/ConfigLocation.cpp\
	src/config/ConfigServer.cpp\

HPP_FILES=\
	src/ServerSocket.hpp\
	src/Connection.hpp\
	src/Server.hpp\
	\
	src/config/ConfigHttp.hpp\
	src/config/ConfigLocation.hpp\
	src/config/ConfigServer.hpp\
	src/config/ErrorPage.hpp\

all: $(NAME)

$(NAME): $(OBJECT_FILES)
	$(CC) -o $@ $^

%.o: %.cpp $(HPP_FILES)
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJECT_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all
