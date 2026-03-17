CXX = g++

CXXFLAGS = -O2 \
`root-config --cflags` \
`$(HOME)/Downloads/pythia8313/bin/pythia8-config --cxxflags` \
-std=c++17

LDFLAGS = \
`root-config --libs` \
`$(HOME)/Downloads/pythia8313/bin/pythia8-config --libs`

TARGET = relatorioparcial
SRC = Relatorioparcial.cc

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

