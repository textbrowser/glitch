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

#include "glowbot-object-analog-read-arduino.h"
#include "glowbot-object-view.h"

glowbot_object_analog_read_arduino::glowbot_object_analog_read_arduino
(QWidget *parent):glowbot_object(parent)
{
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_type = "arduino-analogread";
}

glowbot_object_analog_read_arduino::glowbot_object_analog_read_arduino
(const quint64 id, QWidget *parent):glowbot_object(id, parent)
{
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_type = "arduino-analogread";
}

glowbot_object_analog_read_arduino::~glowbot_object_analog_read_arduino()
{
}

glowbot_object_analog_read_arduino *glowbot_object_analog_read_arduino::
clone(void) const
{
  glowbot_object_analog_read_arduino *object = new
    glowbot_object_analog_read_arduino(0);

  object->m_type = m_type;
  object->m_ui.setupUi(object);
  object->m_ui.label->setAttribute
    (Qt::WA_TransparentForMouseEvents,
     m_ui.label->testAttribute(Qt::WA_TransparentForMouseEvents));
  object->m_ui.label->setAutoFillBackground(m_ui.label->autoFillBackground());
  return object;
}

glowbot_object_analog_read_arduino *glowbot_object_analog_read_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  glowbot_object_analog_read_arduino *object = new
    glowbot_object_analog_read_arduino
    (values.value("myoid").toULongLong(), parent);

  return object;
}

bool glowbot_object_analog_read_arduino::hasView(void) const
{
  return false;
}

bool glowbot_object_analog_read_arduino::isMandatory(void) const
{
  return false;
}

void glowbot_object_analog_read_arduino::addActions(QMenu &menu) const
{
  addDefaultActions(menu);
}
