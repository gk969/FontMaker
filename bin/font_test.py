import sys

FONT_SIZE = 32
BYTE_PER_LINE = (int)((FONT_SIZE+7)/8)
BYTE_PER_FONT = FONT_SIZE*BYTE_PER_LINE
BYTE1_P1_BEGIN = 0xA1
BYTE1_P1_END = 0xA9
BYTE1_P2_BEGIN = 0xB0
BYTE1_P2_END = 0xF7
BYTE2_BEGIN = 0xA1
BYTE2_END = 0xFE

file_dot = [0]
with open('gb2312_%d.df' % FONT_SIZE, 'rb') as file:
    file_dot = file.read()

code = "黪黯鼢鼬。，》《“【】{}（）！@#￥%……&*".encode('gb2312')
# code=[0xFE, 160, 0xFE, 159, 0xFE, 158, 0xFE, 157, 0xFE, 156, 0xFE, 155, 0xFE, 154]
print(code)

for i in range(0, len(code), 2):
    c = code[i:i+2]
    print(c)
    if c[0]<BYTE1_P1_BEGIN or (c[0]>BYTE1_P1_END and c[0]<BYTE1_P2_BEGIN) or c[0]>BYTE1_P2_END:
        continue
    if c[1]<BYTE2_BEGIN or c[1]>BYTE2_END:
        continue
    
    pos_line = (c[0]-BYTE1_P1_BEGIN) if c[0]<BYTE1_P2_BEGIN else (c[0]-BYTE1_P2_BEGIN+BYTE1_P1_END-BYTE1_P1_BEGIN+1)
    pos = (pos_line*(BYTE2_END-BYTE2_BEGIN+1) +
           (c[1]-BYTE2_BEGIN))*BYTE_PER_FONT
    dot = file_dot[pos:pos+BYTE_PER_FONT]

    print('pos %d' % pos)
    print(dot)

    for y in range(FONT_SIZE):
        line = ''
        for x in range(FONT_SIZE):
            line_byte = y*BYTE_PER_LINE+(int)(x/8)
            line += ' ' if (dot[line_byte] & (1 << (x % 8))) == 0 else '*'
        print(line)


ASCII_BYTE_BEGIN = 0x20
ASCII_WIDTH = (int)(FONT_SIZE/2)
ASCII_BYTE_PER_LINE = (int)((ASCII_WIDTH+7)/8)
ASCII_BYTE_PER_FONT = FONT_SIZE*ASCII_BYTE_PER_LINE

with open('ascii_%d.df' % FONT_SIZE, 'rb') as file:
    file_dot = file.read()

code = "Asc12".encode('ascii')
# code=[0xFE, 160, 0xFE, 159, 0xFE, 158, 0xFE, 157, 0xFE, 156, 0xFE, 155, 0xFE, 154]
print(code)

for i in range(0, len(code)):
    c = code[i]
    print(c)
    pos = (c-ASCII_BYTE_BEGIN)*ASCII_BYTE_PER_FONT
    dot = file_dot[pos:pos+ASCII_BYTE_PER_FONT]

    print('pos %d' % pos)
    print(dot)

    for y in range(FONT_SIZE):
        line = ''
        for x in range(ASCII_WIDTH):
            line_byte = y*ASCII_BYTE_PER_LINE+(int)(x/8)
            line += ' ' if (dot[line_byte] & (1 << (x % 8))) == 0 else '*'
        print(line)
