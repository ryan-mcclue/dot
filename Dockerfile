# SPDX-License-Identifier: zlib-acknowledgement
FROM ubuntu:latest 

ENV TZ=Australia/Sydney
RUN set -ex; \
    # NOTE(Ryan): Prevent docker build hanging on timezone confirmation
    ln -sf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone; \
    apt update; \
    apt install -y --no-install-recommends \
      sudo ca-certificates git gnupg openssh-client vim gnome-terminal terminator gcc; \
    useradd -m ryan -g sudo; \
    printf "ryan ALL=(ALL:ALL) NOPASSWD:ALL" | sudo EDITOR="tee -a" visudo; \
    # NOTE(Ryan): Prevent sudo usage prompt appearing on startup
    touch /home/ryan/.sudo_as_admin_successful; \
    git clone https://github.com/ryan-mcclue/cas.git /home/ryan/cas; \
    chmod 777 -R /home/ryan/cas;

ENTRYPOINT ["/bin/bash", "-l"]

USER ryan
WORKDIR /home/ryan/cas
# TODO(Ryan): Prevent having to manually run: eval "$(ssh-agent -s)"
