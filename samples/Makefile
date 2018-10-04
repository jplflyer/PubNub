#
# This builds two example programs.
#

USE_PROXY = 1
USE_DNS_SERVERS = 1
RECEIVE_GZIP_RESPONSE = 1

OBJ=obj
LIB=libpubnub.a

CFLAGS = -I ../openssl -I/usr/local/include/pubnub -Wall -D PUBNUB_THREADSAFE -D PUBNUB_LOG_LEVEL=PUBNUB_LOG_LEVEL_WARNING -D PUBNUB_CALLBACK_API


OS := $(shell uname)
ifeq ($(OS),Darwin)
	LDLIBS = -lpubnub -lpthread -lssl -lcrypto -L/usr/local/opt/openssl/lib
	CFLAGS += -I/usr/local/opt/openssl/include
else
	LDLIBS = -lpubnub -lrt -lpthread -lssl -lcrypto
endif

PROGRAMS= pubnub_callback_cpp11_sample futres_nesting_callback_cpp11

#======================================================================
# Top-level targets: all, clean.
#======================================================================
.PHONY: all
all: ${OBJ} ${PROGRAMS}

clean:
	rm -rf ${OBJ} ${PROGRAMS}

#======================================================================
# Setup -- building the obj directory
#======================================================================
${OBJ}:
	mkdir -p ${OBJ}

#======================================================================
# Building .o files from their .c or .cpp file.
#======================================================================
obj/%.o : %.cpp
		$(CXX) -c -o $@ --std=c++11 $(CFLAGS) -x c++ $<

#======================================================================
# Building the samples
#======================================================================

pubnub_callback_cpp11_sample: ${OBJ}/pubnub_sample.o
	$(CXX) -o $@ $(CFLAGS) ${OBJ}/pubnub_sample.o $(LDLIBS)

futres_nesting_callback_cpp11: ${OBJ}/futres_nesting.o
	$(CXX) -o $@ $(CFLAGS) ${OBJ}/futres_nesting.o $(LDLIBS)
