CC = gcc
CFLAGS = -O2 -I/opt/homebrew/Cellar/openssl@3/3.2.0_1/include
LDFLAGS = -L/opt/homebrew/Cellar/openssl@3/3.2.0_1/lib
LIBS = -lcrypto

SRC_FILES = common.c encap.c gf_poly.c gf_tab_gen.c gf.c goppa_instance.c key_gen.c mat_mul.c PALOMA.c rng.c decoding.c decrypt.c lsh.c lsh512.c decap.c encrypt.c a.c bench.c
EXECUTABLE = executable_file

PALOMA_MODE ?= 2
WORD ?= 64
BENCH_MODE ?= 1

ifeq ($(PALOMA_MODE),0)
    CFLAGS += -DPALOMA_MODE=0
else ifeq ($(PALOMA_MODE),1)
    CFLAGS += -DPALOMA_MODE=1
else ifeq ($(PALOMA_MODE),2)
    CFLAGS += -DPALOMA_MODE=2
else
    $(error Invalid value for PALOMA_MODE. Please use 0, 1, or 2.)
endif

ifeq ($(WORD),8)
    CFLAGS += -DWORD=8
else ifeq ($(WORD),32)
    CFLAGS += -DWORD=32
else ifeq ($(WORD),64)
    CFLAGS += -DWORD=64
else
    $(error Invalid value for WORD. Please use 8, 32, or 64.)
endif

ifeq ($(BENCH_MODE),0)
    CFLAGS += -DBENCH_MODE=0
else ifeq ($(BENCH_MODE),1)
    CFLAGS += -DBENCH_MODE=1
else
    $(error Invalid value for BENCH_MODE. Please use 0 or 1.)
endif

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(EXECUTABLE)