# Use Ubuntu 24.04 as the base image
FROM ubuntu:24.04

# Install system dependencies including CMake >= 3.15, Python3 development tools, and TBB.
# clang-format-18 is required by ./util/format_cpp.py (see the Style workflow).
RUN apt-get update -qq \
    && apt-get install -y --no-install-recommends \
    ca-certificates \
    git \
    ssh \
    sudo \
    g++ \
    gdb \
    make \
    cmake \
    python3 \
    python3-dev \
    python3-venv \
    libtbb-dev \
    lcov \
    clang-format-18 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set the default command to bash
SHELL ["/bin/bash", "-c"]
