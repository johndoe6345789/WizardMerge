# Dockerfile for WizardMerge Backend with Drogon
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    git \
    gcc \
    g++ \
    cmake \
    ninja-build \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Drogon framework
WORKDIR /tmp
RUN git clone https://github.com/drogonframework/drogon.git && \
    cd drogon && \
    git submodule update --init && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make install && \
    cd /tmp && rm -rf drogon

# Set up work directory
WORKDIR /app
COPY . .

# Build WizardMerge
RUN mkdir -p build && cd build && \
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release && \
    ninja

# Expose port
EXPOSE 8080

# Copy config to build directory
RUN cp config.json build/

# Run the server
WORKDIR /app/build
CMD ["./wizardmerge-cli"]
