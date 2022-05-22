FROM alpine:3.15

RUN apk add --update \
    make \
    nasm \
    upx \
    zip \
  && rm -rf /var/cache/apk/*

# ADD opwatcom.tar.xz /opt
ADD open_watcom.tar.gz .

ENV WATCOM=/opt/opwatcom
ENV PATH=$WATCOM/binl:$PATH
ENV EDPATH=$WATCOM/eddat
ENV INCLUDE=$WATCOM/h

ADD build.sh /build.sh

# CMD cd /build && make -j16
# CMD ["sh", "-c"]
ENTRYPOINT ["/build.sh"]