# SPDX-License-Identifier: zlib-acknowledgement
FROM ubuntu:latest 

ENV TZ=Australia/Sydney
RUN set -ex; \
    ln -sf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone; \
    apt update; \
    apt install -y --no-install-recommends \
      sudo ca-certificates git gnupg openssh-client gnome-terminal vim; \
    useradd -m ryan -g sudo; \
    printf "ryan ALL=(ALL:ALL) NOPASSWD:ALL" | sudo EDITOR="tee -a" visudo; \
    git clone https://github.com/ryan-mcclue/cas.git /home/ryan

USER ryan
    
ENTRYPOINT ["/bin/bash"]
