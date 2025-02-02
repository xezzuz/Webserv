# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nazouz <nazouz@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/21 09:47:15 by nazouz            #+#    #+#              #
#    Updated: 2025/02/02 13:09:25 by nazouz           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			= 		webserv

CPP				= 		c++

CPPFLAGS		= 		-Wall -Werror -Wextra -fsanitize=address -std=c++98

INCLUDE			=		\
						./Config/Config.hpp \
						./IEventHandler.hpp \
						./Client/Request/Request.hpp \
						./Client/Response/Response.hpp \
						./Client/Response/Error.hpp \
						./Server/ServerHandler.hpp \
						./Client/ClientHandler.hpp \
						./HTTPServer/Webserv.hpp \
						./Client/CGI/CGIHandler.hpp \


SRCS			= 		\
						./Config/Config.cpp \
						./Config/Parsing.cpp \
						./Config/ServerConstructor.cpp \
						./Config/Validators.cpp \
						./HTTPServer/Webserv.cpp \
						./Client/Request/Request.cpp \
						./Client/Request/_ControlCenter.cpp \
						./Client/Request/Headers.cpp \
						./Client/Request/Body.cpp \
						./Client/Response/Response.cpp \
						./Client/Response/Error.cpp \
						./Client/Response/Setup.cpp \
						./Client/Response/AutoIndex.cpp \
						./Client/Response/Range.cpp \
						./Client/Response/Methods/GET.cpp \
						./Client/Response/Methods/POST.cpp \
						./Client/Response/Methods/DELETE.cpp \
						./Client/CGI/CGIHandler.cpp \
						./Client/ClientHandler.cpp \
						./Server/ServerHandler.cpp \
						./Utils/Helpers.cpp \
						./main.cpp

OBJDIR			=		./objects

OBJS			= 		$(SRCS:%.cpp=$(OBJDIR)/%.o)

all : $(NAME)

$(OBJDIR)/%.o : %.cpp $(INCLUDE)
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

clean :
	rm -rf $(OBJDIR)

fclean : clean
	rm -rf $(NAME)

re : fclean all