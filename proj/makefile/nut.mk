#!/user/bin/env make

TARGET_NAME = nut
SRC_ROOT = ../../src/${TARGET_NAME}

# preface rules
include preface_rules.mk

# INC
INC += -I${SRC_ROOT}/..

# DEF
DEF += -DBUILDING_NUT_DLL

# CXX_FLAGS
CXX_FLAGS += -std=c++11

# LIB LIB_DEPS
LIB_DEPS +=
ifeq (${HOST}, Linux)
	LIB += -lpthread -ldl
endif

# LD_FLAGS
LD_FLAGS +=

# TARGET
ifeq (${HOST}, Darwin)
	TARGET = ${OUT_DIR}/lib${TARGET_NAME}.dylib
else
	TARGET = ${OUT_DIR}/lib${TARGET_NAME}.so
endif

.PHONY: all clean

all: ${TARGET}

clean:
	rm -rf ${OBJS}
	rm -rf ${DEPS}
	rm -rf ${TARGET}

rebuild:
	# 顺序执行，不会并行
	make -f nut.mk clean
	make -f nut.mk all

# rules
include common_rules.mk
include shared_rules.mk
