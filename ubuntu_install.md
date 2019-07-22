# wasm-compiler ubuntu install instruction
  this instruction is used to build coscc program on ubuntu.
## step
  1.install basic software
  ```
    apt-get update -y
    apt-get install -y \
                wget \
                git \
                tar \
                gcc \
                g++ \
                python \
                llvm-4.0 \
                libclang-4.0-dev \
                zlib1g-dev \	
                make \
  ```
  
  2.install cmake
  ```
    cd / && \
    wget https://cmake.org/files/v3.12/cmake-3.12.2-Linux-x86_64.tar.gz && \
    tar zxvf cmake-3.12.2-Linux-x86_64.tar.gz && \
    mv cmake-3.12.2-Linux-x86_64 /opt/cmake-3.12.2 && \
    ln -sf /opt/cmake-3.12.2/bin/*  /usr/bin/ && \
    rm -rf /cmake-3.12.2-Linux-x86_64.tar.gz
  ```
  
  3.install boost
  ```
    wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2 && \
    tar -xf boost_1_67_0.tar.bz2 && \
    cd boost_1_67_0 && \
    ./bootstrap.sh && \
    ./b2 -j12 --without-python install && \
    rm -rf /boost_1_67_0 && \
    rm /boost_1_67_0.tar.bz2
  ```
  
  4.install llvm
  ```
    cd / && \
	  git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/llvm.git && \
	  cd llvm/tools/ && \
	  git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/clang.git && \
	  cd .. && \
	  mkdir build && \
	  cd build && \
	  cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/root/opt/wasm -DLLVM_TARGETS_TO_BUILD= -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly -DCMAKE_BUILD_TYPE=Release ../ && \
    make -j12 install && \
    rm -rf /llvm/
  ```
  
  5.install wabt
  ```
    cd / && \
  	wget https://github.com/WebAssembly/wabt/releases/download/1.0.10/wabt-1.0.10-linux.tar.gz && \
	  tar -xf wabt-1.0.10-linux.tar.gz && \
	  cp /wabt-1.0.10/wat2wasm /usr/bin/ && \
    rm -rf /wabt-1.0.10 && \
    rm -rf /wabt-1.0.10-linux.tar.gz
  ```
  
  6.compile wasm-compiler
  ```
    cd / && \
    git clone -b master https://github.com/coschain/wasm-compiler.git && \
    cd wasm-compiler && \
    git submodule update --init && \
    mkdir build && cd build && \
    cmake .. && \
    make -j2 install
  ```
  
  7.finish, now you can use command to compile contract wasm and abi
  ```
  cosiocc -o xxx.wasm xxx.cpp
  cosiocc -g xxx.abi xxx.cpp
  ```

