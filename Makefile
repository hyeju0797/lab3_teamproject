CC = gcc

LIB_DIR = lib

LIB_SOURCES = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJECTS = $(patsubst $(LIB_DIR)/%.c,$(LIB_DIR)/%.o, $(LIB_SOURCES))
MAIN = main.c
EXECUTABLE = output

.PHONY: all run clean

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

$(EXECUTABLE): $(MAIN) $(LIB_OBJECTS)
	$(CC) -o $(EXECUTABLE) $(LIB_OBJECTS) $(MAIN)

$(LIB_OBJECTS): $(LIB_DIR)/%.o : $(LIB_DIR)/%.c
	$(CC) -o $@ -c $<

clean:
	rm -f $(LIB_DIR)/*.o $(EXECUTABLE)
