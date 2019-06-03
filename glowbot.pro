include(Source/glowbot-arduino.pro)

doxygen.commands = doxygen glowbot.doxygen
purge.commands = find . -name '*~' -exec rm {} \\;

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
                          -mtune=generic -pedantic -pie -std=c++11 \
			  -O3 \
                          -Wall -Wcast-align -Wcast-qual \
                          -Werror -Wextra \
                          -Wno-class-memaccess \
                          -Woverloaded-virtual -Wpointer-arith \
                          -Wstack-protector -Wstrict-overflow=5

lessThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE -= -Werror
}
else {
QMAKE_CXXFLAGS_RELEASE += -Wzero-as-null-pointer-constant
}

QMAKE_DISTCLEAN += -r html -r latex -r temp
QMAKE_EXTRA_TARGETS = doxygen purge
QMAKE_STRIP	= echo

ICON		=
INCLUDEPATH	+= Source
LIBS		+=
RESOURCES	= Icons/icons.qrc

MOC_DIR = temp/moc
OBJECTS_DIR = temp/obj
RCC_DIR = temp/rcc
UI_DIR = temp/ui

FORMS          += UI/glowbot-alignment.ui \
                  UI/glowbot-canvas-settings.ui \
		  UI/glowbot-errors-dialog.ui \
                  UI/glowbot-mainwindow.ui \
		  UI/glowbot-separated-diagram-window.ui \
                  UI/glowbot-structures.ui \
                  UI/glowbot-style-sheet.ui \
                  UI/glowbot-user-functions.ui \
		  UI/glowbot-view.ui

HEADERS	       += Source/glowbot-alignment.h \
                  Source/glowbot-canvas-settings.h \
                  Source/glowbot-graphicsview.h \
                  Source/glowbot-object.h \
                  Source/glowbot-object-edit-window.h \
		  Source/glowbot-object-view.h \
                  Source/glowbot-proxy-widget.h \
                  Source/glowbot-scene.h \
                  Source/glowbot-separated-diagram-window.h \
                  Source/glowbot-structures-treewidget.h \
                  Source/glowbot-style-sheet.h \
                  Source/glowbot-tab.h \
                  Source/glowbot-tab-tabbar.h \
                  Source/glowbot-ui.h \
                  Source/glowbot-user-functions.h \
                  Source/glowbot-user-functions-tablewidget.h \
                  Source/glowbot-view.h

SOURCES	       += Source/glowbot-alignment.cc \
                  Source/glowbot-canvas-settings.cc \
                  Source/glowbot-common.cc \
                  Source/glowbot-graphicsview.cc \
                  Source/glowbot-main.cc \
                  Source/glowbot-misc.cc \
                  Source/glowbot-object.cc \
                  Source/glowbot-object-edit-window.cc \
		  Source/glowbot-object-view.cc \
                  Source/glowbot-proxy-widget.cc \
                  Source/glowbot-scene.cc \
                  Source/glowbot-separated-diagram-window.cc \
                  Source/glowbot-structures-treewidget.cc \
                  Source/glowbot-style-sheet.cc \
                  Source/glowbot-tab.cc \
                  Source/glowbot-tab-tabbar.cc \
                  Source/glowbot-ui-a.cc \
                  Source/glowbot-undo-command.cc \
                  Source/glowbot-user-functions.cc \
                  Source/glowbot-user-functions-tablewidget.cc \
                  Source/glowbot-view.cc \

TRANSLATIONS    =

PROJECTNAME	= GlowBot
TARGET		= GlowBot
