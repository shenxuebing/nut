#!/user/bin/env make

CC = g++
LD = gcc
AR = ar

# variables
SRC_ROOT = ../../src/nut
OBJ_ROOT = obj/nut
THIS = nut.mk

# INC
INC += -I../../src

# LIB
HOST = $(shell uname -s)
ifeq (${HOST}, Darwin)
	LIB += -lstdc++
else
	LIB += -lpthread
endif

# CC_FLAGS
ifneq (${HOST}, Darwin)
	CC_FLAGS += -rdynamic
endif
CC_FLAGS += -Wall -g -std=c++11

# OBJS, DEPS
DIRS = $(shell find ${SRC_ROOT} -maxdepth 10 -type d)
CPPS = $(foreach dir,${DIRS},$(wildcard $(dir)/*.cpp))
OBJS = $(patsubst ${SRC_ROOT}%.cpp,${OBJ_ROOT}%.o,${CPPS})
DEPS = ${OBJS:.o=.d}

# TARGET
TARGET = $(CURDIR)/libnut.a

# mkdirs
$(shell mkdir -p $(patsubst ${SRC_ROOT}%,${OBJ_ROOT}%,${DIRS}))

.PHONY: all clean rebuild dirs

all: dirs ${TARGET}

clean:
	rm -rf ${OBJS}
	rm -rf ${DEPS}
	rm -rf ${TARGET}

rebuild: clean all

dirs:
	@mkdir -p $(patsubst ${SRC_ROOT}%,${OBJ_ROOT}%,${DIRS})

$(TARGET): ${OBJS} ${THIS}
	$(AR) cqs $@ ${OBJS}

${OBJ_ROOT}/%.o: ${SRC_ROOT}/%.cpp
	$(CC) ${INC} ${DEF} ${CC_FLAGS} -c $< -o $@

## 动态生成依赖关系
# %.d: %.cpp
${OBJ_ROOT}/%.d:${SRC_ROOT}/%.cpp
	@rm -f $@
	@# 向 *.d 中写入 "xx/xx/*.o xx/xx/*.d:\" 这样一个字符串
	@echo '$@ $@.o:\\' | sed 's/[.]d[.]o/.o/g' > $@
	@# 向 *.d 中写入 makefile 依赖
	@echo '${THIS} \\' >> $@
	@# 向 *.d.$ 中写入用 gcc -MM 生成的初始依赖关系
	$(CC) ${INC} ${DEF} -MM $< > $@.$$
	@# 将 *.d.$ 中内容去除冒号前的内容，剩余内容写入 *.d 中
	@sed 's/^.*[:]//g' < $@.$$ >> $@
	@echo '${THIS}:' >> $@
	@# 对 *.d.$ 的内容以此处理：
	@#	sed 去除冒号前的内容
	@#	sed 去除续行符
	@#	fmt 每个连续单词作为单独一行
	@#	sed 去除行首空白
	@#	sed 行尾添加冒号
	@sed -e 's/.*://' -e 's/\\$$//' < $@.$$ | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $@
	@rm -f $@.$$

# 引入动态依赖关系
#	起首的'-'符号表示忽略错误命令(这里忽略不存在的文件，不再打warning)
-include ${DEPS}
