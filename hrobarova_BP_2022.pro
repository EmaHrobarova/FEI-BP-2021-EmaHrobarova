QT += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS  += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#sensor
unix:!macx: LIBS += -L$$PWD/../Plocha/BioMiniSDK_for_Linux_v3.6.0.1320/bin/x64/ -lUFScanner
INCLUDEPATH += $$PWD/../Plocha/BioMiniSDK_for_Linux_v3.6.0.1320/include
DEPENDPATH += $$PWD/../Plocha/BioMiniSDK_for_Linux_v3.6.0.1320/include

#openfinger preprocessor
unix:!macx: LIBS += -L$$PWD/../Plocha/build-openfinger-preprocessor-qt5-Debug/ -lofpreproc
INCLUDEPATH += $$PWD/../Plocha/openfinger-preprocessor
DEPENDPATH += $$PWD/../Plocha/openfinger-preprocessor

#openfinger extractor
unix:!macx: LIBS += -L$$PWD/../Plocha/build-Extraction-qt5-Debug/ -lExtraction
INCLUDEPATH += $$PWD/../Plocha/openfinger-extractor
DEPENDPATH += $$PWD/../Plocha/openfinger-extractor

#openfinger matcher
unix:!macx: LIBS += -L$$PWD/../Plocha/build-Matcher-qt5-Debug/ -lMatcher
INCLUDEPATH += $$PWD/../Plocha/openfinger-matcher
DEPENDPATH += $$PWD/../Plocha/openfinger-matcher


# CUDA - Manjaro (as a package)
unix:!macx: LIBS += -L/opt/cuda/lib64 -lcudart
INCLUDEPATH += /opt/cuda/include
DEPENDPATH += /opt/cuda/include

#ArrayFire - Manjaro (as a package)
unix:!macx: LIBS += -L/usr/lib/ -lafcuda
INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include

#OpenCV - Manjaro (as a package)
unix:!macx: LIBS += -L/usr/lib/ -lopencv_core
unix:!macx: LIBS += -L/usr/lib/ -lopencv_imgproc
unix:!macx: LIBS += -L/usr/lib/ -lopencv_imgcodecs
unix:!macx: LIBS += -L/usr/lib/ -lopencv_highgui
INCLUDEPATH += /usr/include/opencv4
DEPENDPATH += /usr/include/opencv4

#Caffe - Manjaro (built from source)
unix:!macx: LIBS += -L/usr/lib/libcaffe.so -lcaffe
INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

#glog, protobuf, boost - Manjaro (as a package)
unix:!macx: LIBS += -L/usr/lib/ -lglog
unix:!macx: LIBS += -L/usr/lib/ -lprotobuf
unix:!macx: LIBS += -L/usr/lib/ -lboost_system
INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include


