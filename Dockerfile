FROM ubuntu:latest AS build

RUN apt-get update && apt-get install -y cmake g++
RUN apt-get install -y libboost-all-dev

WORKDIR /app
COPY . .
RUN cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release && cmake --build ./build --target cat-api

EXPOSE 8080

CMD ./build/api/cat-api
