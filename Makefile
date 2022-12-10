.DEFAULT_GOAL := all

PROJECT_NAME := mingl
THIRD_PARTY := GLAD # GLFW assimp
THIRD_PARTY_LIBS := #glfw3 # assimp # built from source

BUILTIN_LIBS := opengl32 gdi32 m # installed with the base toolchain
EXT_LIBS := ffcall glfw3# installed from package manager
LIBS := $(THIRD_PARTY_LIBS) $(EXT_LIBS)  $(BUILTIN_LIBS) 

SRC_DIR := src
OBJECT_DIR := obj
DIST_DIR := dist
THIRD_PARTY_DIR := third_party

INCLUDE_DIRS := $(patsubst %,-I$(THIRD_PARTY_DIR)/%/include,$(THIRD_PARTY))
LIB_DIRS := $(patsubst %,-L$(THIRD_PARTY_DIR)/%/lib,$(THIRD_PARTY))

SOURCE_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_FILES))
SHADERS_H := $(SRC_DIR)/shaders.h
EXE_FILE := $(DIST_DIR)/$(PROJECT_NAME).exe

override CFLAGS += $(INCLUDE_DIRS)
override LDFLAGS += $(LIB_DIRS)
override LDLIBS += $(patsubst %,-l%, $(LIBS))

BUILT_FILES := $(OBJECT_DIR) $(DIST_DIR)

.PHONY: all
all: $(EXE_FILE)
	@echo build complete

.PHONY: run
run: $(EXE_FILE)
	$<

.PHONY: clean
clean:
	$(RM) -r $(BUILT_FILES)

.PHONY: debug
debug:
	$(foreach var,$(.VARIABLES),$(info $(var) = $($(var))))

# $(SHADERS_H): $(SHARDER_FILES)
# 	./scripts/shader_header.sh $@

$(OBJECT_DIR)/main.o: $(SHADERS_H) src/linmath.h

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJECT_DIR)
	$(COMPILE.c) -o $@ $<

$(EXE_FILE): $(OBJECT_FILES) | $(DIST_DIR)
	$(LINK.c) -o $@ $^ $(LDLIBS)

$(DIST_DIR):
	mkdir -p $@

$(OBJECT_DIR):
	mkdir -p $@

TEST_OBJ := $(filter-out obj/main.o,$(OBJECT_FILES))

test.exe: ./tests/test_obj3d.c $(TEST_OBJ) |  $(EXE_FILE)
	$(LINK.c) -Isrc -o $@ $^ $(LDLIBS)

.PHONY: test
test: test.exe
	@./$< && (rm -f $<; echo Tests Succeeded.) || (echo "Tests Failed"; rm -f $<)

.PHONY: info
info:
	@echo $(LINK.c) -Isrc -o $@ $^ $(LDLIBS)
