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

#include "glitch-object-logical-operator-arduino.h"

glitch_object_logical_operator_arduino::glitch_object_logical_operator_arduino
(QWidget *parent):glitch_object(parent)
{
  m_operator = AND_OPERATOR;
  m_type = "arduino-logicaloperator";
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
}

glitch_object_logical_operator_arduino::glitch_object_logical_operator_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_type = "arduino-logicaloperator";
}

glitch_object_logical_operator_arduino::
~glitch_object_logical_operator_arduino()
{
}

glitch_object_logical_operator_arduino *glitch_object_logical_operator_arduino::
clone(QWidget *parent) const
{
  auto *clone = new glitch_object_logical_operator_arduino(parent);

  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_logical_operator_arduino *glitch_object_logical_operator_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto *object = new glitch_object_logical_operator_arduino
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

bool glitch_object_logical_operator_arduino::hasView(void) const
{
  return false;
}

bool glitch_object_logical_operator_arduino::isMandatory(void) const
{
  return false;
}

void glitch_object_logical_operator_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}
