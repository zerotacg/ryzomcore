FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install --yes software-properties-common wget
RUN wget --quiet https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb
RUN dpkg --install packages-microsoft-prod.deb
RUN apt update
RUN apt remove --yes man-db
RUN apt install --yes --no-install-recommends \
    libasound2-dev \
    libgl1-mesa-dev \
    libjack-dev \
    libpulse-dev \
    libxft-dev \
    libxrandr-dev \
    libxrender-dev \
    libxxf86vm-dev \
    libmsquic
RUN wget https://raw.githubusercontent.com/microsoft/msquic/main/src/inc/msquic.h
RUN wget https://raw.githubusercontent.com/microsoft/msquic/main/src/inc/msquic_posix.h
RUN wget https://raw.githubusercontent.com/microsoft/msquic/main/src/inc/quic_sal_stub.h
RUN mv msquic.h msquic_posix.h quic_sal_stub.h /usr/include/

RUN apt install --yes \
        build-essential \
        gdb \
        git

RUN wget --output-document=/tmp/cmake.sh https://github.com/Kitware/CMake/releases/download/v3.27.9/cmake-3.27.9-linux-x86_64.sh
RUN mkdir /opt/cmake \
 && sh /tmp/cmake.sh --skip-license --prefix=/opt/cmake \
 && ln --symbolic --force /opt/cmake/bin/cmake /usr/bin/cmake


ARG USERNAME=ryzom
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install --yes sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER $USERNAME
