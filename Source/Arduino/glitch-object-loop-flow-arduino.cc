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

#include "glitch-object-edit-window.h"
#include "glitch-object-loop-flow-arduino.h"
#include "glitch-object-view.h"
#include "glitch-undo-command.h"

glitch_object_loop_flow_arduino::glitch_object_loop_flow_arduino
(QWidget *parent):glitch_object_loop_flow_arduino(1, parent)
{
}

glitch_object_loop_flow_arduino::glitch_object_loop_flow_arduino
(const QString &loopType, QWidget *parent):
  glitch_object_loop_flow_arduino(1, parent)
{
  setLoopType(loopType);
}

glitch_object_loop_flow_arduino::glitch_object_loop_flow_arduino
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_editView = new glitch_object_view
    (glitch_common::ProjectTypes::ArduinoProject,
     m_id,
     new QUndoStack(this), // New redo/undo stack.
     this);
  m_editWindow = new glitch_object_edit_window(parent);
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setEditView(m_editView);
  m_editWindow->setUndoStack(m_editView->undoStack());
  m_editWindow->setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  m_editWindow->setWindowTitle(tr("Glitch: loop"));
  m_editWindow->resize(600, 600);
  m_loopType = LoopTypes::DO_LOOP;
  m_type = "arduino-loop-flow";
  m_ui.setupUi(this);
  connect(m_ui.condition,
	  &QLineEdit::textChanged,
	  this,
	  &glitch_object::changed);
  connect(m_ui.loop_type,
	  QOverload<int>::of(&QComboBox::currentIndexChanged),
	  this,
	  &glitch_object_loop_flow_arduino::slotLoopTypeChanged);
  prepareContextMenu();
  setName(m_type);
}

glitch_object_loop_flow_arduino::~glitch_object_loop_flow_arduino()
{
}

QString glitch_object_loop_flow_arduino::code(void) const
{
  return "";
}

QString glitch_object_loop_flow_arduino::loopType(void) const
{
  return m_ui.loop_type->currentText();
}

bool glitch_object_loop_flow_arduino::hasOutput(void) const
{
  return false;
}

glitch_object_loop_flow_arduino *glitch_object_loop_flow_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_loop_flow_arduino(parent);

  clone->m_loopType = m_loopType;
  clone->m_properties = m_properties;
  clone->m_ui.condition->setText(m_ui.condition->text().trimmed());
  clone->m_ui.loop_type->setCurrentIndex(m_ui.loop_type->currentIndex());
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_loop_flow_arduino *glitch_object_loop_flow_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_loop_flow_arduino
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_ui.condition->blockSignals(true);
  object->m_ui.condition->setText
    (object->m_properties.value(Properties::CONDITION).toString().trimmed());
  object->m_ui.condition->blockSignals(false);
  return object;
}

void glitch_object_loop_flow_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_loop_flow_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["condition"] = m_ui.condition->text().trimmed();
  properties["loop_type"] = m_ui.loop_type->currentText();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_loop_flow_arduino::setLoopType(const QString &loopType)
{
  auto l(loopType.toLower().trimmed());

  if(l == "do loop")
    m_loopType = LoopTypes::DO_LOOP;
  else if(l == "for loop")
    m_loopType = LoopTypes::FOR_LOOP;
  else if(l == "while loop")
    m_loopType = LoopTypes::WHILE_LOOP;
  else
    m_loopType = LoopTypes::DO_LOOP;

  m_ui.loop_type->blockSignals(true);
  m_ui.loop_type->setCurrentIndex
    (m_ui.loop_type->findText(l, Qt::MatchEndsWith));

  if(m_ui.loop_type->currentIndex() < 0)
    m_ui.loop_type->setCurrentIndex(0);

  m_ui.loop_type->blockSignals(false);
}

void glitch_object_loop_flow_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  setLoopType(m_properties.value(glitch_object::LOOP_TYPE).toString());
}

void glitch_object_loop_flow_arduino::slotLoopTypeChanged(void)
{
  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (m_properties.value(Properties::LOOP_TYPE).toString(),
     glitch_undo_command::LOOP_TYPE_CHANGED,
     this);

  m_properties[Properties::LOOP_TYPE] = m_ui.loop_type->currentText();
  undoCommand->setText(tr("loop type changed"));
  m_undoStack->push(undoCommand);
  emit changed();
}
