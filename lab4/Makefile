.SUFFIXES:         # remove all the implicit rules (avoid conflicts with our own rules)
.SECONDARY:        # all files are intermediate (keep them, otherwise they are removed)
.PHONY: all clean  # these targets are not files (even if a file named "all" or "clean" exists, the rule is applied)


CXX = g++
CFLAGS = -std=c++20 -Wall -Wextra -g

OBJS = main.o location.o shortest_path.o
TARGET = bilbao

all: $(TARGET)

$(TARGET): $(OBJS)
	$(C) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(C) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *~	