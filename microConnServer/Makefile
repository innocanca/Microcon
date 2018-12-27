CXX = g++ -std=c++11 
CPPFLAGS = -Wall -g
MYSQLFLAGS =`mysql_config --cflags --libs`
TARGET := server
SRCDIRS := src
SRCEXTS := .cpp
SHELL = /bin/sh
SOURCES := $(wildcard $(SRCDIRS)/*.cpp)

%.o : src/%.cpp
	$(CXX) -c $(CPPFLAGS) $(MYSQLFLAGS)  $< -o $@

OBJS =	$(patsubst %.cpp,%.o,$(SOURCES))
$(TARGET):$(OBJS)
	$(CXX) $(MYSQLFLAGS) $(OBJS) -o $(TARGET)
clean:
	rm -rf  *.o server
