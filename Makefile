# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mmaila <mmaila@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/21 09:47:15 by nazouz            #+#    #+#              #
#    Updated: 2025/01/19 21:11:09 by mmaila           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			= 		webserv

CPP				= 		c++

CPPFLAGS		= 		-Wall -Werror -Wextra #-fsanitize=address

INCLUDE			=		\
						./Request/Request.hpp \
						./Response/Response.hpp \
						./Server/Server.hpp \
						./Server/Client.hpp \
						./Main/Webserv.hpp \


SRCS			= 		\
						./Config/Config.cpp \
						./Main/Webserv.cpp \
						./Config/Parsing.cpp \
						./Config/Blocks.cpp \
						./Config/ServerConstructor.cpp \
						./Config/Validators.cpp \
						./Request/Request.cpp \
						./Request/_ControlCenter.cpp \
						./Request/Headers.cpp \
						./Request/Body.cpp \
						./Response/Response.cpp \
						./Response/_ControlCenter.cpp \
						./Response/Location.cpp \
						./Response/Methods/GET.cpp \
						./Response/Methods/POST.cpp \
						./Response/Methods/DELETE.cpp \
						./Server/Server.cpp \
						./Server/Client.cpp \
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