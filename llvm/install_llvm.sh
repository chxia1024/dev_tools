#!/bin/bash
SCRIPT=$(readlink -f "$0")
ROOTPATH=$(dirname $(dirname "$SCRIPT"))

source $ROOTPATH/common.sh

check_env CHXIA
check_env LLVM_INSTALL

compile_llvm() {
   llvm_root=$1
   llvm_src=$llvm_root/llvm
   llvm_build=$llvm_root/build
   rm $llvm_build -rf; mkdir -p $llvm_build
   cd $llvm_build
   CMAKE_ARGS="-DCMAKE_INSTALL_PREFIX=$LLVM_INSTALL"
   if [ ! -z $GCC_PATH ]; then
      CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_C_COMPILER=$GCC_PATH/gcc -DCMAKE_CXX_COMPILER=$GCC_PATH/g++"
      # RPATH for libstdc++
      RPATH="$(dirname $GCC_PATH)"/lib64
      CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_INSTALL_RPATH=$RPATH"
      log_info "LLVM cmake args $CMAKE_ARGS"
   fi
   cmake $CMAKE_ARGS $llvm_src
   check_ret "configure llvm with cmake"
   make -j20 && make install
   check_ret "make llvm"
   cd -
}

llvm_repos=(
git@github.com:llvm-mirror/llvm.git
)

llvm_root=$(dirname $LLVM_INSTALL)
clone_git_repos $llvm_root "${llvm_repos[@]}"

clang_repos=(
git@github.com:llvm-mirror/clang.git
)

# organization clang to LLVM_SRC/tools/clang
llvm_src="$llvm_root/llvm"
clone_git_repos "$llvm_src/tools" "${clang_repos[@]}"

compile_llvm $llvm_root
