CC=gcc
CC_FLAGS=-Wall -Wshadow -std=gnu99 -O3
PROJECT_NAME=utf8

COMPLETE_PRINT=\033[1;32mSuccess\033[0m - Binary location: ${OUT_DIR}${PROJECT_NAME}\n

OBJ_DIR:=./obj/
OUT_DIR:=./out/
SRC_DIR:=./src/

SRC_FILES := $(wildcard ${SRC_DIR}*.c)
SRC := $(SRC_FILES:${SRC_DIR}%=%)
OBJS := $(SRC:%.c=%.o)
BINS := $(SRC:%.c=%)

build: ${BINS}
	@echo "\nBuilding project - ${PROJECT_NAME}"
	@${CC} ${OBJS} -o ${OUT_DIR}${PROJECT_NAME}
	@mv ./*.o ${OBJ_DIR}
	@echo "$(COMPLETE_PRINT)"

%.o: ${SRC_DIR}%.c
	@${CC} -c ${CC_FLAGS} $< -o $@

%: %.o
	
run:
	@echo -n "😀 😁 🤣" > ${OUT_DIR}test.txt
	@${OUT_DIR}${PROJECT_NAME} ${OUT_DIR}test.txt

clean:
	@rm $(OBJ_DIR)* $(OUT_DIR)* *.o 2>/dev/null || true