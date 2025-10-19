FROM randomdude/gcc-cross-x86_64-elf

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y nasm xorriso grub-pc-bin grub-common && \
    rm -rf /var/lib/apt/lists/*

VOLUME ["/root/env"]
WORKDIR /root/env
