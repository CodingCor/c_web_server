# default variables
CXX := g++
CXXFLAGS := -Wall -Wextra -Werror
LDFLAGS :=
LDLIBS := 
CPPFLAGS :=

BUILDDIR := ./build

OBJECTS := ${BUILDDIR}/main.o
OBJECTS += ${BUILDDIR}/cookiemap.o

CXXFLAGS += -Og -g 
# CXXFLAGS += -O2

# link the target here
${BUILDDIR}/webserver: builddir ${OBJECTS}
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILDDIR)/webserver $(LDFLAGS) $(LDLIBS)

# compile all objects here
${BUILDDIR}/main.o: main.cpp include/cookiemap.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c main.cpp -o $(BUILDDIR)/main.o

${BUILDDIR}/cookiemap.o: src/cookiemap.cpp include/cookiemap.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/cookiemap.cpp -o $(BUILDDIR)/cookiemap.o

# create build folder if not exists
.PHONY: builddir
builddir:  
	mkdir -p $(BUILDDIR)

# clean up build directory
.PHONY: clean
clean: 
	rm -rf ${OBJECTS}
	rm -rf ${BUILDDIR}/webserver
	rm -f ./compile_commands.json

