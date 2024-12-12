UPX_EXECUTABLE=upx

if [ "$(uname -m)" = "aarch64" ]
then
    UPX_EXECUTABLE=../tools/arm64/upx
fi

$UPX_EXECUTABLE $@