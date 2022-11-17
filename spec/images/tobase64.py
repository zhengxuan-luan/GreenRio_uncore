import base64
import os

f_list = os.listdir('.')

for i in f_list:
    if i.endswith(".png"):
        f = open(i, 'rb')
        newfilename = i.split('.')[0] + '.txt'
        basefile = open(newfilename, 'w')
        ls_f = base64.b64encode(f.read())
        f.close()
        basefile.write(ls_f.decode("utf-8"))
        basefile.close()