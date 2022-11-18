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

#include "glitch-object-conversion-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-undo-command.h"

glitch_object_conversion_arduino::glitch_object_conversion_arduino
(QWidget *parent):glitch_object_conversion_arduino(1, parent)
{
}

glitch_object_conversion_arduino::
glitch_object_conversion_arduino
(const QString &conversionType, QWidget *parent):
  glitch_object_conversion_arduino(1, parent)
{
  setConversionType(conversionType);
  m_properties[Properties::CONVERSION] = m_ui.conversion->currentText();
}

glitch_object_conversion_arduino::glitch_object_conversion_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_conversionType = ConversionTypes::UNSIGNED_INT;
  m_type = "arduino-conversion";
  m_ui.setupUi(this);
  m_ui.conversion->installEventFilter(new glitch_scroll_filter(this));
  connect(m_ui.conversion,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotConversionChanged(void)));
  prepareContextMenu();
  setConversionType(m_conversionType);
  m_properties[Properties::CONVERSION] = m_ui.conversion->currentText();
}

glitch_object_conversion_arduino::~glitch_object_conversion_arduino()
{
}

QString glitch_object_conversion_arduino::code(void) const
{
  switch(m_conversionType)
    {
    case ConversionTypes::UNSIGNED_INT:
    case ConversionTypes::UNSIGNED_LONG:
      {
	return QString("%1 (%2)").
	  arg(m_ui.conversion->currentText()).arg(inputs().value(0));
      }
    default:
      {
	return QString("%1(%2)").
	  arg(m_ui.conversion->currentText().remove("()")).
	  arg(inputs().value(0));
      }
    }
}

bool glitch_object_conversion_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_conversion_arduino::hasOutput(void) const
{
  switch(m_conversionType)
    {
    default:
      {
	return true;
      }
    }
}

bool glitch_object_conversion_arduino::isFullyWired(void) const
{
  switch(m_conversionType)
    {
    default:
      {
	return inputs().size() >= 1;
      }
    }
}

bool glitch_object_conversion_arduino::shouldPrint(void) const
{
  switch(m_conversionType)
    {
    default:
      {
	return false;
      }
    }
}

glitch_object_conversion_arduino *glitch_object_conversion_arduino::clone
(QWidget *parent) const
{
  auto clone = new glitch_object_conversion_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setConversionType(m_conversionType);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_conversion_arduino *glitch_object_conversion_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_conversion_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (values.value("properties").toString().split(s_splitRegularExpression));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_conversion_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_conversion_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["conversion"] = m_ui.conversion->currentText();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_conversion_arduino::setConversionType
(const ConversionTypes conversionType)
{
  m_conversionType = conversionType;
  m_ui.conversion->blockSignals(true);

  switch(m_conversionType)
    {
    case ConversionTypes::BYTE:
      {
	m_ui.conversion->setCurrentIndex(m_ui.conversion->findText("byte()"));
	break;
      }
    case ConversionTypes::CHAR:
      {
	m_ui.conversion->setCurrentIndex(m_ui.conversion->findText("char()"));
	break;
      }
    case ConversionTypes::FLOAT:
      {
	m_ui.conversion->setCurrentIndex(m_ui.conversion->findText("float()"));
	break;
      }
    case ConversionTypes::INT:
      {
	m_ui.conversion->setCurrentIndex(m_ui.conversion->findText("int()"));
	break;
      }
    case ConversionTypes::LONG:
      {
	m_ui.conversion->setCurrentIndex(m_ui.conversion->findText("long()"));
	break;
      }
    case ConversionTypes::UNSIGNED_LONG:
      {
	m_ui.conversion->setCurrentIndex
	  (m_ui.conversion->findText("(unsigned long)"));
	break;
      }
    case ConversionTypes::WORD:
      {
	m_ui.conversion->setCurrentIndex(m_ui.conversion->findText("word()"));
	break;
      }
    default:
      {
	m_ui.conversion->setCurrentIndex
	  (m_ui.conversion->findText("(unsigned int)"));
	break;
      }
    }

  m_ui.conversion->blockSignals(false);
  setName(m_ui.conversion->currentText());
}

void glitch_object_conversion_arduino::setConversionType(const QString &ct)
{
  auto conversionType(ct.toLower());

  if(conversionType.contains("(unsigned int)"))
    setConversionType(ConversionTypes::UNSIGNED_INT);
  else if(conversionType.contains("(unsigned long)"))
    setConversionType(ConversionTypes::UNSIGNED_LONG);
  else if(conversionType.contains("byte"))
    setConversionType(ConversionTypes::BYTE);
  else if(conversionType.contains("char"))
    setConversionType(ConversionTypes::CHAR);
  else if(conversionType.contains("float"))
    setConversionType(ConversionTypes::FLOAT);
  else if(conversionType.contains("int"))
    setConversionType(ConversionTypes::INT);
  else if(conversionType.contains("long"))
    setConversionType(ConversionTypes::LONG);
  else if(conversionType.contains("word"))
    setConversionType(ConversionTypes::WORD);
  else
    setConversionType(ConversionTypes::UNSIGNED_INT);
}

void glitch_object_conversion_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::CONVERSION] = "(unsigned int)";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("conversion = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::CONVERSION] = string.trimmed();
	}
    }

  setConversionType(m_properties.value(Properties::CONVERSION).toString());
}

void glitch_object_conversion_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::CONVERSION:
      {
	m_ui.conversion->blockSignals(true);
	m_ui.conversion->setCurrentIndex
	  (m_ui.conversion->findText(value.toString()));
	m_ui.conversion->blockSignals(false);
	setConversionType(value.toString());
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_conversion_arduino::slotConversionChanged(void)
{
  setConversionType(m_ui.conversion->currentText());

  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (m_ui.conversion->currentText(),
     m_properties.value(Properties::CONVERSION).toString(),
     glitch_undo_command::PROPERTY_CHANGED,
     Properties::CONVERSION,
     this);

  m_properties[Properties::CONVERSION] = m_ui.conversion->currentText();
  undoCommand->setText
    (tr("conversion changed (%1, %2)").arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}
