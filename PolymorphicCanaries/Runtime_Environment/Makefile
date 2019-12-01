CXX		= g++
GCCPLUGINS_DIR	= $(shell $(CXX) -print-file-name=plugin)
CXXFLAGS	= -I$(GCCPLUGINS_DIR)/include -shared -fPIC -O0 -fno-rtti -Wall
ENV_CXXFLAGS	= -Wl,--no-as-needed -shared -fPIC -O0 -Wall -ldl -lpthread -fno-stack-protector

# do not make changes below this line

ENV_SRC		= LIBPolymorphicCanaries.cpp
ENV_SHARED_OBJ	= $(ENV_SRC:.cpp=.so)

# phony targets
.PHONY: all clean

all:  $(ENV_SHARED_OBJ)


# build the runtime
$(ENV_SHARED_OBJ): $(ENV_SRC) $(ENV_SRC:.cpp=.h) offsets.h
	$(CXX) $(ENV_CXXFLAGS) $(ENV_SRC) -o $@

# clean
clean:
	rm -rf  $(ENV_SHARED_OBJ)
