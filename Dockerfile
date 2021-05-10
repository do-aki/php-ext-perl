FROM php:8-cli-buster

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && \
    apt-get upgrade -y && \
    apt install -y build-essential libperl-dev gdb wget && \
    rm -rf /var/lib/apt/lists/*

RUN pecl install xdebug
RUN docker-php-ext-enable xdebug

WORKDIR /perl
RUN wget http://deb.debian.org/debian/pool/main/p/perl/perl_5.28.1.orig.tar.xz
RUN tar -xvf perl_5.28.1.orig.tar.xz

WORKDIR /php-ext-perl

RUN useradd -u 1000 php
USER php
