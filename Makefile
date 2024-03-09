CC := gcc
CXX := g++
MD := mkdir
RM := rm

INCLUDE = -I /usr/local/include/eigen3 -I /usr/local/include -I /usr/local/opt/libomp/include
LIBRARY = -L /usr/local/opt/llvm/lib
CXXFLAGS = -std=c++17 -Xclang -fopenmp -lomp -O3 $(LIBRARY) $(INCLUDE)

SRCDIR = src
BINDIR = bin
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)
EXEC = gene_pairs

.PHONY: all clean

all: $(EXEC)

$(BINDIR):
	$(MD) -p $(BINDIR)
	
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -f $(OBJS) $(EXEC)