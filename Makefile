TARGET     = bin/game
SRC_DIR    = ${.CURDIR}/src
OBJ_DIR    = ${.CURDIR}/obj
BIN_DIR    = ${.CURDIR}/bin

CXX        = clang++
CXXFLAGS   = -std=c++20 -Wall -Wextra -O2 \
-I${SRC_DIR} \
-I${SRC_DIR}/managers \
-I${SRC_DIR}/core \
-I${SRC_DIR}/scenes \
-I/usr/local/include
LDFLAGS    = -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system

SRCS != find ${SRC_DIR} -name '*.cpp'
OBJS = ${SRCS:S|${SRC_DIR}|${OBJ_DIR}|:S|.cpp|.o|}

.PHONY: all clean run dirs

all: dirs ${TARGET}

dirs:
	@mkdir -p ${OBJ_DIR}
	@mkdir -p ${BIN_DIR}

${TARGET}: ${OBJS} dirs
	@echo "linking $@"
	${CXX} -o $@ ${OBJS} ${LDFLAGS}

.for src in ${SRCS}
objfile = ${src:S|${SRC_DIR}|${OBJ_DIR}|:S|.cpp|.o|}
${objfile}: ${src}
	@mkdir -p ${.TARGET:H}
	@echo "compiling ${src}"
	${CXX} ${CXXFLAGS} -c ${src} -o ${.TARGET}
.endfor

clean:
	rm -rf ${OBJ_DIR} ${TARGET}

run: ${TARGET}
	./${TARGET}
