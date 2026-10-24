"""Validate the tested MHGU NPDM and derive the per-game NPDMs from it.

The stock MHGU and MHXX NPDMs differ only in the ACI0 program ID and the ACID
signature, so MHXX reuses the tested template with its program ID replaced.
"""
from pathlib import Path
import hashlib
import struct

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = "4e449887b88c980670b6407bf6d65a22f20d67752f8d4fb53fb78a6a9b3db36d"
OUTPUTS = [("main.npdm",0x0100770008DD8000),("main-mhxx.npdm",0x0100C3800049C000)]


def prepare():
    image = (ROOT/"metadata/main-hardware.npdm").read_bytes()
    if hashlib.sha256(image).hexdigest() != EXPECTED:
        raise ValueError("The tested NPDM template changed")
    for name, header, fields in [("ACI0",0x70,0x30),("ACID",0x78,0x230)]:
        start = struct.unpack_from("<I",image,header)[0]
        offset,size = struct.unpack_from("<II",image,start+fields)
        allowed = set()
        for pos in range(start+offset,start+offset+size,4):
            cap = struct.unpack_from("<I",image,pos)[0]
            if cap & 31 == 15:
                allowed.update((cap>>29)*24+bit for bit in range(24) if cap & (1<<(bit+5)))
        if not {0x4b,0x4c,0x5f,0x74,0x75} <= allowed:
            raise ValueError(name+" lacks the required code-memory/cache/mapping calls")
    aci0 = struct.unpack_from("<I",image,0x70)[0]
    if image[aci0:aci0+4] != b"ACI0":
        raise ValueError("ACI0 header not found")
    acid = struct.unpack_from("<I",image,0x78)[0]
    tid_min,tid_max = struct.unpack_from("<QQ",image,acid+0x210)
    for name, program_id in OUTPUTS:
        if not tid_min <= program_id <= tid_max:
            raise ValueError(f"{program_id:016X} is outside the ACID program ID range")
        output = bytearray(image)
        struct.pack_into("<Q",output,aci0+0x10,program_id)
        (ROOT/name).write_bytes(output)
        print(f"{name}: tested metadata {EXPECTED[:16]}... for {program_id:016X}")


if __name__=="__main__": prepare()
