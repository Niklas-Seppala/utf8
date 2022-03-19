CC=gcc
CC_FLAGS=-Wall -Wshadow -std=gnu99 -Iinclude
PROJECT_NAME=utf8

ifdef DEBUG
	CC_FLAGS += -DDEBUG -g
endif
ifdef RT_ASSERT
	CC_FLAGS += -DRT_ASSERT
endif

COMPLETE_PRINT = \033[1;32mSuccess\033[0m\n\nBinary location: ${OUT_DIR}${PROJECT_NAME}\n----------------\n
MODULE_PRINT = \033[0;34m$@\033[0m

OBJ_DIR:=./obj/
OUT_DIR:=./out/
SRC_DIR:=./src/

SRC_FILES := $(wildcard ${SRC_DIR}*.c)
SRC := $(SRC_FILES:${SRC_DIR}%=%)
OBJS := $(SRC:%.c=%.o)
BINS := $(SRC:%.c=%)

build: ${BINS}
	@${CC} ${OBJS} -o ${OUT_DIR}${PROJECT_NAME}
	@mv ./*.o ${OBJ_DIR}
	@echo "$(COMPLETE_PRINT)"

%.o: ${SRC_DIR}%.c
	@${CC} -c ${CC_FLAGS} $< -o $@

%: %.o
	@echo "$(MODULE_PRINT)"

run:
	@./out/${PROJECT_NAME}

clean:
	@rm $(OBJ_DIR)* $(OUT_DIR)* *.o 2>/dev/null || true