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

#include "glitch-object-function-parameters-arduino.h"
#include <QtDebug>
glitch_object_function_parameters_arduino::
glitch_object_function_parameters_arduino
(const QMap<QString, QString> &parameters, QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.parameters->setRowCount(parameters.size());

  QMapIterator<QString, QString> it(parameters);
  int i = 0;

  while(it.hasNext())
    {
      it.next();

      auto item = new QTableWidgetItem(it.key());

      m_ui.parameters->setItem(i, ColumnIndices::PARAMETER_COLUMN, item);
      item = new QTableWidgetItem(it.value());
      m_ui.parameters->setItem(i, ColumnIndices::TYPE_COLUMN, item);
      i += 1;
    }
}

glitch_object_function_parameters_arduino::
~glitch_object_function_parameters_arduino()
{
}
