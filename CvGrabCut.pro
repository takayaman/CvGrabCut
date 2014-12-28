INCLUDEPATH += /home/takayaman/opencv3/build/include \
  /home/takayaman/opencv3/build/include/opencv \
  /home/takayaman/opencv3/build/include/opencv2 \
  /usr/include

LIBS += `pkg-config --libs opencv` \
  -L/home/takayaman/opencv3/build/lib \
  -L/usr/lib/x86_64-linux-gnu \
  -lglog

QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    GrabCut.h \
    App.h \
    featurevertex.h \
    featuregraph.h \
    featureedge.h \
    gmm.h \
    gcgraph.h

SOURCES += \
    main.cpp \
    GrabCut.cpp \
    App.cpp \
    featurevertex.cpp \
    featuregraph.cpp \
    featureedge.cpp \
    gmm.cpp \
    gcgraph.cpp
