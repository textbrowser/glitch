include(Source/glitch-arduino.pro)

macx {
dmg.commands = make install && hdiutil create Glitch.d.dmg -srcfolder Glitch.d
QMAKE_EXTRA_TARGETS += dmg
}

unix {
purge.commands = find . -name \'*~\' -exec rm {} \;
QMAKE_EXTRA_TARGETS += purge
}

CONFIG		+= qt release warn_on
DEFINES         += GLITCH_FUTURE QT_SHA3_KECCAK_COMPAT
LANGUAGE	= C++
QMAKE_CLEAN	+= Glitch
QT		+= concurrent gui network printsupport sql widgets

contains(QMAKE_HOST.arch, armv7l) {
QMAKE_CXXFLAGS_RELEASE += -march=armv7
}

contains(QMAKE_HOST.arch, ppc) {
QMAKE_CXXFLAGS_RELEASE += -mcpu=powerpc -mtune=powerpc
}

qtHaveModule(pdf) {
qtHaveModule(pdfwidgets) {
DEFINES         += GLITCH_PDF_SUPPORTED
QT              += pdf pdfwidgets
message("The QtPdf and QtPdfWidgets modules were discovered!")
} else {
warning("The QtPdfWidgets module was not found.")
}
} else {
warning("The QtPdf module was not found.")
}

qtHaveModule(serialport) {
DEFINES         += GLITCH_SERIAL_PORT_SUPPORTED
QT              += serialport
message("The QtSerialPort module was discovered!")
} else {
warning("The QtSerialPort module was not found.")
}

QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE -= -O2

android {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wenum-compare \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Wno-c++20-attribute-extensions \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIC \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++17
} else:freebsd-* {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++17
} else:macx {
QMAKE_APPLE_DEVICE_ARCHS = arm64 x86_64
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Wno-c++20-attribute-extensions \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++17
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
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
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIE \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
} else {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Wformat-overflow=2 \
                          -Wl,-z,relro \
                          -Wlogical-op \
                          -Wno-class-memaccess \
                          -Wno-deprecated-copy \
                          -Wold-style-cast \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wstringop-overflow=4 \
                          -Wundef \
                          -Wzero-as-null-pointer-constant \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
contains(QMAKE_HOST.arch, ppc) {
QMAKE_CXXFLAGS_RELEASE -= -Wformat-overflow=2 \
                          -Wstringop-overflow=4
}
}

greaterThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE += -std=c++17
QMAKE_CXXFLAGS_RELEASE -= -std=c++11
}

macx {
QMAKE_DISTCLEAN     += -r Glitch.d
}

win32 {
QMAKE_DISTCLEAN     += del /F /Q .qmake* \
                       rmdir /Q /S Temporary \
                       rmdir /Q /S debug \
                       rmdir /Q /S release
} else {
QMAKE_DISTCLEAN     += -f .qmake* \
                       -fr Temporary
}

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
OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm
} else {
LIBS		+=
}

MOC_DIR     = Temporary/moc
OBJECTS_DIR = Temporary/obj
RCC_DIR     = Temporary/rcc
RESOURCES   = Documentation/documentation.qrc Icons/icons.qrc
UI_DIR      = Temporary/ui

FORMS          += UI/glitch-ash.ui \
                  UI/glitch-canvas-preview.ui \
                  UI/glitch-canvas-settings.ui \
                  UI/glitch-documentation.ui \
                  UI/glitch-docked-container.ui \
                  UI/glitch-errors-dialog.ui \
                  UI/glitch-find-objects.ui \
                  UI/glitch-floating-context-menu.ui \
                  UI/glitch-mainwindow.ui \
                  UI/glitch-port-colors.ui \
                  UI/glitch-preferences.ui \
                  UI/glitch-redo-undo-stack.ui \
                  UI/glitch-separated-diagram-window.ui \
                  UI/glitch-serial-port-window.ui \
                  UI/glitch-source-preview.ui \
                  UI/glitch-structures.ui \
                  UI/glitch-style-sheet.ui \
                  UI/glitch-tools.ui \
                  UI/glitch-user-functions.ui \
		  UI/glitch-view.ui
HEADERS	       += Source/glitch-alignment.h \
                  Source/glitch-application.h \
                  Source/glitch-ash.h \
                  Source/glitch-aware-containers.h \
                  Source/glitch-canvas-preview.h \
                  Source/glitch-canvas-settings.h \
                  Source/glitch-collapse-expand-tool-button.h \
                  Source/glitch-documentation.h \
                  Source/glitch-docked-container.h \
                  Source/glitch-find-objects.h \
                  Source/glitch-find-objects-position-item.h \
                  Source/glitch-floating-context-menu.h \
                  Source/glitch-font-filter.h \
                  Source/glitch-graphicsview.h \
                  Source/glitch-object.h \
                  Source/glitch-object-arrow.h \
                  Source/glitch-object-edit-window.h \
                  Source/glitch-object-lineedit.h \
                  Source/glitch-object-view.h \
                  Source/glitch-port-colors.h \
                  Source/glitch-preferences.h \
                  Source/glitch-proxy-widget.h \
                  Source/glitch-recent-diagram.h \
                  Source/glitch-recent-diagrams-view.h \
                  Source/glitch-redo-undo-stack.h \
                  Source/glitch-resize-widget.h \
                  Source/glitch-resize-widget-rectangle.h \
                  Source/glitch-scene.h \
                  Source/glitch-scroll-filter.h \
                  Source/glitch-separated-diagram-window.h \
                  Source/glitch-serial-port-window.h \
                  Source/glitch-source-preview.h \
                  Source/glitch-structures-treewidget.h \
                  Source/glitch-style-sheet.h \
                  Source/glitch-syntax-highlighter.h \
                  Source/glitch-swifty.h \
                  Source/glitch-tab.h \
                  Source/glitch-tab-tabbar.h \
                  Source/glitch-tools.h \
                  Source/glitch-ui.h \
                  Source/glitch-user-functions.h \
                  Source/glitch-user-functions-model.h \
                  Source/glitch-user-functions-tableview.h \
                  Source/glitch-view.h \
                  Source/glitch-wire.h
SOURCES	       += Source/glitch-alignment.cc \
                  Source/glitch-application.cc \
                  Source/glitch-ash.cc \
                  Source/glitch-canvas-preview.cc \
                  Source/glitch-canvas-settings.cc \
                  Source/glitch-common.cc \
                  Source/glitch-documentation.cc \
                  Source/glitch-docked-container.cc \
                  Source/glitch-graphicsview.cc \
                  Source/glitch-find-objects.cc \
                  Source/glitch-floating-context-menu.cc \
                  Source/glitch-main.cc \
                  Source/glitch-object.cc \
                  Source/glitch-object-arrow.cc \
                  Source/glitch-object-edit-window.cc \
                  Source/glitch-object-edit-window-ash.cc \
                  Source/glitch-object-view.cc \
                  Source/glitch-port-colors.cc \
                  Source/glitch-preferences.cc \
                  Source/glitch-proxy-widget.cc \
                  Source/glitch-recent-diagrams-view.cc \
                  Source/glitch-redo-undo-stack.cc \
                  Source/glitch-resize-widget.cc \
                  Source/glitch-resize-widget-rectangle.cc \
                  Source/glitch-scene.cc \
                  Source/glitch-separated-diagram-window.cc \
                  Source/glitch-serial-port-window.cc \
                  Source/glitch-source-preview.cc \
                  Source/glitch-structures-treewidget.cc \
                  Source/glitch-style-sheet.cc \
                  Source/glitch-syntax-highlighter.cc \
                  Source/glitch-tab.cc \
                  Source/glitch-tab-tabbar.cc \
                  Source/glitch-tools.cc \
                  Source/glitch-ui-a.cc \
                  Source/glitch-ui-ash.cc \
                  Source/glitch-ui-b.cc \
                  Source/glitch-undo-command.cc \
                  Source/glitch-user-functions-model.cc \
                  Source/glitch-user-functions-tableview.cc \
                  Source/glitch-user-functions.cc \
                  Source/glitch-variety.cc \
                  Source/glitch-view-ash.cc \
                  Source/glitch-view.cc \
		  Source/glitch-wire.cc
TRANSLATIONS    =

PROJECTNAME	= Glitch
TARGET		= Glitch
TEMPLATE	= app

android {
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
DISTFILES += Android/AndroidManifest.xml \
             Android/build.gradle \
             Android/res/values/libs.xml

android: include(/home/saturn/Android/Sdk/android_openssl/openssl.pri)
}

# Installation Procedures

android {
examples.depends += FORCE
examples.files    = Examples/Arduino/*.db
examples.path     = /assets/Examples

INSTALLS += examples
}

macx {
documentation1.extra	= cp ./Documentation/*.1 Glitch.d/Documentation/.
documentation1.path	= Glitch.d/Documentation
documentation2.extra    = cp ./Documentation/*.pdf Glitch.d/Documentation/.
documentation2.path     = Glitch.d/Documentation
documentation3.extra	= cp ./REMINDERS Glitch.d/Documentation/.
documentation3.path	= Glitch.d/Documentation
documentation4.extra	= cp -r ./Documentation/Arduino Glitch.d/Documentation/.
documentation4.path	= Glitch.d/Documentation
examples.extra          = cp -r ./Examples Glitch.d/.
examples.path           = Glitch.d/Examples
glitch.extra	        = cp -r ./Glitch.app Glitch.d/.
glitch.path		= Glitch.d
macdeployqt.extra	= $$[QT_INSTALL_BINS]/macdeployqt Glitch.d/Glitch.app \
                          -executable=Glitch.d/Glitch.app/Contents/MacOS/Glitch
macdeployqt.path	= Glitch.app
preinstall.extra	= rm -fr Glitch.d/Glitch.app
preinstall.path		= Glitch.d

INSTALLS = documentation1 \
           documentation2 \
           documentation3 \
           documentation4 \
           examples \
           preinstall \
           glitch \
           macdeployqt
}

win32 {
documentation1.files = Documentation\\*.pdf
documentation1.path  = release\\Documentation\\.
documentation2.files = Documentation\\Arduino\\*
documentation2.path  = release\\Documentation\\.
documentation3.files = REMINDERS
documentation3.path  = release\\Documentation\\.
examples.files       = Examples
examples.path        = release
plugins.files        = $$[QT_INSTALL_PLUGINS]\\*
plugins.path         = release\\plugins\\.
qt.files             = Qt\\qt.conf
qt.path              = release\\.
qtlibraries.files    = $$[QT_INSTALL_BINS]\\Qt*Core.dll \
                       $$[QT_INSTALL_BINS]\\Qt*Gui.dll \
                       $$[QT_INSTALL_BINS]\\Qt*Network.dll \
                       $$[QT_INSTALL_BINS]\\Qt*Pdf.dll \
                       $$[QT_INSTALL_BINS]\\Qt*PdfWidgets.dll \
                       $$[QT_INSTALL_BINS]\\Qt*PrintSupport.dll \
                       $$[QT_INSTALL_BINS]\\Qt*SerialPort.dll \
                       $$[QT_INSTALL_BINS]\\Qt*Sql.dll \
                       $$[QT_INSTALL_BINS]\\Qt*Widgets.dll \
                       $$[QT_INSTALL_BINS]\\libgcc_s_seh-*.dll \
                       $$[QT_INSTALL_BINS]\\libstdc++-*.dll \
                       $$[QT_INSTALL_BINS]\\libwinpthread-*.dll
qtlibraries.path     = release\\.
shell.files          = Distributions\\glitch-darkmode.bat
shell.path           = release\\.

INSTALLS = documentation1 \
           documentation2 \
           documentation3 \
           examples \
           plugins \
           qt \
           qtlibraries \
           shell
}
