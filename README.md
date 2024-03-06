# jsonrpc
# jsonrpc

This repository contains a simple JSON-RPC C example.

## Description

The JSON-RPC C example demonstrates how to implement a basic JSON-RPC server and client using the C programming language. It provides a simple way to perform remote procedure calls over a network using JSON as the data interchange format.

## Features

- Server-side implementation of JSON-RPC using C
- Client-side implementation of JSON-RPC using C
- Basic request and response handling
- Error handling and reporting

## Getting Started

To get started with the JSON-RPC C example, follow these steps:

1. Clone the repository:

    ```bash
    git clone https://github.com/jac18281828/jsonrpc.git
    ```

2. Build the server and client executables:

    ```bash
    cd jsonrpc
    mkdir -p build
    cmake -H. -Bbuild -DPROJECT_NAME=trimtrain -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_VERBOSE_MAKEFILE=on -Wno-dev "-GUnix Makefiles"
    make -j
    ```

3. Start the server:

    ```bash
    build/main
    ```

4. Run the client:

    ```bash
    ./client
    ```

## Usage

The JSON-RPC C example provides a simple API for making remote procedure calls. 
