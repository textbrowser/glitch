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

#ifndef _glitch_view_arduino_h_
#define _glitch_view_arduino_h_

#include "glitch-view.h"

class glitch_alignment;
class glitch_object_loop_arduino;
class glitch_object_setup_arduino;
class glitch_structures_arduino;

class glitch_view_arduino: public glitch_view
{
  Q_OBJECT

 public:
  glitch_view_arduino(const QString &fileName,
		      const QString &name,
		      const bool fromFile,
		      const glitch_common::ProjectTypes projectType,
		      QWidget *parent);
  ~glitch_view_arduino();
  QString nextUniqueFunctionName(void) const;
  QString projectOutputFileExtension(void) const;
  bool containsFunctionName(const QString &name) const;
  bool open(const QString &fileName, QString &error);
  glitch_object_loop_arduino *loopObject(void) const;
  glitch_object_setup_arduino *setupObject(void) const;
  void consumeFunctionName(const QString &name);
  void generateSource(void);
  void removeFunctionName(const QString &name);
  void separate(void);
  void showStructures(void);

 private:
  QMap<QString, char> m_functionNames;
  QPointer<glitch_structures_arduino> m_arduinoStructures;
  glitch_object_loop_arduino *m_loopObject;
  glitch_object_setup_arduino *m_setupObject;

 private slots:
  void slotFunctionAdded(const QString &name, const bool isClone);
  void slotFunctionDeleted(const QString &name);
  void slotShowStructures(void);
};

#endif
