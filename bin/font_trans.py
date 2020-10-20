import sys
import base64



def write_font_cmd_file(font_name, font_dot, out_file):
    with open(out_file, "wt") as out:
        name_str=""
        for name in font_name:
            name_str += "\"%s\"," % name
        print("font_name [%s]" % name_str[0:-1], file=out)

        list_str = ""
        chksum=0
        total_len=0
        dot_all=[]
        for dot in font_dot:
            chksum += len(dot)
            list_str += "%08X," % (len(dot))
            total_len += len(dot)
            dot_all+=dot
        chksum+=total_len + len(font_dot)
        print("font_list chksum:%08X,total:%08X,item:0_%d,list:%s" % (chksum, total_len, len(font_dot), list_str), file=out)
        print("font_list font %d samples %d" % (len(font_dot), total_len))

        PACK_SIZE_MAX=512
        offset=0
        for pack_offset in range(0, len(dot_all), PACK_SIZE_MAX):
            pack_size=len(dot_all)-pack_offset
            if pack_size>PACK_SIZE_MAX:
                pack_size=PACK_SIZE_MAX
            
            chksum=0
            pack=dot_all[pack_offset:pack_offset+pack_size]
            for value in pack:
                chksum+=value
            chksum += offset+pack_size
            wave_str = base64.b64encode(bytes(pack)).decode("ascii")
            print("font_dot chksum:%08X,offset:%08X,len:%04X,data:%s" % (chksum, offset, pack_size, wave_str), file=out)

            offset += pack_size

DOT_FILES = ["ascii_24", "ascii_32", "ascii_36", "gb2312_24", "gb2312_32", "gb2312_36"]
font_dot=[]
for file in DOT_FILES:
    with open("%s.df" % file, 'rb') as dot_file:
        font_dot.append(dot_file.read())

write_font_cmd_file(DOT_FILES, font_dot, "font_dot.cwfd")
