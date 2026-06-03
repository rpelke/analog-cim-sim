# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Install system dependencies including CMake >= 3.15, Python3 development tools, and TBB
RUN apt-get update -qq \
    && apt-get install -y --no-install-recommends \
    git \
    ssh \
    g++ \
    gdb \
    cmake \
    python3 \
    python3-dev \
    libtbb-dev \
    lcov \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set the default command to bash
SHELL ["/bin/bash", "-c"]
