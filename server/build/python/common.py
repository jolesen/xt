#coding:utf-8
import os, filecmp

def CompareFile(old, new):
    if(not os.path.isfile(old)):
        os.rename(new, old);
        print "   " + old;
    else:
        if(not filecmp.cmp(old, new)):
            os.remove(old);
            os.rename(new, old);
            print "   " + old;
        else:
            os.remove(new);
