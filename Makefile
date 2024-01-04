# default variables
CXX := g++
CXXFLAGS := -Wall -Wextra -Werror
LDFLAGS :=
LDLIBS := 
CPPFLAGS :=

BUILDDIR := ./build

# DEBUG Target
.PHONY: debug
debug: BUILDDIR := ./build/Debug
debug: CXXFLAGS += -Og -g
debug: webserver

# RELEASE Target
.PHONY: release
release: BUILDDIR := ./build/Release
release: CXXFLAGS += -O2 
release: webserver

# link the target here
webserver: builddir main
	$(CXX) $(CXXFLAGS) $(BUILDDIR)/main.o -o $(BUILDDIR)/webserver $(LDFLAGS) $(LDLIBS)

# compile all objects here
main: main.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c main.cpp -o $(BUILDDIR)/main.o

# create build folder if not exists
.PHONY: builddir
builddir:  
	mkdir -p $(BUILDDIR)

# clean up build directory
.PHONY: clean
clean: 
	rm -rf $(BUILDDIR)/*
	rm -f ./compile_commands.json

