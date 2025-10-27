#!/usr/bin/env make

# Directory containing this Makefile (absolute)
MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Allow overriding important paths from environment or command line.
BNO055_DIR ?= $(abspath $(MAKEFILE_DIR)/../../Sensors/BNO055)
TEST_DIR ?= $(abspath $(MAKEFILE_DIR))
OBJDIR ?= $(TEST_DIR)/build

# Compiler and flags
CC ?= gcc
CFLAGS ?= -std=c99

# Host include header (file to be passed to -include). Default placed in test dir.
HOST_INCLUDE ?= host_includes.h
HOST_INCLUDE_PATH ?= $(TEST_DIR)/$(HOST_INCLUDE)

# Include directories (users can override or add more via EXTRA_INCLUDES)
INCLUDE_DIRS ?= -I$(TEST_DIR) -I$(TEST_DIR)/..
EXTRA_INCLUDES ?=

# Compose final CFLAGS
CFLAGS += -include $(HOST_INCLUDE_PATH) $(INCLUDE_DIRS) $(EXTRA_INCLUDES)

# Sources (absolute paths)
SRCS := $(TEST_DIR)/main.c \
    $(BNO055_DIR)/bno055_mock_driver.c \
    $(BNO055_DIR)/bno055_api.c

# Object files live in $(OBJDIR) and are named after the source basenames
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(SRCS)))

TARGET ?= bno055_test

.PHONY: all build run clean

all: build

build: $(OBJDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Per-source compile rules so object files are built from their
# actual source locations (works when sources are in different directories).
define make-compile-rule
$(OBJDIR)/$(notdir $(1:.c=.o)): $(1)
	$(CC) $(CFLAGS) -c -o $$@ $$<
endef
$(foreach src,$(SRCS),$(eval $(call make-compile-rule,$(src))))

run: build
	./$(TARGET)

clean:
	rm -rf $(TARGET) $(OBJDIR)
