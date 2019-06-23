/*
** Copyright (c) 2004 - 10^10^10, Alexis Megas.
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

#include <QApplication>
#include <QDir>
#include <QSettings>

#include <iostream>

#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050000
#include "CocoaInitializer.h"
#endif
#endif

#include "glitch-common.h"
#include "glitch-misc.h"
#include "glitch-ui.h"

int main(int argc, char *argv[])
{
  for(int i = 0; i < argc; i++)
    if(argv && argv[i] && strcmp(argv[i], "--help") == 0)
      {
	std::cout << "Glitch [options]" << std::endl;
	std::cout << "--help" << std::endl;
	std::cout << "--new-arduino-diagram name" << std::endl;
	std::cout << "--open-arduino-diagram name" << std::endl;
	std::cout << "--version" << std::endl;
	return EXIT_SUCCESS;
      }
    else if(argv && argv[i] && strcmp(argv[i], "--version") == 0)
      {
	std::cout << "Glitch version " << GLITCH_VERSION_STR << "."
		  << std::endl;
	return EXIT_SUCCESS;
      }

  QApplication qapplication(argc, argv);

  qapplication.setWindowIcon(QIcon(":Logo/glitch-logo.png"));

  QDir dir;

  dir.mkdir(glitch_misc::homePath());

#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050000
  /*
  ** Eliminate pool errors on OS X.
  */

  CocoaInitializer ci;
#endif
#endif
  QCoreApplication::setApplicationName("Glitch");
#if QT_VERSION >= 0x050700
  QApplication::setAttribute(Qt::AA_DontUseNativeDialogs, true);
#endif
  QCoreApplication::setOrganizationName("Glitch");
  QCoreApplication::setOrganizationDomain("glitch.sf.net");
  QCoreApplication::setApplicationVersion(GLITCH_VERSION_STR);
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat,
		     QSettings::UserScope,
                     glitch_misc::homePath());

  glitch_ui ui;

  ui.show();
  return qapplication.exec();
}
