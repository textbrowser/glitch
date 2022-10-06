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

#include "glitch-object-random-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-undo-command.h"

glitch_object_random_arduino::glitch_object_random_arduino
(const QString &randomType, QWidget *parent):
  glitch_object_random_arduino(1, parent)
{
  m_randomType = stringToRandomType(randomType);
  m_ui.function->blockSignals(true);

  switch(m_randomType)
    {
    case Type::RANDOM_SEED:
      {
	m_ui.function->setCurrentIndex(m_ui.function->findText("randomSeed()"));
	break;
      }
    default:
      {
	m_ui.function->setCurrentIndex(m_ui.function->findText("random()"));
	break;
      }
    }

  m_ui.function->blockSignals(false);
  m_properties[Properties::RANDOM_TYPE] = m_ui.function->currentText();
  setName(m_ui.function->currentText());
}

glitch_object_random_arduino::glitch_object_random_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-random";
  m_ui.setupUi(this);
  m_ui.function->addItems(QStringList() << "random()"
			                << "randomSeed()");
  m_ui.function->installEventFilter(new glitch_scroll_filter(this));
  connect(m_ui.function,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotFunctionChanged(void)));
  prepareContextMenu();
  setName(m_ui.function->currentText());
}

glitch_object_random_arduino::~glitch_object_random_arduino()
{
}

QString glitch_object_random_arduino::code(void) const
{
  switch(m_randomType)
    {
    case Type::RANDOM:
      {
	return QString("randomSeed(%1);").arg(inputs().value(0));
      }
    default:
      {
	if(inputs().size() == 1)
	  return QString("random(%1);").arg(inputs().value(0));
	else
	  return QString("random(%1, %2);").
	    arg(inputs().value(0)).
	    arg(inputs().value(1));
      }
    }
}

bool glitch_object_random_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_random_arduino::hasOutput(void) const
{
  if(m_randomType == Type::RANDOM)
    return true;
  else
    return false;
}

bool glitch_object_random_arduino::isFullyWired(void) const
{
  switch(m_randomType)
    {
    case Type::RANDOM:
      {
	return inputs().size() >= 1;
      }
    default:
      {
	return inputs().size() >= 2;
      }
    }
}

bool glitch_object_random_arduino::shouldPrint(void) const
{
  if(m_randomType == Type::RANDOM_SEED)
    return true;
  else
    return false;
}

glitch_object_random_arduino *glitch_object_random_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_random_arduino(randomTypeToString(), parent);

  clone->cloneWires(m_wires);
  clone->m_properties = m_properties;
  clone->m_randomType = m_randomType;
  clone->m_ui.function->blockSignals(true);
  clone->m_ui.function->setCurrentIndex(m_ui.function->currentIndex());
  clone->m_ui.function->blockSignals(false);
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_random_arduino *glitch_object_random_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_random_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_randomType = stringToRandomType
    (object->m_properties.value(Properties::RANDOM_TYPE).toString());
  return object;
}

void glitch_object_random_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_random_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["random_type"] = m_ui.function->currentText();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_random_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::RANDOM_TYPE] = "random()";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("random_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");

	  if(string.contains("randomseed"))
	    string = "randomSeed()";
	  else
	    string = "random()";

	  m_properties[Properties::RANDOM_TYPE] = string.trimmed();
	}
    }

  m_randomType = stringToRandomType
    (m_properties.value(Properties::RANDOM_TYPE).toString());
  m_ui.function->blockSignals(true);
  m_ui.function->setCurrentIndex
    (m_ui.function->
     findText(m_properties.value(Properties::RANDOM_TYPE).toString()));
  m_ui.function->blockSignals(false);
  setName(m_properties.value(Properties::RANDOM_TYPE).toString());
}

void glitch_object_random_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::RANDOM_TYPE:
      {
	m_randomType = stringToRandomType(value.toString());
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

void glitch_object_random_arduino::slotFunctionChanged(void)
{
  m_randomType = stringToRandomType(m_ui.function->currentText());

  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (m_ui.function->currentText(),
     m_properties.value(Properties::RANDOM_TYPE).toString(),
     glitch_undo_command::PROPERTY_CHANGED,
     Properties::RANDOM_TYPE,
     this);

  m_properties[Properties::RANDOM_TYPE] = m_ui.function->currentText();
  undoCommand->setText
    (tr("random function changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}
