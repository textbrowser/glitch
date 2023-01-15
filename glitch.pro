include(Source/glitch-arduino.pro)

doxygen.commands = doxygen glitch.doxygen
purge.commands = find . -name '*~' -exec rm {} \\;

CONFIG		+= qt release warn_on
DEFINES		+= QT_DEPRECATED_WARNINGS
LANGUAGE	= C++
QMAKE_CLEAN	+= Glitch
QT		+= gui printsupport network sql widgets

contains(QMAKE_HOST.arch, armv7l) {
QMAKE_CXXFLAGS_RELEASE += -march=armv7
}

contains(QMAKE_HOST.arch, ppc) {
QMAKE_CXXFLAGS_RELEASE += -march=powerpc
}

qtHaveModule(pdf) {
qtHaveModule(pdfwidgets) {
DEFINES         += GLITCH_PDF_SUPPORTED
QT              += pdf pdfwidgets
message("The QtPdf module was discovered!")
}
}

QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE -= -O2

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

QMAKE_DISTCLEAN     += -r .qmake* \
                       -r html \
                       -r latex \
                       -r temp
QMAKE_EXTRA_TARGETS = doxygen purge
QMAKE_STRIP	    = echo

macx {
ICON		= Icons/Logo/glitch-logo.icns
}

win32 {
RC_FILE         = Icons/glitch.rc
}

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
RESOURCES   = Documentation/documentation.qrc \
              Icons/icons.qrc
UI_DIR      = temp/ui

FORMS          += UI/glitch-canvas-settings.ui \
                  UI/glitch-documentation.ui \
                  UI/glitch-errors-dialog.ui \
                  UI/glitch-find-objects.ui \
                  UI/glitch-floating-context-menu.ui \
                  UI/glitch-mainwindow.ui \
                  UI/glitch-port-colors.ui \
                  UI/glitch-preferences.ui \
		  UI/glitch-separated-diagram-window.ui \
                  UI/glitch-structures.ui \
                  UI/glitch-style-sheet.ui \
                  UI/glitch-tools.ui \
                  UI/glitch-user-functions.ui \
		  UI/glitch-view.ui
HEADERS	       += Source/glitch-alignment.h \
                  Source/glitch-canvas-settings.h \
                  Source/glitch-collapse-expand-tool-button.h \
                  Source/glitch-documentation.h \
                  Source/glitch-find-objects.h \
                  Source/glitch-floating-context-menu.h \
                  Source/glitch-font-filter.h \
                  Source/glitch-graphicsview.h \
                  Source/glitch-object.h \
                  Source/glitch-object-edit-window.h \
                  Source/glitch-object-view.h \
                  Source/glitch-port-colors.h \
                  Source/glitch-preferences.h \
                  Source/glitch-proxy-widget.h \
                  Source/glitch-recent-diagram.h \
                  Source/glitch-scene.h \
                  Source/glitch-scroll-filter.h \
                  Source/glitch-separated-diagram-window.h \
                  Source/glitch-structures-treewidget.h \
                  Source/glitch-style-sheet.h \
                  Source/glitch-swifty.h \
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
                  Source/glitch-find-objects.cc \
                  Source/glitch-floating-context-menu.cc \
                  Source/glitch-main.cc \
                  Source/glitch-misc.cc \
                  Source/glitch-object.cc \
                  Source/glitch-object-edit-window.cc \
                  Source/glitch-object-view.cc \
                  Source/glitch-port-colors.cc \
                  Source/glitch-preferences.cc \
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

# Installation Procedures

macx {
chown.extra		= chown -Rh root:wheel /Applications/Glitch.d
chown.path		= /Applications/Glitch.d
documentation1.extra	= cp ./Documentation/*.1 /Applications/Glitch.d/Documentation/.
documentation1.path	= /Applications/Glitch.d/Documentation
documentation2.extra    = cp ./Documentation/*.pdf /Applications/Glitch.d/Documentation/.
documentation2.path     = /Applications/Glitch.d/Documentation
documentation3.extra	= cp ./TO-DO /Applications/Glitch.d/Documentation/.
documentation3.path	= /Applications/Glitch.d/Documentation
documentation4.extra	= cp -r ./Documentation/Arduino /Applications/Glitch.d/Documentation/.
documentation4.path	= /Applications/Glitch.d/Documentation
glitch.extra	        = cp -r ./Glitch.app /Applications/Glitch.d/.
glitch.path		= /Applications/Glitch.d
macdeployqt.extra	= $$[QT_INSTALL_BINS]/macdeployqt /Applications/Glitch.d/Glitch.app -executable=/Applications/Glitch.d/Glitch.app/Contents/MacOS/Glitch
macdeployqt.path	= Glitch.app
preinstall.extra	= rm -fr /Applications/Glitch.d/Glitch.app
preinstall.path		= /Applications/Glitch.d

INSTALLS = documentation1 \
           documentation2 \
           documentation3 \
           documentation4 \
           preinstall \
           glitch \
           macdeployqt \
           chown
}

win32 {
documentation1.files = Documentation\\*.pdf
documentation1.path  = release\\Documentation\\.
documentation2.files = Documentation\\Arduino\\*
documentation2.path  = release\\Documentation\\.
documentation3.files = TO-DO
documentation3.path  = release\\Documentation\\.
examples.files      = Examples
examples.path       = release
plugins1.files      = $$[QT_INSTALL_PLUGINS]\\*
plugins1.path       = release\\plugins\\.
plugins2.files      = $$[QT_INSTALL_PLUGINS]\\gamepads\\xinputgamepad.dll
plugins2.path       = release\\plugins\\gamepads\\.
plugins3.files      = $$[QT_INSTALL_PLUGINS]\\platforms\\qdirect2d.dll
plugins3.path       = release\\plugins\\platforms\\.
plugins4.files      = $$[QT_INSTALL_PLUGINS]\\renderplugins\\scene2d.dll
plugins4.path       = release\\plugins\\renderplugins\\.
pluginspurge.extra  = del /Q /S *d.dll
pluginspurge.path   = release\\plugins\\.
qt.files            = Qt\\qt.conf
qt.path             = release\\.
qtlibraries.files   = $$[QT_INSTALL_BINS]\\Qt5Core.dll \
                      $$[QT_INSTALL_BINS]\\Qt5Gui.dll \
                      $$[QT_INSTALL_BINS]\\Qt5PrintSupport.dll \
                      $$[QT_INSTALL_BINS]\\Qt5Sql.dll \
                      $$[QT_INSTALL_BINS]\\Qt5Widgets.dll \
                      $$[QT_INSTALL_BINS]\\libgcc_s_dw2-1.dll \
                      $$[QT_INSTALL_BINS]\\libstdc++-6.dll \
                      $$[QT_INSTALL_BINS]\\libwinpthread-1.dll
qtlibraries.path    = release\\.

INSTALLS = documentation1 \
           documentation2 \
           documentation3 \
           examples \
           plugins1 \
           pluginspurge \
           plugins2 \
           plugins3 \
           plugins4 \
           qt \
           qtlibraries
}
