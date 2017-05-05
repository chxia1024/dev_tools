FROM multiarch/crossbuild
ADD lightjbl.tar.gz /
RUN \
  cd /lightjbl && \
  export PATH=/usr/osxcross/bin:$PATH && \
  ./configure --prefix=/_macos \
    --host=x86_64-apple-darwin14 CC=x86_64-apple-darwin14-clang && \
  make && make install-strip && make clean && \
  cp /_macos/bin/lightjbl /lightjbl-macos && \
  \
  ./configure --prefix=/_win32 --host=i686-w64-mingw32 && \
  make && make install-strip && make clean && \
  cp /_win32/bin/lightjbl* /lightjbl-win32.exe && \
  \
  ./configure --prefix=/_win64 --host=x86_64-w64-mingw32 && \
  make && make install-strip && \
  cp /_win64/bin/lightjbl* /lightjbl-win64.exe && \
  \
  tar zcvf /lightjbl-apple-win.tar.gz /lightjbl-macos /lightjbl-win*.exe
