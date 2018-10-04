#
# This builds libpubnub.a. It is based on the openssl makefile.
#

USE_PROXY = 1
USE_DNS_SERVERS = 1
RECEIVE_GZIP_RESPONSE = 1

OBJ=obj
LIB=libpubnub.a

# This list must cover the obj directory plus any subdirectories that match things from SOURCEFILES below.
OBJDIRS=${OBJ} ${OBJ}/core ${OBJ}/lib ${OBJ}/lib/base64 ${OBJ}/lib/md5 ${OBJ}/lib/miniz ${OBJ}/lib/sockets ${OBJ}/openssl ${OBJ}/posix

# -g enables debugging, remove to get a smaller executable
CFLAGS =-g -I .. -I . -I ../openssl -Wall -D PUBNUB_THREADSAFE -D PUBNUB_LOG_LEVEL=PUBNUB_LOG_LEVEL_WARNING -D PUBNUB_CALLBACK_API

CPPFILES=pubnub_futres_cpp11.cpp

# These paths are all relative from ..
SOURCEFILES = \
	core/pbgzip_decompress.c \
	core/pbhttp_digest.c \
	core/pbntlm_core.c \
	core/pbntlm_packer_std.c \
	core/pbpal_ntf_callback_queue.c \
	core/pbpal_ntf_callback_admin.c \
	core/pbpal_ntf_callback_handle_timer_list.c \
	core/pubnub_alloc_std.c \
	core/pubnub_assert_std.c \
	core/pubnub_blocking_io.c \
	core/pubnub_callback_subscribe_loop.c \
	core/pubnub_ccore.c \
	core/pubnub_ccore_pubsub.c \
	core/pubnub_coreapi.c \
	core/pubnub_coreapi_ex.c \
	core/pubnub_crypto.c \
	core/pubnub_dns_servers.c \
	core/pubnub_free_with_timeout_std.c \
	core/pubnub_generate_uuid.c \
	core/pubnub_helper.c \
	core/pubnub_json_parse.c \
	core/pubnub_netcore.c \
	core/pubnub_proxy.c \
	core/pubnub_proxy_core.c \
	core/pubnub_pubsubapi.c \
	core/pubnub_timer_list.c \
	core/pubnub_timers.c \
	lib/pubnub_parse_ipv4_addr.c \
	lib/base64/pbbase64.c \
	lib/md5/md5.c \
	lib/miniz/miniz_tinfl.c \
	lib/sockets/pbpal_adns_sockets.c \
	lib/sockets/pbpal_ntf_callback_poller_poll.c \
	lib/sockets/pbpal_resolv_and_connect_sockets.c \
	openssl/pbaes256.c \
	openssl/pbpal_add_system_certs_posix.c \
	openssl/pbpal_connect_openssl.c \
	openssl/pbpal_openssl.c \
	openssl/pbpal_openssl_blocking_io.c \
	openssl/pubnub_get_native_socket.c \
	openssl/pubnub_ntf_callback_posix.c \
	openssl/pubnub_version_openssl.c \
	posix/posix_socket_blocking_io.c \
	posix/pubnub_dns_system_servers.c \
	posix/pubnub_generate_uuid_posix.c

OS := $(shell uname)
ifeq ($(OS),Darwin)
	# Relative from ..
	SOURCEFILES += posix/monotonic_clock_get_time_darwin.c
	LDLIBS=-lpthread -lssl -lcrypto -L/usr/local/opt/openssl/lib
	CFLAGS += -I/usr/local/opt/openssl/include
else
	# Relative from ..
	SOURCEFILES += posix/monotonic_clock_get_time_posix.c
	LDLIBS=-lrt -lpthread -lssl -lcrypto
endif

SAMPLES= openssl/pubnub_callback_cpp11_sample openssl/futres_nesting_callback_cpp11

OBJECTS=$(patsubst %.c,${OBJ}/%.o,$(SOURCEFILES)) $(patsubst %.cpp,${OBJ}/%.o,$(CPPFILES))

#======================================================================
# Top-level targets: all, clean.
#======================================================================
.PHONY: all
all: ${OBJDIRS} ${LIB}

clean:
	rm -rf ${PROGRAMS} ${OBJ} ${LIB}

doecho:
	@echo OBJECTS is ${OBJECTS}

#======================================================================
# Setup -- building the obj directory
#======================================================================
${OBJ}:
	mkdir -p ${OBJ}

${OBJ}/core:
	mkdir -p ${OBJ}/core

${OBJ}/lib:
	mkdir -p ${OBJ}/lib

${OBJ}/lib/base64:
	mkdir -p ${OBJ}/lib/base64

${OBJ}/lib/md5:
	mkdir -p ${OBJ}/lib/md5

${OBJ}/lib/miniz:
	mkdir -p ${OBJ}/lib/miniz

${OBJ}/lib/sockets:
	mkdir -p ${OBJ}/lib/sockets

${OBJ}/openssl:
	mkdir -p ${OBJ}/openssl

${OBJ}/posix:
	mkdir -p ${OBJ}/posix

#======================================================================
# Building .o files from their .c or .cpp file.
#======================================================================

${LIB}: ${OBJECTS}
	ar -cr ${LIB} ${OBJECTS}

obj/%.o : ../%.c
		$(CXX) -c -o $@ --std=c++11 $(CFLAGS) -x c++ $<

obj/%.o : %.cpp
		$(CXX) -c -o $@ --std=c++11 $(CFLAGS) -x c++ $<

obj/%.o : samples/%.cpp
		$(CXX) -c -o $@ --std=c++11 $(CFLAGS) -x c++ $<

#======================================================================
# Building the samples
#======================================================================
samples: ${SAMPLES}

openssl/pubnub_callback_cpp11_sample: ${OBJ}/pubnub_sample.o ${LIB}
	$(CXX) -o $@ $(CFLAGS) ${OBJ}/pubnub_sample.o -L. -lpubnub $(LDLIBS)

openssl/futres_nesting_callback_cpp11: ${OBJ}/futres_nesting.o $(LIB)
	$(CXX) -o $@ $(CFLAGS) ${OBJ}/futres_nesting.o -L. -lpubnub $(LDLIBS)

#======================================================================
# Installation into /usr/local/include/posix and /usr/local/lib
#
# This probably requires "sudo make install"
#======================================================================
install: /usr/local/lib/${LIB} installincludes

/usr/local/lib/${LIB}: ${LIB}
	cp ${LIB} /usr/local/lib/${LIB}

# Note that all the .h files get put into subdirs but the .hpp will be at the top
#	find .. -name '*.h' | cpio -pdm /usr/local/include/pubnub
installincludes:
	mkdir -p /usr/local/include/pubnub
	cd .. && find . -name '*.h' | egrep -v "/samples/|^(qt|freertos|microchip_harmony|windows)" | cpio -pdm /usr/local/include/pubnub
	find . -name '*.hpp' | cpio -pdm /usr/local/include/pubnub
