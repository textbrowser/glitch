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

#include "glitch-object-block-comment-arduino.h"
#include "glitch-undo-command.h"

glitch_object_block_comment_arduino::glitch_object_block_comment_arduino
(QWidget *parent):glitch_object_block_comment_arduino(1, parent)
{
  m_properties[Properties::COMMENT] = "";
}

glitch_object_block_comment_arduino::glitch_object_block_comment_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-blockcomment";
  m_ui.setupUi(this);
  m_ui.comment->setUndoRedoEnabled(false);
  connect(m_ui.comment,
	  &QPlainTextEdit::textChanged,
	  this,
	  &glitch_object_block_comment_arduino::slotTextChanged);
  prepareContextMenu();
  setName(m_type);
}

glitch_object_block_comment_arduino::~glitch_object_block_comment_arduino()
{
}

QPlainTextEdit *glitch_object_block_comment_arduino::plainTextEdit(void) const
{
  return m_ui.comment;
}

QString glitch_object_block_comment_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  QString code("");

  foreach(auto const &str, m_ui.comment->toPlainText().split('\n'))
    if(!str.trimmed().isEmpty())
      code.append(QString("// %1\n").arg(str.trimmed()));

  return code.trimmed();
}

bool glitch_object_block_comment_arduino::isFullyWired(void) const
{
  return true;
}

bool glitch_object_block_comment_arduino::shouldPrint(void) const
{
  return true;
}

glitch_object_block_comment_arduino *glitch_object_block_comment_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_block_comment_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_ui.comment->blockSignals(true);
  clone->m_ui.comment->setPlainText(m_ui.comment->toPlainText());
  clone->m_ui.comment->blockSignals(false);
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_block_comment_arduino *glitch_object_block_comment_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_block_comment_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(splitPropertiesAmpersand(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ui.comment->blockSignals(true);
  object->m_ui.comment->setPlainText
    (object->m_properties.value(Properties::COMMENT).toString().trimmed());
  object->m_ui.comment->blockSignals(false);
  return object;
}

void glitch_object_block_comment_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_block_comment_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["comment"] = m_ui.comment->toPlainText().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_block_comment_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMMENT] = "";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("comment = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMMENT] = string.trimmed();
	}
    }
}

void glitch_object_block_comment_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::COMMENT:
      {
	auto cursor(m_ui.comment->textCursor());
	auto const position = cursor.anchor();

	m_ui.comment->blockSignals(true);
	m_ui.comment->setPlainText(value.toString());

	if(position <= m_ui.comment->toPlainText().length())
	  cursor.setPosition(position);

	m_ui.comment->setTextCursor(cursor);
	m_ui.comment->blockSignals(false);
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_block_comment_arduino::slotTextChanged(void)
{
  auto const property = glitch_object::Properties::COMMENT;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(m_ui.comment->toPlainText(),
	 m_properties.value(property),
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 property,
	 this);

      m_properties[property] = m_ui.comment->toPlainText();
      undoCommand->setText
	(tr("comment changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[property] = m_ui.comment->toPlainText();

  emit changed();
}
