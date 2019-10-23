include(Source/glitch-arduino.pro)

doxygen.commands = doxygen glitch.doxygen
purge.commands = find . -name '*~' -exec rm {} \\;

CONFIG		+= qt release warn_on
DEFINES		+=
LANGUAGE	= C++
QT		+= gui sql

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets
}

TEMPLATE	= app

QMAKE_CLEAN	+= Glitch

macx {
QMAKE_CXXFLAGS_RELEASE += -fPIE -fstack-protector-all -fwrapv \
                          -mtune=generic -pedantic -std=c++11 \
                          -Wall -Wcast-align -Wcast-qual \
                          -Wextra \
                          -Woverloaded-virtual -Wpointer-arith \
                          -Wstack-protector -Wstrict-overflow=5
} else {
QMAKE_CXXFLAGS_RELEASE += -fPIE -fstack-protector-all -fwrapv \
                          -mtune=generic -pedantic -pie -std=c++11 \
                          -Wall -Wcast-align -Wcast-qual \
                          -Wextra \
                          -Wno-class-memaccess \
                          -Woverloaded-virtual -Wno-deprecated-copy \
                          -Wpointer-arith \
                          -Wstack-protector -Wstrict-overflow=5
}

QMAKE_DISTCLEAN += -r html -r latex -r temp
QMAKE_EXTRA_TARGETS = doxygen purge
QMAKE_STRIP	= echo

ICON		=
INCLUDEPATH	+= Source

macx {
LIBS            += -framework AppKit -framework Cocoa
} else {
LIBS		+=
}

RESOURCES	= Icons/icons.qrc

MOC_DIR = temp/moc

macx {
OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm
}

OBJECTS_DIR = temp/obj
RCC_DIR = temp/rcc
UI_DIR = temp/ui

FORMS          += UI/glitch-alignment.ui \
                  UI/glitch-canvas-settings.ui \
		  UI/glitch-errors-dialog.ui \
                  UI/glitch-mainwindow.ui \
		  UI/glitch-separated-diagram-window.ui \
                  UI/glitch-structures.ui \
                  UI/glitch-style-sheet.ui \
                  UI/glitch-user-functions.ui \
		  UI/glitch-view.ui

HEADERS	       += Source/glitch-alignment.h \
                  Source/glitch-canvas-settings.h \
                  Source/glitch-graphicsview.h \
                  Source/glitch-object.h \
                  Source/glitch-object-edit-window.h \
		  Source/glitch-object-view.h \
                  Source/glitch-proxy-widget.h \
                  Source/glitch-scene.h \
                  Source/glitch-separated-diagram-window.h \
                  Source/glitch-structures-treewidget.h \
                  Source/glitch-style-sheet.h \
                  Source/glitch-tab.h \
                  Source/glitch-tab-tabbar.h \
                  Source/glitch-ui.h \
                  Source/glitch-user-functions.h \
                  Source/glitch-user-functions-tablewidget.h \
                  Source/glitch-view.h

SOURCES	       += Source/glitch-alignment.cc \
                  Source/glitch-canvas-settings.cc \
                  Source/glitch-common.cc \
                  Source/glitch-graphicsview.cc \
                  Source/glitch-main.cc \
                  Source/glitch-misc.cc \
                  Source/glitch-object.cc \
                  Source/glitch-object-edit-window.cc \
		  Source/glitch-object-view.cc \
                  Source/glitch-proxy-widget.cc \
                  Source/glitch-scene.cc \
                  Source/glitch-separated-diagram-window.cc \
                  Source/glitch-structures-treewidget.cc \
                  Source/glitch-style-sheet.cc \
                  Source/glitch-tab.cc \
                  Source/glitch-tab-tabbar.cc \
                  Source/glitch-ui-a.cc \
                  Source/glitch-undo-command.cc \
                  Source/glitch-user-functions.cc \
                  Source/glitch-user-functions-tablewidget.cc \
                  Source/glitch-view.cc \

TRANSLATIONS    =

PROJECTNAME	= Glitch
TARGET		= Glitch
