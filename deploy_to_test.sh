#!/bin/sh

# This script copies the release to a test pc using FTP

lftp -u brutman,password Dell210 <<EOF
cd DRIVE_C/TEST
rm -r RELEASE
mkdir RELEASE
cd RELEASE
glob
mput release/*.*
EOF
