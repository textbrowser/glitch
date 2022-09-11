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

#include "glitch-object-syntax-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-undo-command.h"

glitch_object_syntax_arduino::glitch_object_syntax_arduino
(QWidget *parent):glitch_object_syntax_arduino(1, parent)
{
}

glitch_object_syntax_arduino::glitch_object_syntax_arduino
(const QString &syntax,
 QWidget *parent):glitch_object_syntax_arduino(1, parent)
{
  m_ui.define_include->setText(syntax.trimmed());
}

glitch_object_syntax_arduino::glitch_object_syntax_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-syntax";
  m_ui.setupUi(this);
  connect(m_ui.define_include,
	  &QLineEdit::returnPressed,
	  this,
	  &glitch_object_syntax_arduino::slotSyntaxChanged);
  prepareContextMenu();
  setName(m_type);
}

glitch_object_syntax_arduino::~glitch_object_syntax_arduino()
{
}

QString glitch_object_syntax_arduino::code(void) const
{
  return m_ui.define_include->text().trimmed();
}

bool glitch_object_syntax_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_syntax_arduino::isFullyWired(void) const
{
  return true;
}

bool glitch_object_syntax_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_syntax_arduino *glitch_object_syntax_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_syntax_arduino(parent);

  clone->cloneWires(m_wires);
  clone->m_properties = m_properties;
  clone->m_ui.define_include->setText(m_ui.define_include->text().trimmed());
  clone->m_ui.define_include->selectAll();
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_syntax_arduino *glitch_object_syntax_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_syntax_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (values.value("properties").toString().split(s_splitRegularExpression));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_syntax_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_syntax_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["syntax"] = m_ui.define_include->text().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_syntax_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("syntax = "))
	{
	}
    }
}

void glitch_object_syntax_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::SYNTAX:
      {
	m_ui.define_include->setText(value.toString().trimmed());
	m_ui.define_include->selectAll();
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_syntax_arduino::slotSyntaxChanged(void)
{
  m_ui.define_include->setText(m_ui.define_include->text().trimmed());
  m_ui.define_include->selectAll();

  if(!m_undoStack)
    return;

  auto property = glitch_object::Properties::SYNTAX;

  if(m_properties.value(property).toString() == m_ui.define_include->text())
    return;

  auto undoCommand = new glitch_undo_command
    (m_ui.define_include->text(),
     m_properties.value(property),
     glitch_undo_command::PROPERTY_CHANGED,
     property,
     this);

  m_properties[property] = m_ui.define_include->text();
  undoCommand->setText
    (tr("syntax property changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}
