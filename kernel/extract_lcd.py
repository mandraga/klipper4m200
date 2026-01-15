# Opens the lcd.ko kernel module from the original image and extracts the initialization data array.

import struct

# Parameters
filename = "lcd.ko"
start = 0x48B0
end = 0x6488
entry_size = 1 + 4 + 3 + 64  # cmd + count + zeros + zeros2 + para_list

try:
    with open(filename, "rb") as f:
        f.seek(start)
        data = f.read(end - start)
except Exception as e:
    print(f"Error reading '{filename}': {e}")
    exit(1)

for i in range(0, len(data), entry_size):
    entry = data[i:i+entry_size]
    if len(entry) < entry_size:
        break
    cmd, count, zeros, zeros2 = struct.unpack(">B I H B", entry[:8])
    #if cmd == 0x02:
        #break
    #print(f"cmd: 0x{cmd:02X}, count: {count}")
    para_list = list(entry[8:])
    # Only print non-zero parameters for clarity
    para_trimmed = [v for v in para_list if v != 0]
    zeros_list = list(entry[8:8+count])
    print(f"{{0x{cmd:02X}, {count}, {{{', '.join(f'0x{v:02X}' for v in zeros_list)}}}")