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

#include <QDir>
#include <QIcon>
#include <QMessageBox>

#include "glowbot-misc.h"

QPointF glowbot_misc::dbPointToPointF(const QString &text)
{
  QStringList list(QString(text).remove("(").remove(")").split(","));

  return QPointF(qAbs(list.value(0).toDouble()),
		 qAbs(list.value(1).toDouble()));
}

QString glowbot_misc::homePath(void)
{
  QByteArray homepath(qgetenv("GLOWBOT_HOME"));

  if(homepath.isEmpty())
#ifdef Q_OS_WIN32
    return QDir::currentPath() + QDir::separator() + ".glowbot";
#else
    return QDir::homePath() + QDir::separator() + ".glowbot";
#endif
  else
    return homepath.constData();
}

void glowbot_misc::showErrorDialog(const QString &text, QWidget *parent)
{
  QMessageBox mb(parent);

  mb.setIcon(QMessageBox::Critical);
  mb.setText(text);
  mb.setWindowIcon(QIcon(":Logo/glowbot-logo.png"));
  mb.setWindowTitle(QObject::tr("GlowBot: Error"));
  mb.exec();
}
