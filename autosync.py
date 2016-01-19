import os
import os.path
import distutils.dir_util as dt

def copy(root, name):
    ps = root.split(os.sep)
    to = os.path.join(os.curdir, 'include')
    sn = name.split('.')[0]
    #print ps
    target_path = os.path.join(to, *ps[2:3])
    dt.mkpath(target_path)

    target_file = os.path.join(target_path, sn)
    #if os.path.isfile(target_file):
    #    return
    f = open(target_file, 'w+')
    # print os.path.relpath(os.path.join(root, name), target_path).replace(os.sep, "/")
    f.write('#include "%s"' % os.path.relpath(os.path.join(root, name), target_path).replace(os.sep, "/") )
    f.flush()
    f.close()


def walkdir(dir, cb = None):
    if (dir.endswith('.svn') or dir.endswith('images') ):
        return
    for root, dirs, files in os.walk(dir):
        if root.endswith('gui'):
            continue
        for f in files:
            if cb and f.endswith('.h'):
                cb(root, f)

if __name__ == '__main__':
    path = os.path.join(os.curdir, 'src')
    walkdir(path, copy)
