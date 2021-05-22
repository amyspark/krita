#!/usr/bin/env python3

from sipbuild.abstract_project import AbstractProject
from sipbuild.exceptions import handle_exception

def main():
    """ Amyspark's dummy SIP-lite tool. """

    try:
        project = AbstractProject.bootstrap('build', "Only generate the project bindings.")
        project.builder._generate_bindings()
    except Exception as e:
        handle_exception(e)

    return 0

if __name__ == "__main__":
    main()
