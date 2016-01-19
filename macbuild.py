# Copyright(c) 2012-2013 by RecMind. All Rights Reserved.
import os
import sys
import re
import shutil

rootPath = 'builds/build-remind/%s'
pathPattern = 'builds/build-remind/%s/RecMind.app/Contents/MacOS'

def dist(mode = 'Debug', clean = False):
    toPath = pathPattern % mode
    files = os.listdir(toPath)
    
    for f in files:
        f = os.path.sep.join([toPath, f])
        if not os.path.exists(f):
            continue
        if clean and os.path.isfile(f):
            print 'deleted:', f
            os.remove(f)

def clean(mode = 'Debug'):
    targetCore = pathPattern % mode
    files = os.listdir(targetCore)
    for f in files:
        print 'deleting:', f
        path = os.path.sep.join([targetCore, f])
        if os.path.isfile(path):
            os.remove( path )

    
if __name__ == '__main__':
    mode = 'Debug'
    if len(sys.argv) == 2 and sys.argv[1] == 'release':
        mode = 'Release'
    clean(mode)
    dist(mode)
