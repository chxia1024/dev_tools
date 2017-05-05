#!/bin/bash
#
# lightjbl
# Copyright (C) 2017 privatelo
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 

# Ensure you have a git tag, e.g. v0.1
VER=0.2

set -e

cd `git rev-parse --show-toplevel`
git archive --format=tar.gz --prefix=lightjbl/ v$VER > release/lightjbl.tar.gz
cd release
rm -f lightjbl-*

for ARCH in 32 64; do
  docker build --force-rm -t linux-$ARCH -f linux-$ARCH/Dockerfile .
  docker run --rm --entrypoint cat linux-$ARCH /lightjbl-bin > lightjbl-linux-$ARCH
  docker rmi linux-$ARCH
done

docker build --force-rm -t apple-win -f apple-win/Dockerfile .
docker run --rm --entrypoint cat apple-win /lightjbl-apple-win.tar.gz > lightjbl-apple-win.tar.gz
docker rmi apple-win
tar zxvf lightjbl-apple-win.tar.gz
rm -f lightjbl-apple-win.tar.gz

docker build --force-rm -t openwrt-mips -f openwrt-mips/Dockerfile .
docker run --rm --entrypoint= openwrt-mips tar -cf - \
    /lightjbl-ar71xx.ipk /lightjbl-openwrt-ar71xx | tar x -
docker rmi openwrt-mips

mkdir bin-package
mv lightjbl-* bin-package/

pushd bin-package
echo "Lightweight JetBrains License Server v$VER" > README
echo "Binary Release (build $(date +"%y%m%d"))" >> README
echo "" >> README
echo "MD5 Checksum:" >> README
if hash openssl 2>/dev/null; then
    MD5="openssl md5 -r"
else
    MD5="md5sum"
fi
for f in lightjbl-*; do
    chksum=($($MD5 $f))
    [[ "$f" == *.ipk ]] && fname="$f (OpenWRT package)" || fname=$f
    echo "  $fname:" >> README
    echo "    $chksum" >> README
done
if hash perl 2>/dev/null; then
    perl -pe 's/\r\n|\n|\r/\r\n/g' README > README.txt
else
    sed -e 's/$/\r/' README > README.txt
fi
rm -f README
popd

mv bin-package lightjbl-$VER-bins
chmod 755 lightjbl-$VER-bins/lightjbl-*
chmod 644 lightjbl-$VER-bins/*.ipk
tar zcvf lightjbl-$VER-bins.tar.gz lightjbl-$VER-bins
rm -f lightjbl.tar.gz
rm -R lightjbl-$VER-bins

cd `git rev-parse --show-toplevel`
git archive --format=tar.gz --prefix=lightjbl-$VER/ v$VER > release/lightjbl-$VER.tar.gz
