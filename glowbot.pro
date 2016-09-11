purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= qt release warn_on
DEFINES		+=
LANGUAGE	= C++
QT		+= gui sql

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets
}

TEMPLATE	= app

QMAKE_CLEAN	+= GlowBot
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -fPIE -fstack-protector-all -fwrapv \
                          -mtune=generic -pie -O3 \
                          -Wall -Wcast-align -Wcast-qual \
                          -Werror -Wextra \
                          -Woverloaded-virtual -Wpointer-arith \
                          -Wstack-protector -Wstrict-overflow=5

lessThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE -= -Werror
}

QMAKE_DISTCLEAN += -r temp
QMAKE_EXTRA_TARGETS = purge
QMAKE_STRIP	= echo

ICON		=
INCLUDEPATH	+= Source
LIBS		+=
RESOURCES	= Icons/icons.qrc

MOC_DIR = temp/moc
OBJECTS_DIR = temp/obj
RCC_DIR = temp/rcc
UI_DIR = temp/ui

FORMS           = UI\\glowbot-canvas-settings.ui \
                  UI\\glowbot-mainwindow.ui \
                  UI\\glowbot-object-analog-read-arduino.ui \
		  UI\\glowbot-object-loop-arduino.ui \
                  UI\\glowbot-object-setup-arduino.ui \
		  UI\\glowbot-object-start.ui \
		  UI\\glowbot-structures.ui \
		  UI\\glowbot-view.ui

HEADERS		= Source\\glowbot-canvas-settings.h \
                  Source\\glowbot-object.h \
                  Source\\glowbot-object-analog-read-arduino.h \
		  Source\\glowbot-object-loop-arduino.h \
		  Source\\glowbot-object-setup-arduino.h \
		  Source\\glowbot-object-start.h \
		  Source\\glowbot-object-view.h \
                  Source\\glowbot-proxy-widget.h \
                  Source\\glowbot-scene.h \
                  Source\\glowbot-structures-arduino.h \
                  Source\\glowbot-tab.h \
                  Source\\glowbot-ui.h \
                  Source\\glowbot-view.h \
		  Source\\glowbot-view-arduino.h

SOURCES		= Source\\glowbot-canvas-settings.cc \
		  Source\\glowbot-common.cc \
                  Source\\glowbot-main.cc \
                  Source\\glowbot-misc.cc \
                  Source\\glowbot-object.cc \
                  Source\\glowbot-object-analog-read-arduino.cc \
		  Source\\glowbot-object-loop-arduino.cc \
		  Source\\glowbot-object-setup-arduino.cc \
		  Source\\glowbot-object-start.cc \
		  Source\\glowbot-object-view.cc \
                  Source\\glowbot-proxy-widget.cc \
                  Source\\glowbot-scene.cc \
                  Source\\glowbot-structures-arduino.cc \
                  Source\\glowbot-tab.cc \
                  Source\\glowbot-ui-a.cc \
                  Source\\glowbot-view.cc \
		  Source\\glowbot-view-arduino.cc

TRANSLATIONS    =

PROJECTNAME	= GlowBot
TARGET		= GlowBot
