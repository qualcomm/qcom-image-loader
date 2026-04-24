FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    git \
    curl \
    wget \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace