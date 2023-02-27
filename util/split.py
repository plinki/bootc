#!/usr/bin/python3
import sys

if len(sys.argv) >= 2:
    hf = sys.argv[1]
    with open(hf, 'r') as f:
        h = f.read()

        byte_arr = bytearray.fromhex(h)
        hex_list = [f'0x{byte:02x}' for byte in byte_arr]
        print(', '.join(hex_list))
else:
    print("no")
