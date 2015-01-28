purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= qt release thread warn_on
DEFINES		+=
LANGUAGE	= C++
QT		+= gui sql
TEMPLATE	= app

QMAKE_CLEAN	+= SparX
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -fPIE -fstack-protector-all -fwrapv \
                          -mtune=generic -pie -O3 \
                          -Wall -Wcast-align -Wcast-qual \
                          -Werror -Wextra \
                          -Woverloaded-virtual -Wpointer-arith \
                          -Wstack-protector -Wstrict-overflow=5
QMAKE_EXTRA_TARGETS = purge
QMAKE_STRIP	= echo

ICON		=
INCLUDEPATH	+= Include
LIBS		+=
RESOURCES	=

FORMS           = UI/main.ui

UI_HEADERS_DIR  = Include

HEADERS		= Include/sparx-misc.h \
                  Include/sparx-ui.h

SOURCES		= Source/sparx-main.cc \
                  Source/sparx-misc.cc \
                  Source/sparx-ui.cc

TRANSLATIONS    =

PROJECTNAME	= SparX
TARGET		= SparX
