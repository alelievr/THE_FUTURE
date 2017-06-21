# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alelievr <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created  2014/07/15 15:13:38 by alelievr          #+#    #+#              #
#    Updated  2017/06/09 04:55:16 by alelievr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#################
##  VARIABLES  ##
#################

#	Sources
SRCDIR		=	src
SRC			=	ShaderRender.cpp		\
				ShaderProgram.cpp		\
				ShaderChannel.cpp		\
				ShaderApplication.cpp	\
				Element.cpp				\
				lua_utils.cpp			\
				LuaGL.cpp				\
				wav.cpp					\
				utils.cpp				\
				main.cpp				\
				NetworkManager.cpp		\
				Timer.cpp				\
				SyncOffset.cpp			\
				Timeval.cpp				\
				NetworkGUI.cpp			\
				ClusterConfig.cpp		\
				KernelProgram.cpp		\


#	Objects
OBJDIR		=	obj

#	Variables
LIBFT		=	2	#1 or 0 to include the libft / 2 for autodetct
DEBUGLEVEL	=	2	#can be 0 for no debug 1 for or 2 for harder debug
					#Warrning: non null debuglevel will disable optlevel
OPTLEVEL	=	1	#same than debuglevel
					#Warrning: non null optlevel will disable debuglevel
CPPVERSION	=	c++14
#For simpler and faster use, use commnd line variables DEBUG and OPTI:
#Example $> make DEBUG=2 will set debuglevel to 2

#	Includes
INCDIRS		=	inc SOIL2/incs glfw/include lua/5.1/src/ SFML/include SFGUI/include

#	Libraries
LIBDIRS		=	lua/5.1/src glfw/src/ SFML/lib SFGUI/build/lib
LDLIBS		=	-lglfw3 -llua -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window -lsfgui
GLFWLIB		=	glfw/src/libglfw3.a
SOILLIB		=	SOIL2/libSOIL2.so
LUALIB		=	lua/5.1/src/liblua.a
SFMLLIB		=	SFML/lib/libsfml-system.dylib
SFGUILIB	=	SFGUI/build/lib/libsfgui.a

#	Output
NAME		=	visualishader

#	Compiler
WERROR		=	#-Werror
CFLAGS		=	-Wall -Wextra -ferror-limit=999
CPROTECTION	=	-z execstack -fno-stack-protector

DEBUGFLAGS1	=	-ggdb
DEBUGFLAGS2	=	-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
OPTFLAGS1	=	-funroll-loops -O2
OPTFLAGS2	=	-pipe -funroll-loops -Ofast

#	Framework
FRAMEWORK	=	

#################
##  COLORS     ##
#################
CPREFIX		=	"\033[38;5;"
BGPREFIX	=	"\033[48;5;"
CCLEAR		=	"\033[0m"
CLINK_T		=	$(CPREFIX)"129m"
CLINK		=	$(CPREFIX)"93m"
COBJ_T		=	$(CPREFIX)"119m"
COBJ		=	$(CPREFIX)"113m"
CCLEAN_T	=	$(CPREFIX)"9m"
CCLEAN		=	$(CPREFIX)"166m"
CRUN_T		=	$(CPREFIX)"198m"
CRUN		=	$(CPREFIX)"163m"
CNORM_T		=	"226m"
CNORM_ERR	=	"196m"
CNORM_WARN	=	"202m"
CNORM_OK	=	"231m"

#################
##  OS/PROC    ##
#################

OS			:=	$(shell uname -s)
PROC		:=	$(shell uname -p)
DEBUGFLAGS	=	
LINKDEBUG	=	
OPTFLAGS	=	
#COMPILATION	=	

SFML_FRAMEWORK_PATH = SFML/extlibs/libs-osx/Frameworks
SFML_LIB_PATH =		SFML/lib

ifeq "$(OS)" "Windows_NT"
endif
ifeq "$(OS)" "Linux"
	LDLIBS		+= -lm -lGL -lX11 -lXrandr -lXrender -lXi -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lrt -lbsd
	CFLAGS		+= -fPIC
	DEBUGFLAGS	+= -fsanitize=memory -fsanitize-memory-use-after-dtor -fsanitize=thread
	LUAMAKEOS	= linux
endif
ifeq "$(OS)" "Darwin"
	FRAMEWORK	= OpenGL AppKit IOKit CoreVideo Cocoa Carbon AudioUnit CoreAudio vorbis freetype vorbisfile OpenAL FLAC ogg OpenCL
	FRAMEWORKPATH =	SFML/extlibs/libs-osx/Frameworks
	LUAMAKEOS	= macosx
	SFML_AUTO_DYLIB = $(shell pwd)/SFML/lib/libsfml-audio.dylib
	OSX_DYLIB_PATH_CORRECTION = install_name_tool -change @rpath/../Frameworks/vorbis.framework/Versions/A/vorbis $(SFML_FRAMEWORK_PATH)/vorbis.framework/Versions/A/vorbis $(NAME); \
								install_name_tool -change @rpath/../Frameworks/freetype.framework/Versions/A/freetype $(SFML_FRAMEWORK_PATH)/freetype.framework/Versions/A/freetype $(NAME); \
								install_name_tool -change @rpath/../Frameworks/OpenAL.framework/Versions/A/OpenAL $(SFML_FRAMEWORK_PATH)/OpenAL.framework/Versions/A/OpenAL $(NAME); \
								install_name_tool -change @rpath/../Frameworks/vorbisfile.framework/Versions/A/vorbisfile $(SFML_FRAMEWORK_PATH)/vorbisfile.framework/Versions/A/vorbisfile $(NAME); \
								install_name_tool -change @rpath/../Frameworks/FLAC.framework/Versions/A/FLAC $(SFML_FRAMEWORK_PATH)/FLAC.framework/Versions/A/FLAC $(NAME); \
								install_name_tool -change @rpath/../Frameworks/ogg.framework/Versions/A/ogg $(SFML_FRAMEWORK_PATH)/ogg.framework/Versions/A/ogg $(NAME); \
								install_name_tool -change @rpath/libsfml-audio.2.4.dylib $(SFML_LIB_PATH)/libsfml-audio.dylib $(NAME); \
								install_name_tool -change @rpath/libsfml-graphics.2.4.dylib $(SFML_LIB_PATH)/libsfml-graphics.dylib $(NAME); \
								install_name_tool -change @rpath/libsfml-system.2.4.dylib $(SFML_LIB_PATH)/libsfml-system.dylib $(NAME); \
								install_name_tool -change @rpath/libsfml-window.2.4.dylib $(SFML_LIB_PATH)/libsfml-window.dylib $(NAME); \
								install_name_tool -change @rpath/../Frameworks/vorbisenc.framework/Versions/A/vorbisenc $(SFML_FRAMEWORK_PATH)/vorbisenc.framework/Versions/A/vorbisenc $(SFML_AUTO_DYLIB);

endif

#################
##  AUTO       ##
#################

NASM		=	nasm
OBJS		=	$(patsubst %.c,%.o, $(filter %.c, $(SRC))) \
				$(patsubst %.cpp,%.o, $(filter %.cpp, $(SRC))) \
				$(patsubst %.s,%.o, $(filter %.s, $(SRC)))
OBJ			=	$(addprefix $(OBJDIR)/,$(notdir $(OBJS)))
NORME		=	**/*.[ch]
VPATH		+=	$(dir $(addprefix $(SRCDIR)/,$(SRC)))
VFRAME		=	$(addprefix -framework ,$(FRAMEWORK))
INCFILES	=	$(foreach inc, $(INCDIRS), $(wildcard $(inc)/*.h))
CPPFLAGS	=	$(addprefix -I,$(INCDIRS))
LDFLAGS		=	$(addprefix -L,$(LIBDIRS))
FRAMEPATH	=	$(addprefix -F,$(FRAMEWORKPATH))
LINKER		=	cc

disp_indent	=	for I in `seq 1 $(MAKELEVEL)`; do \
					test "$(MAKELEVEL)" '!=' '0' && printf "\t"; \
				done

color_exec  =   $(call disp_indent); \
				echo $$tabs$(1)➤ $(3)$(2); \
				echo $$tabs '$(strip $(4))' $(CCLEAR); \
				$(4)

color_exec_t=   $(call disp_indent); \
				echo $(1)➤ '$(strip $(3))'$(2);$(3);printf $(CCLEAR)


ifneq ($(filter 1,$(strip $(DEBUGLEVEL)) ${DEBUG}),)
	OPTLEVEL = 0
	OPTI = 0
	DEBUGFLAGS += $(DEBUGFLAGS1)
endif
ifneq ($(filter 2,$(strip $(DEBUGLEVEL)) ${DEBUG}),)
	OPTLEVEL = 0
	OPTI = 0
	DEBUGFLAGS += $(DEBUGFLAGS1) $(DEBUGFLAGS2)
	LINKDEBUG += $(DEBUGFLAGS1) $(DEBUGFLAGS2)
	export ASAN_OPTIONS=check_initialization_order=1
endif

ifneq ($(filter 1,$(strip $(OPTLEVEL)) ${OPTI}),)
	DEBUGFLAGS = 
	OPTFLAGS = $(OPTFLAGS1)
endif
ifneq ($(filter 2,$(strip $(OPTLEVEL)) ${OPTI}),)
	DEBUGFLAGS = 
	OPTFLAGS = $(OPTFLAGS1) $(OPTFLAGS2)
endif

ifndef $(CXX)
	CXX = clang++
endif

ifneq ($(filter %.cpp,$(SRC)),)
	LINKER = $(CXX)
endif

ifdef ${NOWERROR}
	WERROR = 
endif

ifeq "$(strip $(LIBFT))" "2"
ifneq ($(wildcard ./libft),)
	LIBDIRS += "libft"
	LDLIBS += "-lft"
	INCDIRS += "libft"
endif
endif

#################
##  TARGETS    ##
#################

#	First target
all: $(GLFWLIB) $(SOILLIB) $(LUALIB) $(SFMLLIB) $(SFGUILIB) $(NAME)

$(LUALIB):
	@git submodule init
	@git submodule update
	cd lua/5.1/src && make $(LUAMAKEOS)

$(SOILLIB):
	@git submodule init
	@git submodule update
	cd SOIL2 && make dynamic

$(GLFWLIB):
	@git submodule init
	@git submodule update
	cd glfw && cmake . && make -j4

$(SFMLLIB):
	@git submodule init
	@git submodule update
	cd SFML && cmake -DBUILD_SHARED_LIBS=true CMAKE_BUILD_TYPE=Release . && make -j4

$(SFGUILIB):
	@git submodule init
	@git submodule update
	cd SFGUI && mkdir -p build && cd build && CMAKE_PREFIX_PATH=../../SFML cmake -DSFGUI_BUILD_SHARED_LIBS=false .. && make -j4

#	Linking
$(NAME): $(OBJ)
	@$(if $(findstring lft,$(LDLIBS)),$(call color_exec_t,$(CCLEAR),$(CCLEAR),\
		make -j 4 -C libft))
	@$(call color_exec,$(CLINK_T),$(CLINK),"Link of $(NAME):",\
		$(LINKER) $(WERROR) $(CFLAGS) $(LDFLAGS) $(OPTFLAGS) $(DEBUGFLAGS) $(LINKDEBUG) $(FRAMEPATH) $(VFRAME) -o $@ $^ $(LDLIBS)  $(SOILLIB))
	@$(OSX_DYLIB_PATH_CORRECTION)

$(OBJDIR)/%.o: %.cpp $(INCFILES)
	@mkdir -p $(OBJDIR)
	@$(call color_exec,$(COBJ_T),$(COBJ),"Object: $@",\
		$(CXX) -std=$(CPPVERSION) $(WERROR) $(CFLAGS) $(OPTFLAGS) $(DEBUGFLAGS) $(CPPFLAGS) -o $@ -c $<)

#	Objects compilation
$(OBJDIR)/%.o: %.c $(INCFILES)
	@mkdir -p $(OBJDIR)
	@$(call color_exec,$(COBJ_T),$(COBJ),"Object: $@",\
		clang $(WERROR) $(CFLAGS) $(OPTFLAGS) $(DEBUGFLAGS) $(CPPFLAGS) -o $@ -c $<)

$(OBJDIR)/%.o: %.s
	@mkdir -p $(OBJDIR)
	@$(call color_exec,$(COBJ_T),$(COBJ),"Object: $@",\
		$(NASM) -f macho64 -o $@ $<)

#	Removing objects
clean:
	@$(call color_exec,$(CCLEAN_T),$(CCLEAN),"Clean:",\
		$(RM) $(OBJ))
	@rm -rf $(OBJDIR)

#	Removing objects and exe
fclean: clean
	@$(call color_exec,$(CCLEAN_T),$(CCLEAN),"Fclean:",\
		$(RM) $(NAME))

#	All removing then compiling
re: fclean
	@$(MAKE) all

f:	all run

#	Checking norme
norme:
	@norminette $(NORME) | sed "s/Norme/[38;5;$(CNORM_T)➤ [38;5;$(CNORM_OK)Norme/g;s/Warning/[0;$(CNORM_WARN)Warning/g;s/Error/[0;$(CNORM_ERR)Error/g"

run: $(NAME)
	@echo $(CRUN_T)"➤ "$(CRUN)"./$(NAME) ${ARGS}\033[0m"
	@./$(NAME) ${ARGS}

codesize:
	@cat $(NORME) |grep -v '/\*' |wc -l

functions: $(NAME)
	@nm $(NAME) | grep U

coffee:
	@clear
	@echo ""
	@echo "                   ("
	@echo "	                     )     ("
	@echo "               ___...(-------)-....___"
	@echo '           .-""       )    (          ""-.'
	@echo "      .-''''|-._             )         _.-|"
	@echo '     /  .--.|   `""---...........---""`   |'
	@echo "    /  /    |                             |"
	@echo "    |  |    |                             |"
	@echo "     \  \   |                             |"
	@echo "      '\ '\ |                             |"
	@echo "        '\ '|                             |"
	@echo "        _/ /\                             /"
	@echo "       (__/  \                           /"
	@echo '    _..---""` \                         /`""---.._'
	@echo " .-'           \                       /          '-."
	@echo ":               '-.__             __.-'              :"
	@echo ':                  ) ""---...---"" (                :'
	@echo "\'._                '"--...___...--"'              _.'"
	@echo '   \""--..__                              __..--""/'
	@echo "     '._     """----.....______.....----"""         _.'"
	@echo '         ""--..,,_____            _____,,..--"""'''
	@echo '                      """------"""'
	@sleep 0.5
	@clear
	@echo ""
	@echo "                 ("
	@echo "	                  )      ("
	@echo "               ___..(.------)--....___"
	@echo '           .-""       )   (           ""-.'
	@echo "      .-''''|-._      (       )        _.-|"
	@echo '     /  .--.|   `""---...........---""`   |'
	@echo "    /  /    |                             |"
	@echo "    |  |    |                             |"
	@echo "     \  \   |                             |"
	@echo "      '\ '\ |                             |"
	@echo "        '\ '|                             |"
	@echo "        _/ /\                             /"
	@echo "       (__/  \                           /"
	@echo '    _..---""` \                         /`""---.._'
	@echo " .-'           \                       /          '-."
	@echo ":               '-.__             __.-'              :"
	@echo ':                  ) ""---...---"" (                :'
	@echo "\'._                '"--...___...--"'              _.'"
	@echo '   \""--..__                              __..--""/'
	@echo "     '._     """----.....______.....----"""         _.'"
	@echo '         ""--..,,_____            _____,,..--"""'''
	@echo '                      """------"""'
	@sleep 0.5
	@clear
	@echo ""
	@echo "               ("
	@echo "	                  )     ("
	@echo "               ___..(.------)--....___"
	@echo '           .-""      )    (           ""-.'
	@echo "      .-''''|-._      (       )        _.-|"
	@echo '     /  .--.|   `""---...........---""`   |'
	@echo "    /  /    |                             |"
	@echo "    |  |    |                             |"
	@echo "     \  \   |                             |"
	@echo "      '\ '\ |                             |"
	@echo "        '\ '|                             |"
	@echo "        _/ /\                             /"
	@echo "       (__/  \                           /"
	@echo '    _..---""` \                         /`""---.._'
	@echo " .-'           \                       /          '-."
	@echo ":               '-.__             __.-'              :"
	@echo ':                  ) ""---...---"" (                :'
	@echo "\'._                '"--...___...--"'              _.'"
	@echo '   \""--..__                              __..--""/'
	@echo "     '._     """----.....______.....----"""         _.'"
	@echo '         ""--..,,_____            _____,,..--"""'''
	@echo '                      """------"""'
	@sleep 0.5
	@clear
	@echo ""
	@echo "             (         ) "
	@echo "	              )        ("
	@echo "               ___)...----)----....___"
	@echo '           .-""      )    (           ""-.'
	@echo "      .-''''|-._      (       )        _.-|"
	@echo '     /  .--.|   `""---...........---""`   |'
	@echo "    /  /    |                             |"
	@echo "    |  |    |                             |"
	@echo "     \  \   |                             |"
	@echo "      '\ '\ |                             |"
	@echo "        '\ '|                             |"
	@echo "        _/ /\                             /"
	@echo "       (__/  \                           /"
	@echo '    _..---""` \                         /`""---.._'
	@echo " .-'           \                       /          '-."
	@echo ":               '-.__             __.-'              :"
	@echo ':                  ) ""---...---"" (                :'
	@echo "\'._                '"--...___...--"'              _.'"
	@echo '   \""--..__                              __..--""/'
	@echo "     '._     """----.....______.....----"""         _.'"
	@echo '         ""--..,,_____            _____,,..--"""'''
	@echo '                      """------"""'

.PHONY: all clean fclean re norme codesize
