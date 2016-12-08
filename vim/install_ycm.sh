#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env VIM_PLUGINS
check_env PYTHON_INSTALL
check_env LLVM_INSTALL

dir="$VIM_PLUGINS/YouCompleteMe"

cd $dir
git submodule update --init --recursive

EXTRA_CMAKE_ARGS="-DPATH_TO_LLVM_ROOT=$LLVM_INSTALL"
# for cmake to find python
EXTRA_CMAKE_ARGS="$EXTRA_CMAKE_ARGS -DPYTHON_EXECUTABLE:FILEPATH=$PYTHON_INSTALL/bin/python3"
if [ ! -z $GCC_PATH ]; then
   EXTRA_CMAKE_ARGS="$EXTRA_CMAKE_ARGS -DCMAKE_C_COMPILER=$GCC_PATH/gcc -DCMAKE_CXX_COMPILER=$GCC_PATH/g++"
fi

log_info "EXTRA_CMAKE_ARGS is set to $EXTRA_CMAKE_ARGS"

# TODO add more language support
EXTRA_CMAKE_ARGS=$EXTRA_CMAKE_ARGS PATH=$LLVM_INSTALL/bin:$PYTHON_INSTALL/bin:$PATH python3 install.py --clang-completer
check_ret $? "install ycm."
# link libclang (and libstdc++ if needed ) to the same path where ycm_core.so located in order to reuse the RPATH=$ORIGIN in ycm_core.so
/bin/ln $LLVM_INSTALL/lib/libclang.so* -s $dir/third_party/ycmd/ >/dev/null 2>&1
if [ ! -z $GCC_PATH ]; then
   LIBCXX_PATH="$(dirname $GCC_PATH)"/lib64
   /bin/ln $LIBCXX_PATH/libstdc++.so* -s $dir/third_party/ycmd/ >/dev/null 2>&1
fi
cd -
