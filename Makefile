SRC_DIR=src
SRC_PATH=$(SRC_DIR)/
SRC_FILES=$(SRC_PATH)main.ruc
SRC_KEYWORDS=$(SRC_PATH)keywords.txt

BUILD_DIR=build
BUILD_PATH=$(BUILD_DIR)/
BUILD_FILE=$(BUILD_PATH)ruc
BUILD_KEYWORDS=$(BUILD_PATH)keywords.txt

CC=$(shell which ruc)
CP=$(shell which cp)
MKDIR=$(shell which mkdir)

all: $(BUILD_FILE) $(BUILD_KEYWORDS)

$(BUILD_KEYWORDS): $(SRC_KEYWORDS)
	$(MKDIR) -p $(dir $@)
	$(CP) $< $@

$(BUILD_FILE): $(SRC_FILES)
	$(MKDIR) -p $(dir $@)
	$(CC) -o $@ $<

