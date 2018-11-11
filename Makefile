CC=gcc
CFLAGS=-Wall -pthread
DEBUG=-g

INCLUDES=-I /usr/include/json-c -I /usr/include/mysql -I /usr/include/postgresql
#~ INCLUDES=-I /usr/include/json-c -I /usr/include/mariadb -I /usr/include/postgresql
LFLAGS=-L /usr/lib
LIBS=-l json-c -l mysqlclient -l pq
#~ LIBS=-l json-c -l mariadb -l pq

DEBUG_DIR=debug/
OBJ_DIR=object/
RUN_DIR=run/

PRJ_NOM=Servidor-Switch
SOURCE=${PRJ_NOM}.c
TARGET=${RUN_DIR}${PRJ_NOM}.exe
DEBUG_TARGET=${DEBUG_DIR}${PRJ_NOM}.exe
OBJ_SRC=${SRV_OBJ} ${JSN_OBJ} ${MySQL_OBJ} ${PG_OBJ} ${FB_OBJ}

# Cliente
CLI=Cliente-Switch
CLI_SRC=${CLI}.c
CLI_TARGET=${RUN_DIR}${CLI}.exe
CLI_DEBUG=${DEBUG_DIR}${CLI}.exe

# Servidor
SRV=servidor-concurrente
SRV_SRC=${SRV}.c
SRV_OBJ=${OBJ_DIR}${SRV}.o

# Utilidades JSON
JSN=json-util
JSN_SRC=${JSN}.c
JSN_OBJ=${OBJ_DIR}${JSN}.o

# Módulo MySQL
MySQL=modulo-mysql
MySQL_SRC=${MySQL}.c
MySQL_OBJ=${OBJ_DIR}${MySQL}.o

# Módulo PostgreSQL
PG=modulo-postgres
PG_SRC=${PG}.c
PG_OBJ=${OBJ_DIR}${PG}.o

# Módulo Firebird
FB=modulo-firebird
FB_SRC=${FB}.c
FB_OBJ=${OBJ_DIR}${FB}.o


all: ${RUN_DIR} ${OBJ_DIR} ${TARGET} ${CLI_TARGET}

debug: ${DEBUG_DIR} ${OBJ_DIR} ${DEBUG_TARGET} ${CLI_DEBUG}

${DEBUG_DIR}:
	mkdir ${DEBUG_DIR}

${OBJ_DIR}:
	mkdir ${OBJ_DIR}

${RUN_DIR}:
	mkdir ${RUN_DIR}

${SRV_OBJ}: ${SRV_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${SRV_OBJ} -c ${SRV_SRC} ${LFLAGS} ${LIBS}

${JSN_OBJ}: ${JSN_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${JSN_OBJ} -c ${JSN_SRC} ${LFLAGS} ${LIBS}

${MySQL_OBJ}: ${MySQL_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${MySQL_OBJ} -c ${MySQL_SRC} ${UTIL_OBJ} ${LFLAGS} ${LIBS}

${PG_OBJ}: ${PG_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${PG_OBJ} -c ${PG_SRC} ${LFLAGS} ${LIBS}

${FB_OBJ}: ${FB_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${FB_OBJ} -c ${FB_SRC} ${LFLAGS} ${LIBS}

${TARGET}: ${SOURCE} ${OBJ_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} ${SOURCE} ${OBJ_SRC} ${LFLAGS} ${LIBS}

${DEBUG_TARGET}: ${SOURCE} ${OBJ_SRC}
	${CC} ${CFLAGS} ${DEBUG} ${INCLUDES} -o ${DEBUG_TARGET} ${SOURCE} ${OBJ_SRC} ${LFLAGS} ${LIBS}

${CLI_TARGET}: ${CLI_SRC} ${OBJ_SRC}
	${CC} ${CFLAGS} ${INCLUDES} -o ${CLI_TARGET} ${CLI_SRC} ${OBJ_SRC} ${LFLAGS} ${LIBS}

${CLI_DEBUG}: ${CLI_SRC} ${OBJ_SRC}
	${CC} ${CFLAGS} ${DEBUG} ${INCLUDES} -o ${CLI_DEBUG} ${CLI_SRC} ${OBJ_SRC} ${LFLAGS} ${LIBS}

clean:
	${RM} ${OBJ_DIR}*.* ${RUN_DIR}*.* ${DEBUG_DIR}*.*
