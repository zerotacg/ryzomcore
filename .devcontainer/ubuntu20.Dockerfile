FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install --yes software-properties-common wget
RUN wget --quiet https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb
RUN dpkg --install packages-microsoft-prod.deb
RUN apt update
RUN apt install --yes cmake build-essential ninja-build ccache
RUN apt install --yes bison autoconf automake
RUN apt install --yes libpng-dev
RUN apt install --yes libjpeg-dev
RUN apt install --yes libgif-dev libfreetype6-dev
RUN apt install --yes freeglut3-dev
RUN apt install --yes liblua5.2-dev libluabind-dev libcpptest-dev
RUN apt install --yes libogg-dev libvorbis-dev libopenal-dev
RUN apt install --yes libavcodec-dev libavformat-dev libavdevice-dev libswscale-dev libpostproc-dev
RUN apt install --yes libmysqlclient-dev
RUN apt install --yes libxml2-dev
RUN apt install --yes libcurl4-openssl-dev libssl-dev
RUN apt install --yes libsquish-dev
RUN apt install --yes liblzma-dev
RUN apt install --yes libgsf-1-dev
RUN apt install --yes qtbase5-dev qttools5-dev qttools5-dev-tools
RUN apt install libmsquic
RUN wget https://raw.githubusercontent.com/microsoft/msquic/main/src/inc/msquic.h
RUN wget https://raw.githubusercontent.com/microsoft/msquic/main/src/inc/msquic_posix.h
RUN wget https://raw.githubusercontent.com/microsoft/msquic/main/src/inc/quic_sal_stub.h
RUN mv msquic.h msquic_posix.h quic_sal_stub.h /usr/include/

RUN apt install --yes \
        gdb \
        git \
        libxrandr-dev

RUN wget --output-document=/tmp/cmake.sh https://github.com/Kitware/CMake/releases/download/v3.28.1/cmake-3.28.1-linux-x86_64.sh
RUN mkdir /opt/cmake \
 && sh /tmp/cmake.sh --skip-license --prefix=/opt/cmake \
 && ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake

# work arround as there is no debug version available
RUN cp /lib/x86_64-linux-gnu/libcpptest.so /lib/x86_64-linux-gnu/libcpptestd.so


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
