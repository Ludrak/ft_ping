.PHONY: clean fclean re all check_sources check_headers

# Name of target executable
NAME		= ft_ping

# Locations 
SRC_DIR		= src
INC_DIR		= inc
BIN_DIR		= bin
LIB_DIR		= lib

# Sources & Headers 
# - fill only with name of the file
# - make will check for the file in SRC_DIR
# - use "-" if empty
SRCS=				main.c context.c packet.c packet_error.c checksum.c options.c\
					net_utils.c time_utils.c print_util.c print_util_network.c\

# Librarys (only for local archives in project folder)
LIBRARYS	= 

CLANG		=	gcc
CPP_FLAGS	=	-Wextra -Wall -Werror #-g3 -fsanitize=address
CPP_IFLAGS	=	

CPP_LFLAGS	= -lm

# Fancy prefixes 
PREFIX_PROJECT=[\033[1;32m$(NAME)\033[0m]
PREFIX_COMP=\033[1;30m-\033[0m-\033[1;37m>\033[0m[\033[1;32m✔\033[0m]
PREFIX_LINK=[\033[1;32mLINK\033[0m]
PREFIX_INFO=[\033[1;32mINFO\033[0m]
PREFIX_WARN=[\033[0;33mWARN\033[0m]
PREFIX_ERROR=[\033[0;91mERROR\033[0m]
PREFIX_DUPL=[\033[1;33mDUPLICATES\033[0m]
PREFIX_CLEAN=[\033[1;31mCLEAN\033[0m]


############################################################################################


SRC_FILES	=	$(shell find $(SRC_DIR) | grep -E '$(shell echo $(SRCS) | tr ' ' '|')')
HEADER_FILES=	$(shell find $(INC_DIR) | grep -E '$(shell echo $(HEADERS) | tr ' ' '|')')
OBJS		=	$(addprefix $(BIN_DIR)/, $(SRC_FILES:.c=.o))
CPP_IFLAGS	+=	$(addprefix -I,$(INC_DIR)) $(addprefix -I,$(shell echo $(HEADER_FILES) | tr ' ' '\n' | rev | cut -d'/' -f2- | rev | sort | uniq))
CPP_LFLAG	+=	$(addprefix -L,$(addprefix $(LIB_DIR), $(LIBRARYS)))

#   Main rule
all: check_headers check_sources $(NAME)
	@echo "$(PREFIX_PROJECT)$(PREFIX_INFO) done"

#	check_sources :
#	simple bash script to check duplicates sources files 
check_sources:
	@duplicates=$$( echo $(SRC_FILES) | tr ' ' '\n' | rev | cut -d'/' -f1 | rev | sort | uniq -c | sed 's/ //g' ) ; \
	error=0 ; \
	for source in $$duplicates ; do \
			if [ $$(echo $$source | sed 's/[^0-9]//g') -gt 1 ] ; then \
				echo "$(PREFIX_PROJECT)$(PREFIX_DUPL)" Duplicates source files found for \"$$(echo $$source | sed 's/[0-9]//g')\" in: ; \
				find $(SRC_DIR) | grep $$(echo $$source | sed 's/[0-9]//g') ; \
				echo ; \
				error=1 ; \
			fi \
	done ; \
	if [ $$error -eq 1 ] ; then \
		echo "Error: cannot manage duplicates files, aborting..." ; \
		exit 1 ; \
	fi

#	check_headers :
#	simple bash script to check duplicates header files 
check_headers:
	@duplicates=$$( echo $(HEADER_FILES) | tr ' ' '\n' | rev | cut -d'/' -f1 | rev | sort | uniq -c | sed 's/ //g' ) ; \
	error=0 ; \
	for source in $$duplicates ; do \
			if [ $$(echo $$source | sed 's/[^0-9]//g') -gt 1 ] ; then \
				echo "$(PREFIX_PROJECT)$(PREFIX_DUPL)" Duplicates header files found for \"$$(echo $$source | sed 's/[0-9]//g')\" in: ; \
				find $(INC_DIR) | grep $$(echo $$source | sed 's/[0-9]//g') ; \
				echo ; \
				error=1 ; \
			fi \
	done ; \
	if [ $$error -eq 1 ] ; then \
		echo "Error: cannot manage duplicates files, aborting..." ; \
		exit 1 ; \
	fi

#	Linking rule
$(NAME): $(BIN_DIR) $(OBJS)
	@$(CLANG) $(OBJS) -o $(NAME) $(CPP_FLAGS) $(CPP_LFLAGS)
	@echo "$(PREFIX_PROJECT)$(PREFIX_LINK) Linking done for: $(NAME)"

#	Bin directory
$(BIN_DIR):
	@echo "$(PREFIX_PROJECT)$(PREFIX_WARN) No bin dir found. Creating one."
	@mkdir -p $(BIN_DIR)

# Compilation rule 
$(BIN_DIR)/$(SRC_DIR)/%.o : $(SRC_DIR)/%.c $(HEADER_FILES)
	@mkdir -p $(BIN_DIR)/$(shell dirname $<)
	@$(CLANG) -c $< -o $@ $(CPP_FLAGS) $(CPP_IFLAGS)
	@echo "$(PREFIX_PROJECT)$(PREFIX_COMP) Compiled: $(shell basename $<)"

# clean rule
clean:
	@echo "$(PREFIX_PROJECT)$(PREFIX_CLEAN) Cleaning $(BIN_DIR)/"
	@rm -rf $(BIN_DIR)

# final clean rule
fclean: clean
	@echo "$(PREFIX_PROJECT)$(PREFIX_CLEAN) Cleaning $(NAME)"
	@rm -f $(NAME)

# remake rule
re: fclean all
