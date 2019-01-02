HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXX = clang++
CPPFLAGS += `pkg-config --cflags protobuf`
CXXFLAGS += -std=c++17
CXXFLAGS += -g
CXXFLAGS += -I.
LDFLAGS += -lc++ -L/usr/local/lib `pkg-config --libs-only-l --libs-only-L protobuf` -lboost_date_time -lboost_system -ldl

ifeq ($(SYSTEM),Darwin)
LDFLAGS += -lSystem  -bundle -undefined dynamic_lookup
else
LDFLAGS += -Wl,--no-as-needed -Wl,--as-needed -shared
endif

PROTOC = protoc

all: redepth.so

redepth.so: depth.o depth.pb.o price_level.o redepth.o

%.pb.cc: %.proto
	$(PROTOC) --cpp_out=. $<

%.so:
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf *.so *.o *.pb.cc *.pb.h  2> /dev/null || true
