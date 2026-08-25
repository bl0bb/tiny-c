CC := gcc

ERROR_FLAGS := \
# -Wall\
# -Wextra\
# -Werror\
# -pedantic

C_FILES := \
src/main.c\
\
src/types.h\
src/file.h\
\
src/log.h\
src/log.c\
src/tokenizer.h\
src/tokenizer.c\



FILES := $(C_FILES)

BUILD_DIR := build



# windows
LIBS :=


$(BUILD_DIR):
	mkdir $(BUILD_DIR)

build-comp: $(BUILD_DIR)
	$(CC) $(ERROR_FLAGS) $(FILES) -o $(BUILD_DIR)/main $(LIBS)

run: build-comp
	.\$(BUILD_DIR)\main.exe