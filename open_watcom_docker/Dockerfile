FROM alpine:3.15

RUN apk add --update \
    make \
    nasm \
    upx \
    zip \
  && rm -rf /var/cache/apk/*

ADD open_watcom.tar.gz .

ENV WATCOM=/opt/opwatcom
ENV PATH=$WATCOM/binl:$PATH
ENV EDPATH=$WATCOM/eddat
ENV INCLUDE=$WATCOM/h

COPY open_watcom_build.sh /open_watcom_build.sh

ENTRYPOINT ["/open_watcom_build.sh"]