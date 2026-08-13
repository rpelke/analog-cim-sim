# Use Ubuntu 24.04 as the base image
FROM ubuntu:24.04

# Install system dependencies including CMake >= 3.15, Python3 development tools, and TBB.
# clang-format-18 is required by ./util/format_cpp.py (see the Style workflow).
# clangd-18 backs the C/C++ LSP (see the clangd settings in devcontainer.json).
# shellcheck and shfmt back the Bash IDE extension.
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
    clangd-18 \
    shellcheck \
    shfmt \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Restore the man pages Ubuntu's OCI image strips, for Bash IDE hover. ~50 MB.
# hadolint ignore=DL4006
RUN apt-get update -qq \
    && apt-get install -y --no-install-recommends \
    locales \
    man-db \
    && yes | unminimize \
    && locale-gen en_US.UTF-8 \
    && update-locale LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV LANG=en_US.UTF-8 \
    LC_ALL=en_US.UTF-8

# Set the default command to bash
SHELL ["/bin/bash", "-c"]
