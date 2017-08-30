from distutils.core import setup, Extension

SCRATCHPy = Extension('SCRATCHPy',
                    #include_dirs = [
                    #'/usr/include/python3.4m',
                    #'/usr/include/arm-linux-gnueabihf/qt5/', '/usr/include/arm-linux-gnueabihf/qt5/QtCore/', '/usr/include/arm-linux-gnueabihf/qt5/QtGui'],
                    include_dirs = ['/usr/include/python3.6m', '/usr/include/qt/', '/usr/include/qt/QtCore', '../libSCRATCHy/'],
                    sources = [
                    'scratchpy.cpp', 'graphicaldisplay.cpp',
                    'signalgenerator.cpp', 'signalmanager.cpp',
                    'converters.cpp', 'datatypes.cpp'],
                    library_dirs=[ '../build-libSCRATCHy/' ],
                    libraries = ['boost_python3', 'SCRATCHy'],
                    extra_compile_args=['-std=c++11'])

setup (name = 'SCRATCHPy Interface',
       version = '.1',
       description = 'Python bindings for the SCRATCHy library',
       ext_modules = [SCRATCHPy])
