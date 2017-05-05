FROM alpine:latest
ADD lightjbl.tar.gz /
RUN apk update && apk add gcc libc-dev musl-dev make
RUN \
  cd /lightjbl && \
  ./configure --prefix=/_install LDFLAGS="-static" && \
  make LDFLAGS="-all-static" && \
  make install-strip && \
  cp /_install/bin/lightjbl /lightjbl-bin
