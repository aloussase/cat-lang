FROM ubuntu:latest AS build

RUN apt-get update && apt-get install -y cmake g++
RUN apt-get install -y libboost-all-dev

WORKDIR /app
COPY . .
RUN cmake -S . -B ./build && cmake --build ./build --target web --config Release

EXPOSE 8080

CMD ./build/web/web
