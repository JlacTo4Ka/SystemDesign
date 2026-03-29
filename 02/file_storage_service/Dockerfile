FROM ghcr.io/userver-framework/ubuntu-24.04-userver:latest AS builder

WORKDIR /app

COPY configs/ configs/
COPY docs/ docs/
COPY src/ src/
COPY cmake/ cmake/
COPY CMakeLists.txt Makefile ./

RUN cmake -S . -B build-release \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSERVER_FEATURE_TESTSUITE=OFF \
    && cmake --build build-release --target jwt_auth -j"$(nproc)"

FROM ghcr.io/userver-framework/ubuntu-24.04-userver:latest

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        curl \
        libssl3t64 \
        libcurl4t64 \
        libev4t64 \
        libnghttp2-14 \
        libc-ares2 \
        libboost-iostreams1.83.0 \
        libboost-filesystem1.83.0 \
        libboost-locale1.83.0 \
        libboost-program-options1.83.0 \
        libboost-regex1.83.0 \
        libboost-stacktrace1.83.0 \
        libcrypto++8t64 \
        libjemalloc2 \
        libre2-10 \
        libfmt9 \
        libcctz2 \
        libyaml-cpp0.8 \
        zlib1g \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build-release/jwt_auth /app/jwt_auth
COPY configs/ /app/configs/

EXPOSE 8080

CMD ["/app/jwt_auth", \
     "--config", "/app/configs/static_config.yaml", \
     "--config_vars", "/app/configs/config_vars.docker.yaml"]