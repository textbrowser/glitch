/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from Glitch without specific prior written permission.
**
** GLITCH IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLITCH, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QDir>
#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
#include <QJniObject>
#endif
#endif
#include <QSettings>

#include <iostream>

#ifdef Q_OS_MACOS
#include "CocoaInitializer.h"
#endif

#include "glitch-application.h"
#include "glitch-ui.h"
#include "glitch-variety.h"
#include "glitch-version.h"

int main(int argc, char *argv[])
{
  for(int i = 0; i < argc; i++)
    if(!argv || !argv[i])
      continue;
    else if(strcmp(argv[i], "--help") == 0)
      {
	std::cout << "Glitch [options]" << std::endl;
	std::cout << " --full-screen" << std::endl;
	std::cout << " --generate-source" << std::endl;
	std::cout << " --help" << std::endl;
	std::cout << " --new-arduino-diagram diagram-name" << std::endl;
	std::cout << " --open-arduino-diagrams file-name(s)" << std::endl;
	std::cout << " --show-serial-port-window" << std::endl;
	std::cout << " --show-tools" << std::endl;
	std::cout << " --version" << std::endl;
	return static_cast<int> (EXIT_SUCCESS);
      }
    else if(strcmp(argv[i], "--version") == 0)
      {
	std::cout << "Glitch version "
		  << GLITCH_VERSION_STRING
		  << "."
		  << std::endl;
	return static_cast<int> (EXIT_SUCCESS);
      }

#if defined(Q_OS_ANDROID) || defined(Q_OS_MACOS) || defined(Q_OS_WINDOWS)
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
#endif
#if defined(Q_OS_ANDROID) || defined(Q_OS_MACOS)
#else
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
#endif

  QDir dir;
  glitch_application qapplication(argc, argv);

  dir.mkdir(glitch_variety::homePath());

#ifdef Q_OS_MACOS
  /*
  ** Eliminate pool errors on MacOS.
  */

  CocoaInitializer ci;
#endif
  QCoreApplication::setApplicationName("Glitch");
  QCoreApplication::setApplicationVersion(GLITCH_VERSION_STRING);
  QCoreApplication::setOrganizationName("Glitch");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat,
		     QSettings::UserScope,
                     glitch_variety::homePath());

  int rc = 0;

  {
    glitch_ui ui;

#ifndef Q_OS_ANDROID
    glitch_ui::s_mainWindow = &ui;
#endif
    ui.show();
    rc = static_cast<int> (qapplication.exec());
  }

#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
  auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());

  activity.callMethod<void> ("finishAndRemoveTask");
#endif
#endif
  return rc;
}
