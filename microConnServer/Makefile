CXX = g++ -std=c++11 -w -g `mysql_config --cflags --libs`
CPPFLAGS = -I include
TARGET = server
SRCDIRS = src
OUTDIRS = bin/
SOURCES =$(wildcard $(SRCDIRS)/*.cpp)
OBJS =	$(patsubst %.cpp,%.o,$(SOURCES))
%.o : $(SRCDIRS)/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@   $< 

$(TARGET):$(OBJS)
	$(CXX)  $(OBJS) -o $(TARGET) 
clean:
	rm -rf  src/*.o server
count:
	make clean
	find . -name "*[.cpp|.h]" | xargs wc -l
