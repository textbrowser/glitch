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

#ifndef _glitch_object_function_parameters_arduino_h_
#define _glitch_object_function_parameters_arduino_h_

#include <QStyledItemDelegate>

#include "ui_glitch-object-function-parameters-arduino.h"

class glitch_object_function_parameters_arduino_item_delegate:
  public QStyledItemDelegate
{
  Q_OBJECT

 public:
  glitch_object_function_parameters_arduino_item_delegate
    (QObject *parent):QStyledItemDelegate(parent)
  {
  }

  QSize sizeHint
    (const QStyleOptionViewItem &option, const QModelIndex &index) const;
  QWidget *createEditor(QWidget *parent,
			const QStyleOptionViewItem &option,
			const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor,
                    QAbstractItemModel *model,
                    const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor,
			    const QStyleOptionViewItem &option,
			    const QModelIndex &index) const;
};

class glitch_object_function_parameters_arduino: public QDialog
{
  Q_OBJECT

 public:
  enum ColumnIndices
  {
    PARAMETER_COLUMN = 0,
    TYPE_COLUMN
  };

  glitch_object_function_parameters_arduino
    (const QMap<QString, QString> &parameters, QWidget *parent);
  ~glitch_object_function_parameters_arduino();

 private:
  Ui_glitch_object_function_parameters_arduino m_ui;

 private slots:
  void slotAdd(void);
  void slotApply(void);
  void slotDelete(void);
};

#endif
