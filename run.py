#!/usr/bin/env python3

import argparse, subprocess

def Build():

    if args.target == "tests":
        options = "-DPACKAGE_TESTS=ON"
    else:
        options = "-DPACKAGE_TESTS=OFF"
    options += f" -DENABLE_SSL={args.enable_ssl}"
    if args.ssl_path != None:
        options += f" -DOPENSSL_ROOT_DIR={args.ssl_path}"
    if subprocess.call([f"cmake -B build {options}"], shell=True):
        exit(-1)

    if subprocess.call(["make -C build"], shell=True):
        exit(-1)

def Run(target, config_file):

    if target == "server":
        if subprocess.call([f"./bin/irc_server {config_file}"], shell=True):
            exit(-1)
    elif target == "client":
        if subprocess.call(["./bin/irc_client localhost"], shell=True):
            exit(-1)
    elif target == "tests":
        if args.name_test:
            specific_test_option = f"-R '{args.name_test}'"
        else:
            specific_test_option = ""
        print(specific_test_option)
        if subprocess.call([f"cd build && ctest --verbose {specific_test_option}"], shell=True):
            exit(-1)

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description='Build and Run tool for IRC')
    arg_parser.add_argument('-m', '--method', default='BR', choices=['BR', 'build', 'run'],
        help='Options: B - Build; R - Run. Default is build and run.')
    arg_parser.add_argument('-t', '--target', default='server', choices=['server', 'client', 'tests'],
        help='Options: server, client, tests.')
    arg_parser.add_argument('-n', '--name_test', default=None,
        help='Name of specific test')
    arg_parser.add_argument('-c', '--config_file', default="./config.json",
        help='Config file')
    arg_parser.add_argument('-ssl', '--enable_ssl', default="OFF",
        help='Enable or disable SSL via ON/OFF')
    arg_parser.add_argument('-ssl_path', '--ssl_path', default=None,
        help='Enable or disable SSL via ON/OFF')
    args = arg_parser.parse_args()

    if args.method != 'run':
        Build()
    if args.method != 'build':
        Run(args.target, args.config_file)
