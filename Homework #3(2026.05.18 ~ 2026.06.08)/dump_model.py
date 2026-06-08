import struct

def read_tag(f):
    len_byte = f.read(1)
    if not len_byte: return None
    l = len_byte[0]
    return f.read(l).decode('utf-8')

def read_string(f):
    len_byte = f.read(1)
    if not len_byte: return None
    l = len_byte[0]
    return f.read(l).decode('utf-8', errors='ignore')

def read_value(f, fmt):
    s = struct.calcsize(fmt)
    data = f.read(s)
    if not data: return None
    res = struct.unpack(fmt, data)
    return res[0] if len(res) == 1 else res

def dump_model(filename):
    print(f"--- {filename} ---")
    with open(filename, 'rb') as f:
        read_tag(f) # <Model>:
        read_string(f)
        read_tag(f) # <Nodes>:
        node_count = read_value(f, '<I')
        for i in range(node_count):
            read_tag(f) # <Node>:
            idx = read_value(f, '<i')
            parent_idx = read_value(f, '<i')
            node_name = read_string(f)
            read_tag(f) # <LocalPosition>:
            read_value(f, '<fff')
            read_tag(f) # <LocalRotation>:
            read_value(f, '<ffff')
            read_tag(f) # <LocalScale>:
            scale = read_value(f, '<fff')
            read_tag(f) # <Renderers>:
            rc = read_value(f, '<I')
            for r in range(rc):
                read_tag(f) # <Renderer>:
                read_string(f)
                read_string(f)
            print(f"Node {i}: {node_name} | Parent {parent_idx} | Scale {scale}")

dump_model('Resources/Models/Gunship.bin')
dump_model('Resources/Models/SuperCobra.bin')
