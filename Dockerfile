FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libvulkan-dev \
    libxcb1-dev \
    libx11-xcb-dev \
    libxcb-dri3-dev \
    libxcb-present-dev \
    libxcb-randr0-dev \
    libxcb-shm0-dev \
    libxcb-xfixes0-dev \
    libxcb-xkb-dev \
    libxkbcommon-x11-dev \
    libgl1-mesa-dev \
    libgtk-3-dev \
    libx11-dev \
    libxext-dev \
    libxrender-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Initialize submodules (required for rexglue-sdk and moltenvk)
RUN git submodule update --init --recursive 2>&1 || true

# Build
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DREXSDK_DIR=/app/thirdparty/rexglue-sdk \
    -DREXGLUE_USE_VULKAN=ON \
    -DREXGLUE_USE_D3D12=OFF \
    && cmake --build build --config Release --parallel $(nproc)

# Package (optional)
# RUN cd build && cpack -C Release
