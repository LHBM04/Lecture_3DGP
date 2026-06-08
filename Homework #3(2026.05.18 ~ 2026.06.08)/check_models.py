import string
import sys

def print_strings(filepath):
    with open(filepath, 'rb') as f:
        data = f.read()
    
    current_str = []
    strings = set()
    for byte in data:
        char = chr(byte)
        if char in string.ascii_letters + string.digits + "_":
            current_str.append(char)
        else:
            if len(current_str) >= 4:
                strings.add("".join(current_str))
            current_str = []
    if len(current_str) >= 4:
        strings.add("".join(current_str))
        
    for s in sorted(strings):
        print(s)

print("--- Gunship ---")
print_strings('Resources/Models/Gunship.bin')
print("--- SuperCobra ---")
print_strings('Resources/Models/SuperCobra.bin')
