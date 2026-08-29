# run on windows
# mingw32-make.exe run

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
src/array.h\
src/file.h\
\
src/ast.h\
src/ast.c\
src/log.h\
src/log.c\
src/tokenizer.h\
src/tokenizer.c\
src/string.h\
src/string.c\



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