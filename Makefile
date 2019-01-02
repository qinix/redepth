HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXXFLAGS += -g -Ibuild/ -std=c++17
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
VPATH = src:build
BUILDDIR = build

all: dir protogen $(BUILDDIR)/redepth.so

dir:
	mkdir -p $(BUILDDIR)

protogen: $(BUILDDIR)/depth.pb.cc

$(BUILDDIR)/redepth.so: $(BUILDDIR)/depth.o $(BUILDDIR)/depth.pb.o $(BUILDDIR)/price_level.o $(BUILDDIR)/redepth.o

$(BUILDDIR)/%.pb.cc: %.proto
	$(PROTOC) --cpp_out=$(BUILDDIR) $<

$(BUILDDIR)/%.so:
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: %.cc
	$(CXX) -o $@ $< $(CXXFLAGS) -c

clean:
	rm -rf build 2> /dev/null || true
