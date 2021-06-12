![example workflow](https://github.com/marijnvanh/ft_IRC/actions/workflows/ci.yml/badge.svg)
# ft_IRC
As part of the Codam Advanced curriculum, students are expected to create a simple IRC server that handles basic client to client communication. The server needs to work with real IRC clients. For this server the irssi and kiwiirc clients were used. Multiple servers can be linked to each other to create an IRC network.

# Table of content
- [ft_IRC](#ft_irc)
- [Table of content](#table-of-content)
- [Requirements](#requirements)
- [Quick start](#quick-start)
	- [Local](#local)
	- [Docker](#docker)
- [Manual build](#manual-build)
- [Running the server](#running-the-server)
- [Config](#config)
- [TLS](#tls)
- [Testing methods](#testing-methods)
	- [Unit testing](#unit-testing)
	- [End to End tests](#end-to-end-tests)
- [Supported client commands](#supported-client-commands)
- [Shared Lib](#shared-lib)
- [To be improved](#to-be-improved)
- [Contributors](#contributors)
- [Resources](#resources)
	- [IRC Documentation](#irc-documentation)
	- [Network programming](#network-programming)
	- [OpenSSL](#openssl)
	- [JSON parser](#json-parser)
	- [Other recourses](#other-recourses)


# Requirements

The IRC server can be build and run with the included Dockerfile. If you want to build the server locally you need to install at least the following dependencies:

`build-essential`  
`cmake 3.16 or higher`  
`C++ 17`  

For supporting TLS you also need OpenSSL (`libssl-dev`) installed, however this is not mandatory.  
Optionally you also need python3 to run the run script.

# Quick start

Clone the repo and init the submodules:  
` git clone --recursive https://github.com/marijnvanh/ft_IRC.git`

## Local
To run the server locally run the `run.py` script:  
`./run.py`

After the server is running you can now use an IRC client of your choice to connect to localhost on port 5000.

## Docker
To run the server in a docker container follow the following steps:  

Build the docker image:  
`docker build -t ft_irc .`

Start a container with the following line where 8080 is your local port for TCP and 8081 is your port for TCP with TLS enabled:  
`docker run --rm -dt -p 8080:5000 -p 8081:5001 ft_irc`  

After the server is running you can now use an IRC client of your choice to connect to localhost on port 8080 or with a client with TLS enabled on port 8081.

# Manual build

To manually build the project run the following command.

`cmake -B build && make -C build`  

To also build with TLS support run the following command where <OPENSSL_ROOT_DIR> is the root directory of you OpenSSL installation.   

`cmake -B build -DENABLE_SSL=ON -DOPENSSL_ROOT_DIR=<OPENSSL_ROOT_DIR> && make -C build`

# Running the server
After the build the binary will be in `./bin`. You can then run the server like:  
`./bin/irc_server <config_file>`  
Where config is a valid configuration file.

# Config
In `./configs` there are a few examples of valid config files where config_1.json is the default config file. You can modify the config file to your needs. You can change ports, admins, loglevels and more.

Server Data:
```
	"server-data": {
		"name": "codam.s3.com",
		"network": "codam.s3.com",
		"description": "The best server in the whole wide world."
	}
```
Admins:
```
	"administrators": [
		["nickname", "password"]
	]
```
Authorized servers:
```
	"authorized-servers": {
		"codam.s1.com": {
			"ip": "0.0.0.0",
			"port": "5000",
			"recv_pass": "pass1",
			"send_pass": "pass2"
		}
```
You can change the default log level for all modules or change the log level per module.  
Possible log levels are `DEBUG, INFO, WARNING, ERROR`  
Log settings:
```
	"log-settings": {
		"default-level": "ERROR",
		"log-levels": [
			["IRCServer", "INFO"]
```


# TLS
To support TLS connections between a client and a server OpenSSL is used. Included in this repo you will find self signed certificates that can be used for demonstration purposes only (includes private key). Do not use these keys for secure communication.

# Testing methods

## Unit testing
The main components are unit tested with GoogleTest and GoogleMock. However, the code coverage can definitely be improved.

## End to End tests
There are no end to end tests run on the codebase. The message flows could (should) be tested with for example a tool like postman.

# Supported client commands
* PING
* PASS
* NICK
* USER
* KILL
* QUIT
* PRIVMSG
* JOIN
* PART
* NAMES
* MODE
* SQUIT
* CONNECT
* OPER

# Shared Lib
The code included in shared lib mostly contains code related to handling sockets and TCP. It also contains some other tools that could be shared between a server and a client program. There is no client (yet) but this shared library could be used for both.

# To be improved
* User identification with an identity server
* Server to Server encryption
* Enriched server diagnostics
* A Client
* Improved Unit testing
* Integration testing of different message flows
* And probably a lot more

# Contributors
- [Marijn van Heesewijk](https://github.com/marijnvanh)
- [Rob Kuijper](https://github.com/robkuijper)
- [Emily Martins](https://github.com/emiflake)

# Resources

## IRC Documentation

https://tools.ietf.org/html/rfc1459 Protocol  
https://tools.ietf.org/html/rfc2810 Architecture  
https://tools.ietf.org/html/rfc2811 Channel Management  
https://tools.ietf.org/html/rfc2812 Client  
https://tools.ietf.org/html/rfc2813 Server Protocol  
https://tools.ietf.org/html/rfc7194 Default Port for Internet Relay Chat (IRC) via TLS/SSL


## Network programming
https://beej.us/guide/bgnet/html/ Guide to Network Programming

## OpenSSL

https://wiki.openssl.org/index.php/Simple_TLS_Server  
https://support.hpe.com/hpesc/public/docDisplay?docId=emr_na-c04621464  
https://help.ubuntu.com/community/OpenSSL  
https://developer.ibm.com/tutorials/l-openssl/#get-started
https://www.digitalocean.com/community/tutorials/openssl-essentials-working-with-ssl-certificates-private-keys-and-csrs  
https://gist.github.com/darrenjs/4645f115d10aa4b5cebf57483ec82eca  

## JSON parser

https://github.com/nlohmann/json json parser

## Other recourses

http://chi.cs.uchicago.edu/chirc/index.html IRC example project  
https://github.com/inspircd/inspircd InspIRCd repo  

