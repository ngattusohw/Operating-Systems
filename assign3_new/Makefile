CXX      = g++
CXX_FILE = $(wildcard *.cpp)
TARGET   = $(patsubst %.cpp,%,$(CXX_FILE))
CXXFLAGS = -g -fmessage-length=0 -std=c++11


all:
	$(CXX) $(CXXFLAGS) $(CXX_FILE) -o $(TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe
re: clean all
