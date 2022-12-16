NAME=dobby
CC_FLAGS = -Wall -Wextra -Werror -std=c++98
CC = c++

OBJECT_FILES=$(SOURCE_FILES:.cpp=.o)
SOURCE_FILES=\
	src/main.cpp\
	src/ServerSocket.cpp\
	src/Connection.cpp\
	src/Server.cpp\

all: $(NAME)

$(NAME): $(OBJECT_FILES)
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJECT_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all
