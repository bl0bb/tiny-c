# run on windows
# clear; mingw32-make.exe run
# run on windows with gdb
# clear; mingw32-make.exe build-comp; gdb .\build\main.exe

CC := gcc

ERROR_FLAGS := \
-g \
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
src/codegen.h\
src/codegen.c\
src/log.h\
src/log.c\
src/tokenizer.h\
src/tokenizer.c\
src/string.h\
src/string.c\
src/type.h\
src/type.c\



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