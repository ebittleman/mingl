.DEFAULT_GOAL := all

PROJECT_NAME := mingl
THIRD_PARTY := GLAD # GLFW assimp
THIRD_PARTY_LIBS := #glfw3 # assimp # built from source

BUILTIN_LIBS := opengl32 gdi32 m # installed with the base toolchain
EXT_LIBS := ffcall glfw3# installed from package manager
LIBS := $(THIRD_PARTY_LIBS) $(EXT_LIBS)  $(BUILTIN_LIBS) 

SRC_DIR := src
TESTS_DIR := tests
OBJECT_DIR := obj
DIST_DIR := dist
THIRD_PARTY_DIR := third_party

INCLUDE_DIRS := $(patsubst %,-I$(THIRD_PARTY_DIR)/%/include,$(THIRD_PARTY))
LIB_DIRS := $(patsubst %,-L$(THIRD_PARTY_DIR)/%/lib,$(THIRD_PARTY))

SOURCE_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_FILES))
EXE_FILE := $(DIST_DIR)/$(PROJECT_NAME).exe

TEST_FILES := $(wildcard $(TESTS_DIR)/*.c)
TEST_OBJ := $(patsubst $(TESTS_DIR)/%.c,$(TESTS_DIR)/%.o,$(TEST_FILES))
# TEST_EXES := $(patsubst $(TESTS_DIR)/%.c,$(TESTS_DIR)/%.exe,$(TEST_FILES))

override CFLAGS += $(INCLUDE_DIRS)
override LDFLAGS += $(LIB_DIRS)
override LDLIBS += $(patsubst %,-l%, $(LIBS))

BUILT_FILES := $(OBJECT_DIR) $(DIST_DIR) .depend

.PHONY: depend
depend: .depend

.depend: $(SOURCE_FILES)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ | sed '/^ /!s/\(.*\)/$(OBJECT_DIR)\/\1/' > "$@"

include .depend

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

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJECT_DIR)
	$(COMPILE.c) -o $@ $<

$(EXE_FILE): $(OBJECT_FILES) | $(DIST_DIR)
	$(LINK.c) -o $@ $^ $(LDLIBS)

TEST_DEP_OBJ := $(filter-out obj/main.o,$(OBJECT_FILES))

$(TESTS_DIR)/%.o: $(TESTS_DIR)/%.c
	$(COMPILE.c) -Isrc -o $@ $<

TEST_EXES := $(TESTS_DIR)/test.exe

$(TESTS_DIR)/%.exe: $(TEST_OBJ) $(TEST_DEP_OBJ) |  $(EXE_FILE)
	$(LINK.c) -Isrc -o $@ $^ $(LDLIBS)

.PHONY: test
test: $(TEST_EXES)
	@./$< && (rm -f $<; echo Tests Succeeded.) || (echo "Tests Failed"; rm -f $<)

$(DIST_DIR):
	mkdir -p $@

$(OBJECT_DIR):
	mkdir -p $@