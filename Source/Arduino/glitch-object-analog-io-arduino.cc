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

#include "glitch-object-analog-io-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-undo-command.h"

glitch_object_analog_io_arduino::glitch_object_analog_io_arduino
(const QString &ioType, QWidget *parent):
  glitch_object_analog_io_arduino(1, parent)
{
  m_ioType = stringToIOType(ioType);
  m_ui.function->blockSignals(true);

  switch(m_ioType)
    {
    case Type::REFERENCE:
      {
	m_ui.function->setCurrentIndex
	  (m_ui.function->findText("analogReference()"));
	break;
      }
    case Type::WRITE:
      {
	m_ui.function->setCurrentIndex
	  (m_ui.function->findText("analogWrite()"));
	break;
      }
    default:
      {
	m_ui.function->setCurrentIndex(m_ui.function->findText("analogRead()"));
	break;
      }
    }

  m_ui.function->blockSignals(false);
  m_properties[Properties::ANALOG_IO_TYPE] = m_ui.function->currentText();
  setName(m_ui.function->currentText());
}

glitch_object_analog_io_arduino::glitch_object_analog_io_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-analogio";
  m_ui.setupUi(this);
  m_ui.function->addItems(QStringList() << "analogRead()"
			                << "analogReference()"
			                << "analogWrite()");
  m_ui.function->installEventFilter(new glitch_scroll_filter(this));
  connect(m_ui.function,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotFunctionChanged(void)));
  m_properties[Properties::ANALOG_IO_TYPE] = m_ui.function->currentText();
  prepareContextMenu();
  setName(m_ui.function->currentText());
}

glitch_object_analog_io_arduino::~glitch_object_analog_io_arduino()
{
}

QString glitch_object_analog_io_arduino::code(void) const
{
  switch(m_ioType)
    {
    case Type::REFERENCE:
      {
	return QString("analogReference(%1);").arg(inputs().value(0));
      }
    case Type::WRITE:
      {
	return QString("analogWrite(%1, %2);").
	  arg(inputs().value(0)).
	  arg(inputs().value(1));
      }
    default:
      {
	return QString("analogRead(%1);").arg(inputs().value(0));
      }
    }
}

bool glitch_object_analog_io_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_analog_io_arduino::hasOutput(void) const
{
  if(m_ioType == Type::READ)
    return true;
  else
    return false;
}

bool glitch_object_analog_io_arduino::isFullyWired(void) const
{
  switch(m_ioType)
    {
    case Type::REFERENCE:
      {
	return inputs().size() >= 1;
      }
    case Type::WRITE:
      {
	return inputs().size() >= 2;
      }
    default:
      {
	return inputs().size() >= 1;
      }
    }
}

bool glitch_object_analog_io_arduino::shouldPrint(void) const
{
  switch(m_ioType)
    {
    case Type::REFERENCE:
    case Type::WRITE:
      {
	return true;
      }
    default:
      {
	return false;
      }
    }
}

glitch_object_analog_io_arduino *glitch_object_analog_io_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_analog_io_arduino(ioTypeToString(), parent);

  clone->cloneWires(m_wires);
  clone->m_ioType = m_ioType;
  clone->m_properties = m_properties;
  clone->m_ui.function->blockSignals(true);
  clone->m_ui.function->setCurrentIndex(m_ui.function->currentIndex());
  clone->m_ui.function->blockSignals(false);
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_analog_io_arduino *glitch_object_analog_io_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_analog_io_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ioType = stringToIOType
    (object->m_properties.value(Properties::ANALOG_IO_TYPE).toString());
  return object;
}

void glitch_object_analog_io_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_analog_io_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["io_type"] = m_ui.function->currentText();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_analog_io_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::ANALOG_IO_TYPE] = "analogRead()";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("io_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("reference"))
	    string = "analogReference()";
	  else if(string.contains("write"))
	    string = "analogWrite()";
	  else
	    string = "analogRead()";

	  m_properties[Properties::ANALOG_IO_TYPE] = string.trimmed();
	}
    }

  m_ioType = stringToIOType
    (m_properties.value(Properties::ANALOG_IO_TYPE).toString());
  m_ui.function->blockSignals(true);
  m_ui.function->setCurrentIndex
    (m_ui.function->
     findText(m_properties.value(Properties::ANALOG_IO_TYPE).toString()));
  m_ui.function->blockSignals(false);
  setName(m_properties.value(Properties::ANALOG_IO_TYPE).toString());
}

void glitch_object_analog_io_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::ANALOG_IO_TYPE:
      {
	m_ioType = stringToIOType(value.toString());
	m_ui.function->blockSignals(true);
	m_ui.function->setCurrentIndex
	  (m_ui.function->findText(value.toString()));
	m_ui.function->blockSignals(false);
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_analog_io_arduino::slotFunctionChanged(void)
{
  m_ioType = stringToIOType(m_ui.function->currentText());

  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (m_ui.function->currentText(),
     m_properties.value(Properties::ANALOG_IO_TYPE).toString(),
     glitch_undo_command::PROPERTY_CHANGED,
     Properties::ANALOG_IO_TYPE,
     this);

  m_properties[Properties::ANALOG_IO_TYPE] =
    m_ui.function->currentText();
  undoCommand->setText
    (tr("analog i/o function changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}
