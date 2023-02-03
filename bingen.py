import struct
import random

filename = "input.bin"

with open(filename, "wb") as f:
    for i in range(10):
        num = random.uniform(0, 0.8)
        packed_num = struct.pack("f", num)
        f.write(packed_num)
