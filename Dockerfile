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

# No ENTRYPOINT/CMD specified.
# It's most useful to start this image in a detached state,
# use docker exec to enter a shell and run the IRC binary.

# BUILD
# docker build -t ft_irc .

# RUN
# docker run --rm -dt -p8080:<irc_port> ft_irc

# EXEC
# docker exec -it <container_id> /bin/bash
