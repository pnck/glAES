#!/usr/bin/python3
from PIL import Image, ImageMath
import sys
import struct
from base64 import b64decode as b64d
if( len(sys.argv) < 3):
    print('usage: '+sys.argv[0]+' B64KEY FILE')
    exit(1)

#keylen = 128bits = 4x4x8bits = 4bytes x 4channels = 2x2 image with 4 channel

keybytes = b64d(sys.argv[1])
pixel4 = (bytes(b) for b in zip(*[iter(keybytes)]*4))

img = Image.frombytes('RGBA',(4,1),b''.join(pixel4))
img.save(sys.argv[2],'png')

#print ( Image.open(sys.argv[2]).tobytes() )