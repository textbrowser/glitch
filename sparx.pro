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

FORMS           =

UI_HEADERS_DIR  = Include

HEADERS		=

SOURCES		=

TRANSLATIONS    =

PROJECTNAME	= SparX
TARGET		= SparX
