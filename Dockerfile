FROM redis:latest as builder

# Set up a build environment
RUN apt-get update
RUN apt-get install -y --no-install-recommends build-essential m4 libprotobuf-dev protobuf-compiler libboost-date-time-dev libboost-system-dev

ADD . /redepth
WORKDIR /redepth
RUN make clean
RUN make

# Package the runner
FROM redis:latest
ENV LIBDIR /usr/lib/redis/modules
WORKDIR /data
RUN apt-get update
RUN apt-get install -y --no-install-recommends libprotobuf10 libboost-date-time-dev libboost-system-dev
RUN mkdir -p "$LIBDIR";

COPY --from=builder /redepth/build/redepth.so "$LIBDIR"

EXPOSE 6379
CMD ["redis-server", "--loadmodule", "/usr/lib/redis/modules/redepth.so"]