# Multi-stage build for TinyMCP
# Based on the approach from MCPServer.cpp for minimal image size

# Build stage
FROM ubuntu:22.04 as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /build

# Copy source code
COPY . .

# Build the project
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Runtime stage - use distroless base for minimal size
FROM gcr.io/distroless/cc-debian12

# Copy the built executable
COPY --from=builder /build/build/Example/MCPServer/Build/Linux/mcp-server /mcp-server

# Copy configuration
COPY --from=builder /build/config.ini /config.ini

# Create directories for certificates and plugins
USER 65532:65532
WORKDIR /

# Expose default port
EXPOSE 6666

# Run the server
ENTRYPOINT ["/mcp-server"]
