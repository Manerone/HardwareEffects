# Use an official Ubuntu as a parent image
FROM ubuntu:20.04

# Set environment variables to avoid user interaction during the installation
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /

# Install dependencies
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    git \
    clangd-12 \
    clang \
    sudo \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/google/benchmark.git && \
cd benchmark && \
cmake -DCMAKE_BUILD_TYPE=Release -S . -B "build" -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on && \
cmake --build "build" --config Release --target install

RUN rm -rf benchmark/build

WORKDIR /
