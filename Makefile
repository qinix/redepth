HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXXFLAGS += -g -I. -std=c++17
LDFLAGS += -L/usr/local/lib -lprotobuf -lboost_date_time -lboost_system -ldl

ifeq ($(SYSTEM),Darwin)
LDFLAGS += -lSystem  -bundle -undefined dynamic_lookup -lc++
CXX = clang++
else
CXXFLAGS += -fPIC
LDFLAGS += -Wl,--no-as-needed -Wl,--as-needed -shared -lstdc++
CXX = g++
endif

PROTOC = protoc

all: protogen redepth.so

protogen: depth.pb.cc

redepth.so: depth.o depth.pb.o price_level.o redepth.o

%.pb.cc: %.proto
	$(PROTOC) --cpp_out=. $<

%.so:
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf *.so *.o *.pb.cc *.pb.h  2> /dev/null || true
