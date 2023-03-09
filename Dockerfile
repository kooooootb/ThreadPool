# Build
FROM gcc:latest as build

RUN apt-get update &&  \
    apt-get install -y cmake

COPY ./src /app/src

WORKDIR /app/build

RUN cmake ../src && \
    cmake --build . --target ThreadPool

# Run
FROM ubuntu:latest as run

# run app from system user
RUN groupadd -r runner && useradd -r -g runner runner

USER runner

WORKDIR /app

COPY --from=build /app/build/ThreadPool .

ENTRYPOINT ["./ThreadPool"]
