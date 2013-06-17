#!/usr/bin/env python3

# Reads languages on stdin from http://pastebin.com/api#5

import sys

for line in sys.stdin.readlines():
	name, label = line.strip().split("=")
	print('{{"{0}", "{1}"}},'.format(name.strip(), label.strip()))
