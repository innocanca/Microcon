CXX = g++ -std=c++11 
CPPFLAGS = -Wall -g `mysql_config --cflags --libs`
TARGET := server
SRCDIRS := src
SRCEXTS := .cpp
SHELL = /bin/sh
SOURCES := $(wildcard $(SRCDIRS)/*.cpp)

%.o : src/%.cpp
	$(CXX) $(CPPFLAGS) -c  $< -o $@  

OBJS =	$(patsubst %.cpp,%.o,$(SOURCES))
$(TARGET):$(OBJS)
	$(CXX)  $(OBJS) -o $(TARGET) 
clean:
	rm -rf  *.o server
