# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/21 09:47:15 by nazouz            #+#    #+#              #
#    Updated: 2024/11/27 15:43:22 by nazouz           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			= 		Webserv

CPP				= 		c++

CPPFLAGS		= 		-fsanitize=address -g -g3 # -Wall -Werror -Wextra

INCLUDE			=		\
						./Request/Request.hpp \
						./Server/Server.hpp \
						./Server/Client.hpp \


SRCS			= 		\
						./Main/Webserv.cpp \
						./Config/Config.cpp \
						./Config/Parsing.cpp \
						./Config/Blocks.cpp \
						./Config/ServerConstructor.cpp \
						./Config/Validators.cpp \
						./Request/Request.cpp \
						./Request/_ControlCenter.cpp \
						./Request/Headers.cpp \
						./Request/Body.cpp \
						./Server/Server.cpp \
						./Server/Client.cpp \
						./Utils/Helpers.cpp \
						./main.cpp

OBJS			= 		$(SRCS:.cpp=.o)

all : $(NAME)

%.o : %.cpp $(INCLUDE)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME) $(OBJS)

re : fclean all