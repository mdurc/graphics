# usage:
# 	make
# 	make rebuild
# 	make clean

# -- config --
CC								:= gcc
CXX								:= g++
PROGRAM						:= a.out

SRC_DIR						:= src
BIN_DIR						:= bin
LIB_DIR						:= lib
ENGINE_DIR				:= $(SRC_DIR)/engine
ENGINE_HEADER			:= $(ENGINE_DIR)/engine.h
ENGINE_LIB				:= $(BIN_DIR)/libengine.a

UNAME_S						:= $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	OS := Apple
else ifeq ($(UNAME_S),Linux)
	OS := Linux
else
	OS := Windows
endif

# cimgui library setup, requires cpp files and cpp compilation/linking
CIMGUI_FREETYPE		?= 0
CIMGUI_DIR				:= $(LIB_DIR)/cimgui
IMGUI_DIR					:= $(CIMGUI_DIR)/imgui
CIMGUI_BACKENDS		:= $(shell if [ -d "$(IMGUI_DIR)/backends" ]; then \
										 echo "$(IMGUI_DIR)/backends"; else \
										 echo "$(IMGUI_DIR)/examples"; fi)
CIMGUI_LIB				:= $(BIN_DIR)/libcimgui.a
CIMGUI_SRC_FILES	:= $(CIMGUI_DIR)/cimgui.cpp \
										 $(IMGUI_DIR)/imgui.cpp \
										 $(IMGUI_DIR)/imgui_draw.cpp \
										 $(IMGUI_DIR)/imgui_demo.cpp \
										 $(IMGUI_DIR)/imgui_widgets.cpp \
										 $(IMGUI_DIR)/imgui_tables.cpp \
										 $(CIMGUI_BACKENDS)/imgui_impl_opengl3.cpp \
										 $(CIMGUI_BACKENDS)/imgui_impl_glfw.cpp
DEFINES						:= -DIMGUI_IMPL_OPENGL_LOADER_GLAD \
										 -DCIMGUI_USE_OPENGL3 \
										 -DCIMGUI_USE_GLFW
ifeq ($(OS),Windows)
	DEFINES += -DIMGUI_IMPL_API='extern "C" __declspec(dllexport)'
else
	DEFINES += -DIMGUI_IMPL_API='extern "C"'
endif
# ----

# -- compilation flags --
WARNINGS					:= -Wall -Wextra -Wshadow -Wstrict-prototypes \
										 -Wfloat-equal -Wmissing-declarations -Wmissing-include-dirs \
										 -Wmissing-prototypes -Wredundant-decls -Wunreachable-code
CFLAGS						:= $(WARNINGS) -g -MMD -MP `pkg-config --cflags glfw3` -DCLIB_TIME_GLFW
CXXFLAGS					:= -std=c++11 -g -MMD -MP `pkg-config --cflags glfw3` -DCLIB_TIME_GLFW

# -isystem instead of -I to avoid compiler warnings on external libraries
INCFLAGS					:= $(addprefix -isystem,$(LIB_DIR)) \
										 -isystem$(CIMGUI_DIR) -isystem$(IMGUI_DIR) -isystem$(CIMGUI_BACKENDS)

LDFLAGS						:= `pkg-config --libs glfw3` -lm
ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework OpenGL
else
	LDFLAGS += -lGL
endif

ifeq ($(CIMGUI_FREETYPE),1)
	CIMGUI_SRC_FILES += $(IMGUI_DIR)/misc/freetype/imgui_freetype.cpp
	DEFINES 				+= -DIMGUI_ENABLE_FREETYPE -DIMGUI_ENABLE_STB_TRUETYPE=1
	LDFLAGS 				+= `pkg-config --libs freetype2`
endif

ENGINE_SRC_FILES	:= $(shell find $(ENGINE_DIR) -name '*.c')
ENGINE_OBJ_FILES	:= $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(ENGINE_SRC_FILES))

SRC_FILES					:= $(wildcard $(SRC_DIR)/*.c) \
										 lib/glad.c
OBJ_FILES					:= $(patsubst $(LIB_DIR)/%.c,$(BIN_DIR)/%.o, \
										 $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRC_FILES)))
CIMGUI_OBJ_FILES	:= $(patsubst %.cpp,$(BIN_DIR)/%.o,$(CIMGUI_SRC_FILES))
DEP_FILES					:= $(patsubst %.o,%.d,$(OBJ_FILES) $(CIMGUI_OBJ_FILES))

# -- rules --
all: $(PROGRAM)

# link final program with c++ compiler due to using cimgui now
$(PROGRAM): $(OBJ_FILES) $(CIMGUI_LIB) $(ENGINE_LIB)
	$(CXX) $(OBJ_FILES) -o $@ $(CIMGUI_LIB) $(ENGINE_LIB) $(LDFLAGS)

# create/maintain the library archive for the static cimgui library
$(CIMGUI_LIB): $(CIMGUI_OBJ_FILES)
	ar rcs $@ $^

$(ENGINE_LIB): $(ENGINE_OBJ_FILES)
	ar rcs $@ $^

# project C source files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCFLAGS) $(DEFINES) -c $< -o $@

# library/external C source files
$(BIN_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCFLAGS) $(DEFINES) -c $< -o $@

# cimgui c++ source files
$(BIN_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(DEFINES) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(PROGRAM)

rebuild: clean all

# --- Installation ---
PREFIX?=/usr/local
INSTALL_LIB_DIR = $(PREFIX)/lib
INSTALL_INCLUDE_DIR = $(PREFIX)/include

install: $(ENGINE_LIB)
	sudo mkdir -p $(INSTALL_LIB_DIR)
	sudo mkdir -p $(INSTALL_INCLUDE_DIR)
	sudo cp $(ENGINE_HEADER) $(INSTALL_INCLUDE_DIR)
	sudo cp $(ENGINE_LIB) $(INSTALL_LIB_DIR)

uninstall:
	sudo rm -f $(INSTALL_INCLUDE_DIR)/engine.h $(INSTALL_LIB_DIR)/libengine.a

-include $(DEP_FILES)

.PHONY: all clean rebuild install uninstall
