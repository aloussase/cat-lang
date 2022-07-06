FROM ubuntu:latest AS build

RUN apt-get update && apt-get install -y cmake g++ libasio-dev spim

WORKDIR /app
COPY . .
RUN cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release && cmake --build ./build --target cat-api

ENV PORT=8080
EXPOSE ${PORT}

CMD ./build/api/cat-api
