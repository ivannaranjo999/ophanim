# Stage 1: Build
FROM alpine:latest AS builder

RUN apk add --no-cache build-base

WORKDIR /app
COPY . .

RUN gcc -O2 -Wall -Wextra -static -Iinclude \
    -o ophanim src/main.c src/http.c src/cpu.c src/mem.c src/net.c

# Stage 2: Runtime
FROM alpine:latest

COPY --from=builder /app/ophanim /usr/local/bin/ophanim

# Expose HTTP port
EXPOSE 8080

# Env paths
ENV PROC_PATH=/host_proc

ENTRYPOINT ["/usr/local/bin/ophanim"]
