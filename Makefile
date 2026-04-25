CC		:= gcc
CFLAGS  := -Wall -Wextra -Werror -Wstrict-prototypes -Wmissing-prototypes -ansi -pedantic -Iinclude -Isrc
LDFLAGS := -lm -lncurses -lpanel
OBJ_DIR	:= obj
BIN		:= ctm

# --- MODULE DEFINITIONS ---

# Core
CORE_DIR  := src/core
CORE_SRCS := $(wildcard $(CORE_DIR)/*.c) \
             $(wildcard $(CORE_DIR)/linux/*.c)
CORE_OBJS := $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(CORE_SRCS))
CORE_LIB  := libcore.a

# Metrics
METRICS_DIR		:= src/metrics
METRICS_SRCS 	:= $(wildcard $(METRICS_DIR)/*.c)
METRICS_OBJS	:= $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(METRICS_SRCS))
METRICS_LIB		:= libmetrics.a

# UI
UI_TYPE		?= tui
UI_DIR =	:= src/ui/$(UI_TYPE)
UI_SRCS		:= $(wildcard $(UI_DIR)/*.c)
UI_OBJS		:= $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(UI_SRCS))
UI_LIB		:= libui.a

# --- BUILD RULES ---

all: $(BIN)

# Link
$(BIN): src/main.o $(UI_LIB) $(METRICS_LIB) $(CORE_LIB)
	$(CC) $^ -o $@ $(LDFLAGS)

$(CORE_LIB): $(CORE_OBJS)
	ar rcs $@ $^

$(METRICS_LIB): $(METRICS_OBJS)
	ar rcs $@ $^

UI_TYPE ?= tui
$(UI_LIB): $(UI_OBJS)
	ar rcs $@ $^

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean all core metrics ui

core: $(CORE_LIB)
metrics: $(METRICS_LIB)
ui: $(UI_LIB)

clean:
	rm -rf $(OBJ_DIR) $(BIN) *.a