# SPDX-License-Identifier: zlib-acknowledgement
FROM ubuntu:latest 

ENV TZ=Australia/Sydney
RUN set -ex; \
    ln -sf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone; \
    apt update; \
    apt install -y --no-install-recommends \
      ca-certificates sudo git gpg ssh gnome-terminal terminator vim gcc; \
    useradd -m ryan -g sudo; \
    printf "ryan ALL=(ALL:ALL) NOPASSWD:ALL" | sudo EDITOR="tee -a" visudo; \
    git clone https://github.com/ryan-mcclue/cas.git /home/ryan
    
CMD su - ryan; \
    cd cas
