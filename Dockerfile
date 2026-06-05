FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    clang \
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

# Copy source files (submodules must be populated on the host first)
COPY . .

# Generate FFmpeg avconfig.h (required by FFmpeg build)
RUN cat > /app/thirdparty/rexglue-sdk/thirdparty/FFmpeg/libavutil/avconfig.h << 'EOF'
#ifndef AVUTIL_AVCONFIG_H
#define AVUTIL_AVCONFIG_H
#define HAVE_THREADS 1
#define HAVE_PTHREADS 1
#define HAVE_PRAGMA_DEPRECATED 1
#define HAVE_FAST_CLZ 1
#define HAVE_FAST_UNALIGNED 1
#define AV_HAVE_BIGENDIAN 0
#endif
EOF

# Build
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DREXSDK_DIR=/app/thirdparty/rexglue-sdk \
    -DREXGLUE_USE_VULKAN=ON \
    -DREXGLUE_USE_D3D12=OFF \
    && cmake --build build --config Release --parallel $(nproc)

# Package (optional)
# RUN cd build && cpack -C Release
