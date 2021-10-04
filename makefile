BINARY_NAME = thread_safe_vec
AUTHOR = Nicholas Warfield

SOURCE_DIR = src
OBJECT_DIR = src/obj
INCLUDE_DIR = include
LIBRARY_DIR = lib
RESOURCE_DIR = resources
BUILD_DIR = build
TEST_DIR = tests

CC = ccache clang++
OPT = O2

CFLAGS = -std=c++17 -$(OPT) -pthread -Wall -Wextra -Wno-missing-braces -DDEBUG
LDFLAGS =

CFLAGS += -I$(INCLUDE_DIR) -I$(LIBRARY_DIR)

HEADERS = $(wildcard *, $(INCLUDE_DIR)/*.hpp)
SOURCE = $(wildcard *, $(SOURCE_DIR)/*.cpp)
OBJECTS := $(patsubst %.cpp,%.o, $(notdir $(SOURCE)))
OBJECTS := $(addprefix $(OBJECT_DIR)/, $(OBJECTS))

TEST_SOURCE = $(wildcard *, $(TEST_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst %.cpp,%.o, $(notdir $(TEST_SOURCE)))
TEST_OBJECTS := $(addprefix $(TEST_DIR)/obj/, $(TEST_OBJECTS))
TEST_OBJECTS += $(OBJECTS)
TEST_OBJECTS := $(filter-out $(OBJECT_DIR)/main.o, $(TEST_OBJECTS))

RUN_ARGS =
ifeq (run,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
else ifeq (test,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/$(BINARY_NAME): init-build $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS) $(LDFLAGS)

$(TEST_DIR)/obj/%.o: $(TEST_DIR)/%.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TEST_DIR)/.test: init-tests $(TEST_OBJECTS)
	$(CC) -o $@ $(TEST_OBJECTS) $(CFLAGS) $(LDFLAGS)

.PHONY: run clean test init-tests init-build all

all: $(BUILD_DIR)/$(BINARY_NAME) $(TEST_DIR)/.test

run: $(BUILD_DIR)/$(BINARY_NAME)
	@./$(BUILD_DIR)/$(BINARY_NAME)

clean:
	@[ ! -d $(OBJECT_DIR) ] || trash $(OBJECT_DIR)
	@[ ! -d $(BUILD_DIR) ] || trash $(BUILD_DIR)
	@[ ! -d $(TEST_DIR)/obj ] || trash $(TEST_DIR)/obj
	@[ ! -e $(TEST_DIR)/.test ] || trash $(TEST_DIR)/.test

test: $(TEST_DIR)/.test
	@./$(TEST_DIR)/.test $(RUN_ARGS)

init-tests:
	@[ -d $(TEST_DIR)/obj ] || mkdir -p $(TEST_DIR)/obj

init-build:
	@[ -d $(OBJECT_DIR) ] || mkdir -p $(OBJECT_DIR)
	@[ -d $(BUILD_DIR)  ] || mkdir -p $(BUILD_DIR)
	@[[ !(! -d $(BUILD_DIR)/$(RESOURCE_DIR) && -d $(RESOURCE_DIR)) ]] \
		|| (ln -s "$(realpath $(RESOURCE_DIR))" $(BUILD_DIR); \
		echo "$(RESOURCE_DIR) linked to $(BUILD_DIR)")

