FROM yhnw/openwrt-sdk:15.05.1-ar71xx
ADD lightjbl.tar.gz /
COPY \
  openwrt-mips/Makefile.openwrt \
  openwrt-mips/lightjbl.html \
  openwrt-mips/lightjbl.init /
RUN \
  cd /home/openwrt/sdk && \
  mkdir -p package/lightjbl && \
  cp /Makefile.openwrt package/lightjbl/Makefile && \
  cp /lightjbl.html package/lightjbl/ && \
  cp /lightjbl.init package/lightjbl/ && \
  su openwrt -c "make package/lightjbl/compile V=99" && \
  cp bin/ar71xx/packages/base/lightjbl*.ipk /lightjbl-ar71xx.ipk && \
  cp build_dir/target-*/lightjbl/ipkg-ar71xx/lightjbl/usr/bin/lightjbl /lightjbl-openwrt-ar71xx
