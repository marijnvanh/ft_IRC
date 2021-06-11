FROM debian

RUN apt-get update && apt-get upgrade -y && \
	apt-get install -y \
	wget \
	build-essential \
	python3 \
	libssl-dev

RUN wget https://cmake.org/files/v3.20/cmake-3.20.0-linux-x86_64.sh && \
	mkdir /opt/cmake && \
	sh cmake-3.20.0-linux-x86_64.sh --prefix=/opt/cmake --skip-license && \
	ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake

WORKDIR /irc

COPY . /irc

RUN rm -rf build bin && \
	./run.py -ssl ON -m build && \
	cp ./bin/irc_server /bin/irc_server

# Run the server with 'default' config.
# This config can be mounted with different values as described below.
ENTRYPOINT irc_server /irc/config.json

# BUILD
# docker build -t ft_irc .

# RUN
# docker run --rm -dt -p<link_port>:<irc_port> ft_irc

# RUN WITH MOUNTED (DIFFERENT) CONFIG
# docker run --rm -dt -p 8080:5000 -p 8081:5001 --mount type=bind,source="$(pwd)"/configs/config_1.json,target=/irc/config.json ft_irc

# EXEC
# docker exec -it <container_id> /bin/bash
