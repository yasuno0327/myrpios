FROM ubuntu:18.04

RUN apt-get update -yy && \
    apt-get install build-essential

RUN mkdir -p /app/src
WORKDIR /app
ADD . /app/src