#!/usr/bin/python3
from PIL import Image, ImageMath
import sys
import struct
from base64 import b64decode as b64d
if( len(sys.argv) < 3):
    print('usage: '+sys.argv[0]+' B64KEY FILE')
    exit(1)

#keylen = 128bits = 4x4x8bits = 4bytes x 4channels = 2x2 image with 4 channel

keybytes = b64d(sys.argv[1])[:16]
keywords = list(struct.unpack("IIII",keybytes))


SBOX = bytes.fromhex('''
63	7C	77	7B	F2	6B	6F	C5	30	01	67	2B	FE	D7	AB	76
CA	82	C9	7D	FA	59	47	F0	AD	D4	A2	AF	9C	A4	72	C0
B7	FD	93	26	36	3F	F7	CC	34	A5	E5	F1	71	D8	31	15
04	C7	23	C3	18	96	05	9A	07	12	80	E2	EB	27	B2	75
09	83	2C	1A	1B	6E	5A	A0	52	3B	D6	B3	29	E3	2F	84
53	D1	00	ED	20	FC	B1	5B	6A	CB	BE	39	4A	4C	58	CF
D0	EF	AA	FB	43	4D	33	85	45	F9	02	7F	50	3C	9F	A8
51	A3	40	8F	92	9D	38	F5	BC	B6	DA	21	10	FF	F3	D2
CD	0C	13	EC	5F	97	44	17	C4	A7	7E	3D	64	5D	19	73
60	81	4F	DC	22	2A	90	88	46	EE	B8	14	DE	5E	0B	DB
E0	32	3A	0A	49	06	24	5C	C2	D3	AC	62	91	95	E4	79
E7	C8	37	6D	8D	D5	4E	A9	6C	56	F4	EA	65	7A	AE	08
BA	78	25	2E	1C	A6	B4	C6	E8	DD	74	1F	4B	BD	8B	8A
70	3E	B5	66	48	03	F6	0E	61	35	57	B9	86	C1	1D	9E
E1	F8	98	11	69	D9	8E	94	9B	1E	87	E9	CE	55	28	DF
8C	A1	89	0D	BF	E6	42	68	41	99	2D	0F	B0	54	BB	16
'''.strip().replace('\t','').replace('\n',''))

SBOX = list(SBOX)

def g(idx,w):
    #shl
    h = (w & 0xff000000) >> 24
    ww = (w & 0x00ffffff) << 8
    ww |= h
    #sbox
    l = list(struct.pack("I",ww))
    ll = [ SBOX[v] for v in l ]
    #xor rc
    RC = [0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36]
    www = struct.unpack("I",bytes(ll))[0]
    www ^= RC[idx//4]
    return www

roundwords = [0 for i in range(44)]

roundwords[0:4] = keywords[0:4]

for i in range(4,44):
    if i%4 == 0:
        roundwords[i] = roundwords[i-4] ^ g(i,roundwords[i-1])
    else:
        roundwords[i] = roundwords[i-4] ^ roundwords[i-1]
print("\n\nRound keys:\n")
for i in range (4,11):
    for j in range(4):
        print(struct.pack("I",roundwords[i*4+j]),' ')
    print('---------------------')

data = struct.pack('IIII'*10,*roundwords[4:])
img = Image.frombytes('RGBA',(4,10),data)
img.save(sys.argv[2],'png')

#print ( Image.open(sys.argv[2]).tobytes() )