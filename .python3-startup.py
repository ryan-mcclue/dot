# SPDX-License-Identifier: zlib-acknowledgement

import math, os
import keyboard

def clear():
  os.system("clear")

keyboard.add_hotkey("ctrl+l", clear)
