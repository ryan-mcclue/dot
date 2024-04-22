# SPDX-License-Identifier: zlib-acknowledgement

from math import *
import os
import random

def c():
  os.system("clear")

def top(v, n, l):
  return (v & (((1 << n) - 1) << (l - n))) >> (l - n)

# f"{0x200:012b}"
