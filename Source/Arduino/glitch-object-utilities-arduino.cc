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

#include "glitch-object-utilities-arduino.h"

glitch_object_utilities_arduino::glitch_object_utilities_arduino
(const QString &utilitiesType, QWidget *parent):
  glitch_object_utilities_arduino(1, parent)
{
  m_utilitiesType = stringToUtilitiesType(utilitiesType);

  switch(m_utilitiesType)
    {
    default:
      {
	m_ui.label->setText("sizeof()");
	break;
      }
    }

  m_properties[Properties::UTILITIES_TYPE] = m_ui.label->text();
  setName(m_ui.label->text());
}

glitch_object_utilities_arduino::glitch_object_utilities_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-utilities";
  m_ui.setupUi(this);
  prepareContextMenu();
  setName(m_ui.label->text());
}

glitch_object_utilities_arduino::~glitch_object_utilities_arduino()
{
}

QString glitch_object_utilities_arduino::code(void) const
{
  switch(m_utilitiesType)
    {
    default:
      {
	return QString("sizeof(%1);").arg(inputs().value(0));
      }
    }
}

bool glitch_object_utilities_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_utilities_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_utilities_arduino::isFullyWired(void) const
{
  switch(m_utilitiesType)
    {
    default:
      {
	return inputs().size() >= 1;
      }
    }
}

bool glitch_object_utilities_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_utilities_arduino *glitch_object_utilities_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_utilities_arduino
    (utilitiesTypeToString(), parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_utilitiesType = m_utilitiesType;
  clone->m_ui.label->setText(m_ui.label->text());
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_utilities_arduino *glitch_object_utilities_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_utilities_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ui.label->setText
    (object->m_properties.value(Properties::UTILITIES_TYPE).toString());
  object->m_utilitiesType = stringToUtilitiesType
    (object->m_properties.value(Properties::UTILITIES_TYPE).toString());
  return object;
}

void glitch_object_utilities_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_utilities_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["utilities_type"] = m_ui.label->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_utilities_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::UTILITIES_TYPE] = "sizeof()";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("utilities_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("sizeof"))
	    string = "sizeof()";
	  else
	    string = "sizeof()";

	  m_properties[Properties::UTILITIES_TYPE] = string.trimmed();
	}
    }

  setName(m_properties.value(Properties::UTILITIES_TYPE).toString());
}
