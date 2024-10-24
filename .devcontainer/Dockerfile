# Base image
FROM mcr.microsoft.com/vscode/devcontainers/cpp:ubuntu-22.04

# Install additional tools if needed
RUN apt-get update && \
    apt-get install -y cmake gdb valgrind

# Clean up
RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/*
