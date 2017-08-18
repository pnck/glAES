#!/usr/bin/python3

from PIL import Image, ImageMath
import sys
import struct
if(len(sys.argv) < 2):
    print('usage: ' + sys.argv[0] + ' FILE')
    exit(1)

imgdata = b''
greyscale = bytes([i for i in range(256)])
rbar = b''.join([bytes(px) for px in zip(greyscale, [0] * 256, [0] * 256)] * 50)
gbar = b''.join([bytes(px) for px in zip([0] * 256, greyscale, [0] * 256)] * 50)
bbar = b''.join([bytes(px) for px in zip([0] * 256, [0] * 256, greyscale)] * 50)

rgRect =  b''.join([b''.join([bytes(j) for j in zip(greyscale,[i]*256,[0]*256)]) for i in range(256)])
rbRect =  b''.join([b''.join([bytes(j) for j in zip([i]*256,[0]*256,greyscale)]) for i in range(256)])
gbRect =  b''.join([b''.join([bytes(j) for j in zip([0]*256,greyscale,[i]*256)]) for i in range(256)])

imgdata1 = gbRect
imgdata2 = rbRect
imgdata3 = rgRect

img1 = Image.frombytes('RGB',(256,256),imgdata1)
img2 = Image.frombytes('RGB',(256,256),imgdata2)
img3 = Image.frombytes('RGB',(256,256),imgdata3)

img3.save(sys.argv[1])