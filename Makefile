CC = arm-none-eabi-gcc

TARGET = mmu

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj

SOURCE_FILES =\
	main.c\
	m25p80.c\
	dbg_print.c\

TARGET_FILES = $(TARGET:%=$(BIN_DIR)/%)
OBJECT_FILES = $(SOURCE_FILES:%.c=$(OBJ_DIR)/%.o)

CFLAGS += --std=gnu99
CFLAGS += -c
LDFLAGS += --specs=nano.specs
LDFLAGS += --specs=nosys.specs
LDFLAGS += -lgcc
LDFLAGS += -lc
LDFLAGS += -lm
LDFLAGS += -lnosys

build: $(TARGET_FILES)

.PHONY: build clean

$(TARGET_FILES): $(OBJECT_FILES)
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $^

$(OBJECT_FILES): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -r -f $(BIN_DIR)
