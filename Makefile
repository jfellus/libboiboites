#################################
# NOTES FOR UBUNTU 12.04 USERS :
# --------------------------------
# EVENTUALLY YOU'LL HAVE TO TYPE : sudo apt-add-repository ppa:teward/libmicrohttpd-quantal
# INSTALL GCC 4.8 and G++ 4.8 by typing : 
#	sudo add-apt-repository ppa:ubuntu-toolchain-r/test
#	sudo apt-get update
#	sudo apt-get install gcc-4.8 g++-4.8
####################################


###### INSTALLATION GUIDE ######
# make external_libs
# make
# ./coeos++
################################


APT_GET_DEPENDENCIES:= libmicrohttpd-dev libmicrohttpd10 libgtk-3-dev libwebkitgtk-dev libxml++2.6-dev


REQUIRED_PACKAGES:= cairo gtk+-3.0 webkitgtk-3.0 libmicrohttpd libxml++-2.6
REQUIRED_LIBS:= -lpthread


SRC_DIR:=./src

LIBRARY_NAME:=libboiboites.so







########################## DON'T EDIT BELOW THIS LINE (unless you are a gnu make's expert ##############

SRC := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(addprefix bin/,$(SRC:.cpp=.o))

$(LIBRARY_NAME): $(OBJS)

CXXFLAGS := -g -rdynamic -Wall -MMD `pkg-config --cflags $(REQUIRED_PACKAGES)`
LDFLAGS := -rdynamic `pkg-config --libs $(REQUIRED_PACKAGES)` $(REQUIRED_LIBS)
DEPENDS = $(OBJS:.o=.d)    

$(LIBRARY_NAME) : $(OBJS)          
	@echo "Build library $@"
	@-$(CXX) -fPIC -shared $(OBJS) -o $@ $(LDFLAGS)
	@cp $@ ~/bin_leto_prom/
	@echo "DONE" 

bin/%.o: %.cpp
	@mkdir -p `dirname $(@:.o=.d)`
	@touch $(@:.o=.d)
	@echo "Compilation : $< "
	@-g++ -fPIC -std=c++11 $(CXXFLAGS) -MMD -c $< -o $@

bin: 
	@mkdir -p bin

clean::
	@rm -f $(LIBRARY_NAME)
	@rm -rf bin
	
all: $(LIBRARY_NAME)

external_libs:
	@echo "We need your password for this : "
	@sudo echo "thank you"
	@sudo apt-get install $(APT_GET_DEPENDENCIES)
	@echo "DONE"
	

-include $(DEPENDS) 
