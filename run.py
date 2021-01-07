#!/usr/bin/python3

import argparse, subprocess

def Build():

    if args.target == "tests":
        options = "-DPACKAGE_TESTS=ON"
    else:
        options = ""
    if subprocess.call([f"cmake -B build {options}"], shell=True):
        exit(-1)

    if subprocess.call(["make -C build"], shell=True):
        exit(-1)

def Run(target):

    if target == "server":
        if subprocess.call(["./bin/irc_server localhost"], shell=True):
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
    arg_parser.add_argument('-m', '--method', default='BR', choices=['BR', 'B', 'R'],
        help='BR: Build and Run; B: Build only; R: Run only')
    arg_parser.add_argument('-t', '--target', default='server', choices=['server', 'client', 'tests'],
        help='Valid options: server, client, tests')
    arg_parser.add_argument('-n', '--name_test', default=None,
        help='Name of specific test')
    args = arg_parser.parse_args()

    if args.method != 'R':
        Build()
    if args.method != 'B':
        Run(args.target)
