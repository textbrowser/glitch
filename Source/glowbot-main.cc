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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QApplication>
#include <QSettings>

#include <iostream>

#include "glowbot-common.h"
#include "glowbot-misc.h"
#include "glowbot-ui.h"

int main(int argc, char *argv[])
{
  for(int i = 0; i < argc; i++)
    if(argv && argv[i] && strcmp(argv[i], "--version") == 0)
      {
	std::cout << "GlowBot version " << GLOWBOT_VERSION_STR << "."
		  << std::endl;
	return EXIT_SUCCESS;
      }

  QApplication qapplication(argc, argv);

  qapplication.setWindowIcon(QIcon(":Logo/glowbot-logo.png"));

#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050000
  /*
  ** Eliminate pool errors on OS X.
  */

  CocoaInitializer ci;
#endif
#endif
  QCoreApplication::setApplicationName("GlowBot");
#if QT_VERSION >= 0x050700
  QApplication::setAttribute(Qt::AA_DontUseNativeDialogs, true);
#endif
  QCoreApplication::setOrganizationName("GlowBot");
  QCoreApplication::setOrganizationDomain("glowbot.sf.net");
  QCoreApplication::setApplicationVersion(GLOWBOT_VERSION_STR);
  QSettings::setPath(QSettings::IniFormat,
		     QSettings::UserScope,
                     glowbot_misc::homePath());
  QSettings::setDefaultFormat(QSettings::IniFormat);

  glowbot_ui ui;

  ui.show();
  return qapplication.exec();
}
