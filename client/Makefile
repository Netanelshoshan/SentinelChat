UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
    CMAKE := $(shell command -v cmake 2> /dev/null)
    GCC := $(shell command -v gcc 2> /dev/null)
    BUILD_ESSENTIAL := $(shell dpkg -s build-essential 2> /dev/null | grep Status)
    CRYPTOPP_DEV := $(shell dpkg -s libcrypto++-dev 2> /dev/null | grep Status)

    ifeq ($(CMAKE),)
        $(info CMake not found. Installing CMake using apt...)
        $(shell sudo apt update)
        $(shell sudo apt install -y cmake)
    endif

    ifeq ($(GCC),)
        $(info GCC not found. Installing GCC using apt...)
        $(shell sudo apt update)
        $(shell sudo apt install -y gcc)
    endif

    ifeq ($(BUILD_ESSENTIAL),)
        $(info Build-essential package not found. Installing build-essential using apt...)
        $(shell sudo apt update)
        $(shell sudo apt install -y build-essential)
    endif

endif

ifeq ($(UNAME), Darwin)
	CMAKE := $(shell command -v cmake 2> /dev/null)
	GCC := $(shell command -v gcc 2> /dev/null)
	BREW := $(shell command -v brew 2> /dev/null)
	CRYPTOPP := $(shell brew list | grep cryptopp)

	ifeq ($(CMAKE),)
		$(info CMake not found. Installing CMake using brew...)
		$(shell brew install cmake)
	endif

	ifeq ($(GCC),)
		$(info GCC not found. Installing GCC using brew...)
		$(shell brew install gcc)
	endif

	ifeq ($(BREW),)
		$(info Brew not found. Installing Brew...)
		$(shell /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)")
	endif

endif

all:
	@cmake -S. -B build
	@cmake --build build
	@cp build/client .
	@cp -r client server.info client1/
	@cp -r client server.info client2/

clean:
	@rm -rf build client1/* client2/*
	@rm -rf client ../server/__pycache__ ../server/server.db