CC = clang
CFLAGS = -Wall -Wextra -O2 -I./src -fPIC
OBJCFLAGS = -Wall -Wextra -O2 -I./src -fPIC
FRAMEWORKS = -framework Foundation -framework AppKit -framework IOKit -framework CoreGraphics -framework AudioToolbox -framework CoreAudio -framework SystemConfiguration

SRC_DIR = src
BIN_DIR = bin
LIB_DIR = lib

SRCS_C = $(SRC_DIR)/config.c $(SRC_DIR)/server.c
SRCS_M = $(SRC_DIR)/system_macos.m
OBJS = $(SRCS_C:.c=.o) $(SRCS_M:.m=.o)

TARGET_SERVER = $(BIN_DIR)/tiwut-api-server
TARGET_DYLIB = $(LIB_DIR)/libtiwut_api.dylib
TARGET_A = $(LIB_DIR)/libtiwut_api.a

all: $(BIN_DIR) $(LIB_DIR) $(TARGET_SERVER) $(TARGET_DYLIB) $(TARGET_A) build-cfeel

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.m
	$(CC) $(OBJCFLAGS) -c $< -o $@

$(TARGET_SERVER): $(OBJS) $(SRC_DIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(FRAMEWORKS)

$(TARGET_DYLIB): $(OBJS)
	$(CC) -dynamiclib -o $@ $^ $(FRAMEWORKS)

$(TARGET_A): $(OBJS)
	ar rcs $@ $^

build-cfeel:
	$(MAKE) -C external/cfeel

samples: all
	mkdir -p sample/bin
	external/cfeel/cfeelc -o sample/bin/cfeel_monitor -L./lib -ltiwut_api $(FRAMEWORKS) sample/cfeel/monitor.cfeel
	external/cfeel/cfeelc -o sample/bin/cfeel_process -L./lib -ltiwut_api $(FRAMEWORKS) sample/cfeel/process_manager.cfeel
	external/cfeel/cfeelc -o sample/bin/cfeel_power -L./lib -ltiwut_api $(FRAMEWORKS) sample/cfeel/power_control.cfeel
	external/cfeel/cfeelc -o sample/bin/cfeel_notifier -L./lib -ltiwut_api $(FRAMEWORKS) sample/cfeel/notifier.cfeel
	external/cfeel/cfeelc -o sample/bin/cfeel_bench -L./lib -ltiwut_api $(FRAMEWORKS) sample/cfeel/bench.cfeel
	clang++ -std=c++17 -I./src -I./external/CPP-Headers sample/cpp/main.cpp -L./lib -ltiwut_api $(FRAMEWORKS) -o sample/bin/cpp_sample

clean:
	bash clean.sh

.PHONY: all build-cfeel samples clean
