include(Source/glitch-arduino.pro)

doxygen.commands = doxygen glitch.doxygen
purge.commands = find . -name '*~' -exec rm {} \\;

CONFIG		+= qt release warn_on
DEFINES		+= QT_DEPRECATED_WARNINGS
LANGUAGE	= C++
QT		+= gui printsupport sql widgets

QMAKE_CLEAN	+= Glitch

macx {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -Wzero-as-null-pointer-constant \
                          -fPIE \
                          -fstack-protector-all \
                          -fwrapv \
                          -pedantic \
                          -std=c++11
} else:win32 {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Wl,-z,relro \
                          -Wno-class-memaccess \
                          -Wno-deprecated-copy \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -Wzero-as-null-pointer-constant \
                          -fPIE \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++11
} else {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Wl,-z,relro \
                          -Wlogical-op \
                          -Wno-class-memaccess \
                          -Wno-deprecated-copy \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++11
}

greaterThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE += -std=c++17
QMAKE_CXXFLAGS_RELEASE -= -std=c++11
}

QMAKE_CXXFLAGS_RELEASE += -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_DISTCLEAN += -r .qmake* \
                   -r html \
                   -r latex \
                   -r temp
QMAKE_EXTRA_TARGETS = doxygen purge
QMAKE_STRIP	= echo

ICON		=
INCLUDEPATH	+= Source

macx {
LIBS            += -framework AppKit -framework Cocoa
} else {
LIBS		+=
}

macx {
OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm
}

MOC_DIR     = temp/moc
OBJECTS_DIR = temp/obj
RCC_DIR     = temp/rcc
RESOURCES   = Icons/icons.qrc
UI_DIR      = temp/ui

FORMS          += UI/glitch-canvas-settings.ui \
                  UI/glitch-documentation.ui \
                  UI/glitch-errors-dialog.ui \
                  UI/glitch-floating-context-menu.ui \
                  UI/glitch-mainwindow.ui \
		  UI/glitch-separated-diagram-window.ui \
                  UI/glitch-structures.ui \
                  UI/glitch-style-sheet.ui \
                  UI/glitch-tools.ui \
                  UI/glitch-user-functions.ui \
		  UI/glitch-view.ui
HEADERS	       += Source/glitch-alignment.h \
                  Source/glitch-canvas-settings.h \
                  Source/glitch-documentation.h \
                  Source/glitch-floating-context-menu.h \
                  Source/glitch-graphicsview.h \
                  Source/glitch-object.h \
                  Source/glitch-object-edit-window.h \
                  Source/glitch-object-view.h \
                  Source/glitch-proxy-widget.h \
                  Source/glitch-recent-diagram.h \
                  Source/glitch-scene.h \
                  Source/glitch-separated-diagram-window.h \
                  Source/glitch-structures-treewidget.h \
                  Source/glitch-style-sheet.h \
                  Source/glitch-tab.h \
                  Source/glitch-tab-tabbar.h \
                  Source/glitch-tools.h \
                  Source/glitch-ui.h \
                  Source/glitch-user-functions.h \
                  Source/glitch-user-functions-tablewidget.h \
                  Source/glitch-view.h \
                  Source/glitch-wire.h
SOURCES	       += Source/glitch-alignment.cc \
                  Source/glitch-canvas-settings.cc \
                  Source/glitch-common.cc \
                  Source/glitch-documentation.cc \
                  Source/glitch-graphicsview.cc \
                  Source/glitch-floating-context-menu.cc \
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
                  Source/glitch-tools.cc \
                  Source/glitch-ui-a.cc \
                  Source/glitch-undo-command.cc \
                  Source/glitch-user-functions.cc \
                  Source/glitch-user-functions-tablewidget.cc \
                  Source/glitch-view.cc \
		  Source/glitch-wire.cc
TRANSLATIONS    =

PROJECTNAME	= Glitch
TARGET		= Glitch
TEMPLATE	= app
