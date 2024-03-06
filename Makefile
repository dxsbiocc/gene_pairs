CC := g++
MD := mkdir
RM := rm

INCLUDE = -I /usr/local/include/eigen3 -I /usr/local/include
CFLAGS = -std=c++17 $(INCLUDE)

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
	$(CC) $(CFLAGS) $^ -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f $(OBJS) $(EXEC)