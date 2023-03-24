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

#ifndef _glitch_structures_arduino_h_
#define _glitch_structures_arduino_h_

#include <QDialog>
#include <QTimer>

#include "ui_glitch-structures.h"

class glitch_collapse_expand_tool_button;

class glitch_structures_arduino: public QDialog
{
  Q_OBJECT

 public:
  glitch_structures_arduino(QWidget *parent);
  ~glitch_structures_arduino();
  QFrame *frame(void) const;
  static QStringList keywords(void);
  static QStringList nonArrayVariableTypes(void);
  static QStringList variableTypes(void);
  static bool containsStructure(const QString &structureName);
  static bool isReserved(const QString &t);
  void setIconSize(const QString &t);

 private:
  QMap<QString, QString> m_categoriesMap;
  QTimer m_filterTimer;
  Ui_glitch_structures m_ui;
  glitch_collapse_expand_tool_button *m_collapse;
  static QMap<QString, QString> s_itemsForIcons;
  static QMap<QString, QStringList> s_itemsForCategories;
  static QMap<QString, char> s_structureNamesMap;
  static QStringList s_nonArrayVariableTypes;
  static QStringList s_variableTypes;
  static QStringList structureNames(void);
  void prepareCategories(void);

 private slots:
  void slotFilter(const QString &text);
  void slotFilter(void);
};

#endif
