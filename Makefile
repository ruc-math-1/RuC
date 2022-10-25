SRC_DIR=src
SRC_PATH=$(SRC_DIR)/
SRC_FILES=$(SRC_PATH)main.ruc

BUILD_DIR=build
BUILD_PATH=$(BUILD_DIR)/
BUILD_FILE=$(BUILD_PATH)ruc

CC=$(shell which ruc)

all: $(BUILD_FILE) 

$(BUILD_FILE): $(SRC_FILES)
	mkdir -p $(dir $@)
	$(CC) -o $@ $<

