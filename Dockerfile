# SPDX-License-Identifier: zlib-acknowledgement
FROM ubuntu:latest 

ENV TZ=Australia/Sydney
RUN set -ex; \
    # NOTE(Ryan): Prevent docker build hanging on timezone confirmation
    ln -sf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone; \
    apt update; \
    apt install -y --no-install-recommends \
    # NOTE(Ryan): To find package binary is in, search in ubuntu man pages
      sudo ca-certificates git \
           dconf-cli libglib2.0-bin wget gnome-terminal \
           gnupg openssh-client vim terminator gcc; \
    useradd -m ryan -g sudo; \
    printf "ryan ALL=(ALL:ALL) NOPASSWD:ALL" | sudo EDITOR="tee -a" visudo; \
    # NOTE(Ryan): Prevent sudo usage prompt appearing on startup
    touch /home/ryan/.sudo_as_admin_successful; \
    git clone https://github.com/ryan-mcclue/cas.git /home/ryan/prog/personal/cas; \
    chmod 777 -R /home/ryan/prog/personal/cas;

USER ryan
WORKDIR /home/ryan/prog/personal/cas
# NOTE(Ryan): This is equivalent to running: eval "$(ssh-agent -c)"
CMD ["ssh-agent", "bash", "-l"]
