/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include "glitch-object-constant-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-undo-command.h"

glitch_object_constant_arduino::glitch_object_constant_arduino
(QWidget *parent):glitch_object_constant_arduino(1, parent)
{
}

glitch_object_constant_arduino::glitch_object_constant_arduino
(const QString &constantType,
 QWidget *parent):glitch_object_constant_arduino(1, parent)
{
  setConstantType(constantType);
  m_properties[Properties::CONSTANT_OTHER] = "";
  m_properties[Properties::CONSTANT_TYPE] = m_ui.constant->currentText();
}

glitch_object_constant_arduino::glitch_object_constant_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-constant";
  m_ui.setupUi(this);
  m_ui.constant->installEventFilter(new glitch_scroll_filter(this));
  m_ui.constant->setCurrentIndex(m_ui.constant->findText(tr("Other")));
  m_ui.other->setVisible(true);
  connect(m_ui.constant,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotConstantChanged(void)));
  connect(m_ui.other,
	  &QLineEdit::editingFinished,
	  this,
	  &glitch_object_constant_arduino::slotOtherConstantChanged);
  m_properties[Properties::CONSTANT_OTHER] = "";
  m_properties[Properties::CONSTANT_TYPE] = m_ui.constant->currentText();
  prepareContextMenu();
  glitch_object_constant_arduino::setName(m_type);
}

glitch_object_constant_arduino::~glitch_object_constant_arduino()
{
}

QSize glitch_object_constant_arduino::preferredSize(void) const
{
  if(m_ui.constant->currentText() == tr("Other"))
    {
      QFontMetrics const fontMetrics(font());
      auto const width = 5 * qCeil
	(fontMetrics.boundingRect(m_ui.other->text().trimmed()).width() / 5.0) +
	(compressed() == false ?
	 5 * qCeil(fontMetrics.boundingRect(m_ui.constant->currentText().
					    trimmed()).width() / 5.0) : 0) +
	s_widthTweak;

      return QSize
	(qMax(5 * qCeil(minimumSizeHint().width() / 5.0), width),
	 minimumHeight(sizeHint().height()));
    }
  else
    return QSize(5 * qCeil(sizeHint().width() / 5.0),
		 minimumHeight(sizeHint().height()));
}

QString glitch_object_constant_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  if(m_ui.constant->currentText() == tr("Other"))
    return m_ui.other->text().trimmed();
  else
    return m_ui.constant->currentText();
}

QString glitch_object_constant_arduino::constantType(void) const
{
  return m_ui.constant->currentText();
}

bool glitch_object_constant_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_constant_arduino::isFullyWired(void) const
{
  return true;
}

bool glitch_object_constant_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_constant_arduino *glitch_object_constant_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_constant_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_ui.constant->blockSignals(true);
  clone->m_ui.constant->setCurrentIndex(m_ui.constant->currentIndex());
  clone->m_ui.constant->blockSignals(false);
  clone->m_ui.other->setText(m_ui.other->text().trimmed());
  clone->m_ui.other->setCursorPosition(0);
  clone->setCanvasSettings(m_canvasSettings);
  clone->setConstantType(m_ui.constant->currentText());
  clone->setName(m_ui.other->text().trimmed());
  clone->setStyleSheet(styleSheet());
  clone->compressWidget
    (m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
  clone->resize(size());
  return clone;
}

glitch_object_constant_arduino *glitch_object_constant_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_constant_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (splitPropertiesRegularExpression(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_constant_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_constant_arduino::compressWidget(const bool state)
{
  glitch_object::compressWidget(state);

  if(m_ui.constant->currentText() == tr("Other"))
    m_ui.constant->setVisible(!state);

  adjustSize();
  resize(preferredSize());
}

void glitch_object_constant_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["constant"] = m_ui.constant->currentText();
  properties["constant_other"] = m_ui.other->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_constant_arduino::setConstantType
(const QString &constantType)
{
  m_ui.other->setVisible(false);

  auto c(constantType.toLower().trimmed());

  if(c.endsWith("false"))
    c = "false";
  else if(c.endsWith("high"))
    c = "HIGH";
  else if(c.endsWith("input"))
    c = "INPUT";
  else if(c.endsWith("input_pullup"))
    c = "INPUT_PULLUP";
  else if(c.endsWith("led_builtin"))
    c = "LED_BUILTIN";
  else if(c.endsWith("low"))
    c = "LOW";
  else if(c.endsWith("output"))
    c = "OUTPUT";
  else if(c.endsWith("true"))
    c = "true";
  else
    {
      c = "Other";

      if(!constantType.toLower().trimmed().endsWith("other"))
	m_ui.other->setText
	  (QString(constantType).remove("glitch-arduino-constants-").
	   toUpper());

      m_ui.other->setVisible(true);
    }

  m_ui.constant->blockSignals(true);
  m_ui.constant->setCurrentIndex(m_ui.constant->findText(c));

  if(m_ui.constant->currentIndex() < 0)
    m_ui.constant->setCurrentIndex(m_ui.constant->findText(tr("Other")));

  m_ui.constant->blockSignals(false);
  setName(m_ui.other->text());
}

void glitch_object_constant_arduino::setName(const QString &n)
{
  if(m_ui.constant->currentText() == tr("Other"))
    {
      auto name(n.trimmed());

      if(name.isEmpty())
	name = m_type;

      glitch_object::setName(name);
    }
  else
    glitch_object::setName(m_ui.constant->currentText());
}

void glitch_object_constant_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::CONSTANT_OTHER] = "";
  m_properties[Properties::CONSTANT_TYPE] = "other";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("constant = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::CONSTANT_TYPE] = string.trimmed();
	}
      else if(string.simplified().startsWith("constant_other = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string = string.mid(string.indexOf('"') + 1);
	  string = string.mid(0, string.lastIndexOf('"'));
	  m_properties[Properties::CONSTANT_OTHER] = string.trimmed();
	  m_ui.other->setText(string.trimmed());
	  m_ui.other->setCursorPosition(0);
	}
    }

  setConstantType(m_properties.value(Properties::CONSTANT_TYPE).toString());
  setName(m_ui.other->text());
  compressWidget(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
}

void glitch_object_constant_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::CONSTANT_OTHER:
      {
	m_ui.other->setText(value.toString().trimmed());
	m_ui.other->setCursorPosition(0);
	setName(m_ui.other->text());
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_constant_arduino::slotAdjustSize(void)
{
  auto const before(size());

  resize(preferredSize());

  if(before == this->size())
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(size(),
	 before,
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 Properties::SIZE,
	 this);

      undoCommand->setText
	(tr("object size changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }

  emit changed();
}

void glitch_object_constant_arduino::slotConstantChanged(void)
{
  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(m_properties.value(Properties::CONSTANT_TYPE).toString(),
	 glitch_undo_command::Types::CONSTANT_TYPE_CHANGED,
	 this);

      m_properties[Properties::CONSTANT_TYPE] = m_ui.constant->currentText();
      undoCommand->setText
	(tr("constant type changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[Properties::CONSTANT_TYPE] = m_ui.constant->currentText();

  emit changed();
}

void glitch_object_constant_arduino::slotOtherConstantChanged(void)
{
  m_ui.other->setText(m_ui.other->text().trimmed());
  m_ui.other->setCursorPosition(0);

  auto const property = glitch_object::Properties::CONSTANT_OTHER;

  if(m_properties.value(property).toString() == m_ui.other->text())
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(m_ui.other->text(),
	 m_properties.value(property),
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 property,
	 this);

      m_properties[property] = m_ui.other->text();
      undoCommand->setText
	(tr("constant property changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[property] = m_ui.other->text();

  m_ui.other->selectAll();
  emit changed();
}
