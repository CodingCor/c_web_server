# default variables
CXX := g++
CXXFLAGS := -Wall -Wextra -Werror
LDFLAGS :=
LDLIBS := 
CPPFLAGS :=

BUILDDIR := ./build

OBJECTS := ${BUILDDIR}/main.o
OBJECTS += ${BUILDDIR}/cookiemap.o
OBJECTS += ${BUILDDIR}/stringlist.o
OBJECTS += ${BUILDDIR}/http.o
OBJECTS += ${BUILDDIR}/util.o

CXXFLAGS += -Og -g 
# CXXFLAGS += -O2

# link the target here
${BUILDDIR}/webserver: builddir ${OBJECTS}
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILDDIR)/webserver $(LDFLAGS) $(LDLIBS)

# compile all objects here
${BUILDDIR}/main.o: main.cpp include/cookiemap.h include/http.h include/util.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c main.cpp -o $(BUILDDIR)/main.o

${BUILDDIR}/cookiemap.o: src/cookiemap.cpp include/cookiemap.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/cookiemap.cpp -o $(BUILDDIR)/cookiemap.o

${BUILDDIR}/stringlist.o: src/stringlist.cpp include/stringlist.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/stringlist.cpp -o $(BUILDDIR)/stringlist.o

${BUILDDIR}/http.o: src/http.cpp include/http.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/http.cpp -o $(BUILDDIR)/http.o

${BUILDDIR}/util.o: src/util.cpp include/util.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/util.cpp -o $(BUILDDIR)/util.o

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

