import os
import sys
import commands
import shutil

bundleExe = 'RecMind'
bundleBuildDir = 'builds/build-remind/Release'
bundleApp = 'RecMind.app'
bundleRoot = 'dist/mac'

def isQtFramework(f):
    if f.find('Qt') != -1:
        return True
    return False

def isSystemFramework(f):
    if f.find('/usr/') != -1 or f.find('/System/') != -1:
        return True
    return False

def isCoreLibrary(f):
    if f.find('plugin') != -1:
        return False
    return True

def isPluginLibrary(f):
    if f.find('plugin') != -1:
        return True
    return False

def copyFile(f):
    copyTo = '%s%s' % (bundleRoot, f.replace(bundleBuildDir, ''))
    if copyTo.find( bundleApp ) != -1:
        print 'copy to:', copyTo
        shutil.copy(f, copyTo) 


def makeBundleDirs():
    bundleBin = '%s/%s/Contents/MacOS' % (bundleRoot, bundleApp) 
    bundleResource = '%s/%s/Contents/Resources' % (bundleRoot, bundleApp)
    os.system('mkdir -p %s' % bundleBin)
    os.system('mkdir -p %s' % bundleResource)

def getDependList(target):
    dependList = commands.getoutput('otool -L %s' % target).replace('\t', '').split('\n')
    del(dependList[0])
    dependList = [item.split(' ')[0] for item in dependList]
    return dependList

def getAllDepends(dir):
    allDependFiles = []
    filters = ['.obj', '.qrc', '.ui', '.moc', '.svn']
    for root, dirs, files in os.walk(dir):
        if root.split('/')[-1] in filters:
            continue
        for f in files:
            fullPath = os.path.join(root, f)
            dependList = getDependList(fullPath)
            dependList = [item.split('/')[-1] for item in dependList if not isQtFramework(item) and not isSystemFramework(item)]
            allDependFiles.extend(dependList)
    return set(allDependFiles)

def copyBundleFiles(dir):
    allDepends = getAllDepends(dir)
    filters = ['.obj', '.qrc', '.ui', '.moc', '.svn']
    for root, dirs, files in os.walk(dir):
        if root.split('/')[-1] in filters:
            continue
        for f in files:
            if f.endswith('.dylib'):
                if f in allDepends:
                    copyFile( os.path.join(root, f) )
            else:
                copyFile(os.path.join(root, f))

def deployQtFrameworks():
    os.chdir(bundleRoot)
    os.system("macdeployqt %s" % bundleApp) 
    os.chdir('../../')
    path = '%s/%s/Contents/PlugIns/sqldrivers' % (bundleRoot, bundleApp) 
    os.system('mkdir -p %s' % path) 
    os.system('cp /Developer/Applications/Qt/plugins/sqldrivers/libqsqlite.dylib %s' %  path)

def updateInstallName():
    bundleDir = '%s/%s/Contents/MacOS' % (bundleRoot, bundleApp)
    for root, dirs, files in os.walk(bundleDir):
        for f in files:
            base = os.path.join(root, f)
            if not f.endswith('.dylib') and f != bundleExe:
                continue
            print 'now checking:', f
            dependList = getDependList(base)
            for src in dependList:
                if isSystemFramework(src):
                    pass
                elif isQtFramework(src):
                    if not src.startswith('@executable_path'):
                        os.system('install_name_tool -change %s @executable_path/../Frameworks/%s %s' % (src, src, base))
                elif isCoreLibrary(src):
                    os.system('install_name_tool -change %s @executable_path/%s %s' % (src, src.split('/')[-1], base))
                elif isPluginLibrary(src):
                    os.system('install_name_tool -change %s @executable_path/plugin/%s %s' % (src, src.split('/')[-1], base)) 
                else:
                    pass

def updatePlist():
    os.system('cp macinfo.plist %s/%s/Contents/Info.plist' % (bundleRoot, bundleApp))
    os.system('cp macentitlements.plist %s/%s/Contents/Entitlements.plist' % (bundleRoot, bundleApp))


if __name__ == '__main__':
    print 'Clearning...'
    os.system("rm -rf dist/mac/RecMind.app")

    print 'Making dirs...'
    makeBundleDirs()

    print 'Copying bundle files...'
    copyBundleFiles(bundleBuildDir)
    
    print 'Deploying the Qt Framework...'
    deployQtFrameworks()

    print 'Updating the shared library install name...'
    updateInstallName()

    print 'updating plist files...'
    updatePlist()

    print 'Job done!'
