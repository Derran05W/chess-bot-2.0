# Makefile

# Compiler and flags
CXX       := g++
CXXFLAGS  := -std=c++20 -O2 -Wall -Wextra

# Where our sources live
SRCDIR    := src
SRCS      := $(wildcard $(SRCDIR)/*.cpp)

# Derive object names by replacing .cpp with .o
OBJS      := $(SRCS:.cpp=.o)

# Final executable
TARGET    := chess-bot

# Default target
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile each .cpp → .o
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
