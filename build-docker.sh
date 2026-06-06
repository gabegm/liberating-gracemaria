#!/bin/bash
set -e

IMAGE_NAME="ac6recomp-linux-build"
CONTAINER_NAME="ac6recomp-build"

# Initialize submodules (required before Docker build)
echo "Initializing submodules..."
git submodule update --init --recursive

# Build Docker image
echo "Building Docker image..."
docker build -t "$IMAGE_NAME" .

# Run build in container
echo "Running build..."
docker run --name "$CONTAINER_NAME" "$IMAGE_NAME"

# Copy build artifacts out
echo "Copying build artifacts..."
docker cp "$CONTAINER_NAME:/app/build" ./build

# Clean up container
docker rm "$CONTAINER_NAME"

# Copy generated headers if they exist (from codegen step)
if [ -d assets ]; then
    echo "Copying generated headers..."
    mkdir -p generated
    docker cp "$CONTAINER_NAME:/app/generated" ./generated 2>/dev/null || true
fi

echo "Build complete! Artifacts in ./build"
