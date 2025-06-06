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

#include <QActionGroup>

#include "glitch-object-compound-operator-arduino.h"
#include "glitch-object-variable-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-structures-arduino.h"
#include "glitch-undo-command.h"

glitch_object_variable_arduino::glitch_object_variable_arduino
(QWidget *parent):glitch_object_variable_arduino(1, parent)
{
}

glitch_object_variable_arduino::glitch_object_variable_arduino
(const QString &variableType, QWidget *parent):
  glitch_object_variable_arduino(1, parent)
{
  auto string(variableType);

  string.remove("glitch-arduino-variables-");
  m_ui.array->setChecked(string.contains("array"));
  m_ui.name->setStyleSheet("QLineEdit {background-color: #ffc0cb;}");
  m_ui.type->setCurrentIndex
    (m_ui.type->findText(string.remove("array "), Qt::MatchFixedString));
  m_properties[Properties::VARIABLE_ARRAY] = m_ui.array->isChecked();
  m_properties[Properties::VARIABLE_NAME] = "";
  m_properties[Properties::VARIABLE_POINTER_ACCESS] = "";
  m_properties[Properties::VARIABLE_PROGMEM] = false;
  m_properties[Properties::VARIABLE_QUALIFIER] = "";
  m_properties[Properties::VARIABLE_TYPE] = m_ui.type->currentText();
}

glitch_object_variable_arduino::glitch_object_variable_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-variable";
  m_ui.setupUi(this);
  m_ui.name->setStyleSheet("QLineEdit {background-color: #ffc0cb;}");
  m_ui.qualifier->installEventFilter(new glitch_scroll_filter(this));
  m_ui.pointer_access->installEventFilter(new glitch_scroll_filter(this));
  m_ui.type->addItems
    (QStringList() << ""
                   << glitch_structures_arduino::nonArrayVariableTypes());
  m_ui.type->installEventFilter(new glitch_scroll_filter(this));
  prepareContextMenu();
  glitch_object_variable_arduino::setName(m_type);
  connectSignals(true);
}

glitch_object_variable_arduino::~glitch_object_variable_arduino()
{
}

QSize glitch_object_variable_arduino::preferredSize(void) const
{
  if(m_properties.value(Properties::COMPRESSED_WIDGET).toBool())
    {
      QFontMetrics const fontMetrics(font());
      auto const width = 5 * qCeil
	(fontMetrics.boundingRect(m_ui.name->text().trimmed()).width() / 5.0) +
	s_widthTweak;

      return QSize
	(qMax(5 * qCeil(minimumSizeHint().width() / 5.0), width),
	 minimumHeight(sizeHint().height()));
    }
  else
    return QSize(5 * qCeil(sizeHint().width() / 5.0),
		 minimumHeight(sizeHint().height()));
}

QString glitch_object_variable_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  QString assignment("=");
  auto const array(m_ui.array->isChecked() ? QString("[]") : QString(""));
  auto const inputs(this->inputs());
  auto const name(m_ui.name->text().trimmed());
  auto const pointerAccess(m_ui.pointer_access->currentText());
  auto const progmem
    (m_ui.progmem->isChecked() ? QString("PROGMEM") : QString(""));
  auto const qualifier(m_ui.qualifier->currentText());
  auto const type(m_ui.type->currentText().trimmed());

  if(glitch_object_compound_operator_arduino::isOperator(inputs.value(0)) ||
     glitch_object_compound_operator_arduino::isOperator(inputs.value(1)))
    assignment.clear();

  if(array.isEmpty())
    {
      // Non-array.

      if(type.isEmpty())
	{
	  /*
	  ** The variable is not being defined.
	  */

	  if(inputs.isEmpty())
	    return (pointerAccess + name + ";").trimmed();
	  else
	    return (pointerAccess +
		    name +
		    " " +
		    assignment +
		    " " +
		    inputs.value(0) +
		    ";").trimmed();
	}
      else
	{
	  /*
	  ** The variable is being defined.
	  */

	  if(inputs.isEmpty())
	    return (qualifier +
		    " " +
		    type +
		    " " +
		    pointerAccess +
		    name +
		    " " +
		    progmem +
		    ";").trimmed();
	  else
	    return (qualifier +
		    " " +
		    type +
		    " " +
		    pointerAccess +
		    name +
		    " " +
		    progmem +
		    " " +
		    assignment +
		    " " +
		    inputs.value(0) +
		    ";").trimmed();
	}
    }
  else
    {
      // Array.

      if(type.isEmpty())
	{
	  /*
	  ** The array is not being defined.
	  */

	  if(inputs.size() >= 2)
	    return QString("%1[%2] = %3;").
	      arg(name).
	      arg(inputs.value(0)).
	      arg(inputs.value(1));
	  else
	    return QString("%1[%2];").arg(name).arg(inputs.value(0));
	}
      else
	{
	  /*
	  ** The array is being defined.
	  */

	  if(inputs.size() >= 2)
	    return (qualifier +
		    " " +
		    type +
		    " " +
		    pointerAccess +
		    name +
		    "[" +
		    inputs.value(0) +
		    "] " +
		    progmem +
		    " " +
		    assignment +
		    " " +
		    inputs.value(1) +
		    ";").trimmed();
	  else
	    {
	      if(inputs.value(0).startsWith("{"))
		return (qualifier +
			" " +
			type +
			" " +
			pointerAccess +
			name +
			"[] " +
			progmem +
			" " +
			assignment +
			" " +
			inputs.value(0) +
			";").trimmed();
	      else
		return (qualifier +
			" " +
			type +
			" " +
			pointerAccess +
			name +
			"[" +
			inputs.value(0) +
			"] " +
			progmem +
			";").trimmed();
	    }
	}
    }
}

bool glitch_object_variable_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_variable_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_variable_arduino::isFullyWired(void) const
{
  if(m_ui.array->isChecked())
    return inputs().size() > 1;
  else
    return inputs().size() > 0;
}

bool glitch_object_variable_arduino::shouldPrint(void) const
{
  if(outputs().size() > 0)
    return false;
  else if(m_ui.array->isChecked())
    return inputs().size() == 2 ||
      m_ui.type->currentText().trimmed().length() > 0;
  else
    return inputs().size() > 0 ||
      m_ui.type->currentText().trimmed().length() > 0;
}

glitch_object_variable_arduino *glitch_object_variable_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_variable_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->connectSignals(false);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_ui.array->setChecked(m_ui.array->isChecked());
  clone->m_ui.name->setText(m_ui.name->text().trimmed());
  clone->m_ui.name->setCursorPosition(0);
  clone->m_ui.pointer_access->setCurrentIndex
    (m_ui.pointer_access->currentIndex());
  clone->m_ui.progmem->setChecked(m_ui.progmem->isChecked());
  clone->m_ui.qualifier->setCurrentIndex(m_ui.qualifier->currentIndex());
  clone->m_ui.type->setCurrentIndex(m_ui.type->currentIndex());
  clone->connectSignals(true);
  clone->prepareHighlights();
  clone->setCanvasSettings(m_canvasSettings);
  clone->setName(clone->name());
  clone->setStyleSheet(styleSheet());
  clone->compressWidget
    (m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
  clone->resize(size());
  return clone;
}

glitch_object_variable_arduino *glitch_object_variable_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_variable_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (splitPropertiesRegularExpression(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_variable_arduino::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::SET_VARIABLE_TYPE))
    {
      auto group = new QActionGroup(m_parent);
      auto list(glitch_structures_arduino::nonArrayVariableTypes());
      auto m = new QMenu(tr("Variable Type"), m_parent);

      list.prepend("");
      m->setStyleSheet("QMenu {menu-scrollable: 1;}");

      foreach(auto const &i, list)
	{
	  auto action = new QAction(i, m);

	  action->setCheckable(true);
	  action->setChecked(i == m_ui.type->currentText());
	  connect
	    (action,
	     &QAction::triggered,
	     this,
	     &glitch_object_variable_arduino::slotVariableTypeActionTriggered,
	     Qt::QueuedConnection);
	  group->addAction(action);
	  m->addAction(action);
	}

      if(group->actions().isEmpty())
	group->deleteLater();

      m_actions[DefaultMenuActions::SET_VARIABLE_TYPE] = m->menuAction();
      menu.addMenu(m);
    }
  else if(m_actions.value(DefaultMenuActions::SET_VARIABLE_TYPE, nullptr))
    menu.addMenu
      (m_actions.value(DefaultMenuActions::SET_VARIABLE_TYPE)->menu());

  addDefaultActions(menu);
}

void glitch_object_variable_arduino::compressWidget(const bool state)
{
  glitch_object::compressWidget(state);
  m_ui.array->setVisible(!state);
  m_ui.pointer_access->setVisible(!state);
  m_ui.progmem->setVisible(!state);
  m_ui.qualifier->setVisible(!state);
  m_ui.type->setVisible(!state);
  adjustSize();
  resize(preferredSize());
}

void glitch_object_variable_arduino::connectSignals(const bool state)
{
  if(state)
    {
      connect(m_ui.array,
	      SIGNAL(toggled(bool)),
	      this,
	      SLOT(slotToolButtonChecked(void)),
	      Qt::UniqueConnection);
      connect(m_ui.name,
	      &QLineEdit::editingFinished,
	      this,
	      &glitch_object_variable_arduino::slotLineEditSet,
	      Qt::UniqueConnection);
      connect(m_ui.pointer_access,
	      SIGNAL(currentIndexChanged(int)),
	      this,
	      SLOT(slotComboBoxChanged(void)),
	      Qt::UniqueConnection);
      connect(m_ui.progmem,
	      SIGNAL(toggled(bool)),
	      this,
	      SLOT(slotToolButtonChecked(void)),
	      Qt::UniqueConnection);
      connect(m_ui.qualifier,
	      SIGNAL(currentIndexChanged(int)),
	      this,
	      SLOT(slotComboBoxChanged(void)),
	      Qt::UniqueConnection);
      connect(m_ui.type,
	      SIGNAL(currentIndexChanged(int)),
	      this,
	      SLOT(slotComboBoxChanged(void)),
	      Qt::UniqueConnection);
    }
  else
    {
      disconnect(m_ui.array,
		 SIGNAL(toggled(bool)),
		 this,
		 SLOT(slotToolButtonChecked(void)));
      disconnect(m_ui.name,
		 &QLineEdit::editingFinished,
		 this,
		 &glitch_object_variable_arduino::slotLineEditSet);
      disconnect(m_ui.pointer_access,
		 SIGNAL(currentIndexChanged(int)),
		 this,
		 SLOT(slotComboBoxChanged(void)));
      disconnect(m_ui.progmem,
		 SIGNAL(toggled(bool)),
		 this,
		 SLOT(slotToolButtonChecked(void)));
      disconnect(m_ui.qualifier,
		 SIGNAL(currentIndexChanged(int)),
		 this,
		 SLOT(slotComboBoxChanged(void)));
      disconnect(m_ui.type,
		 SIGNAL(currentIndexChanged(int)),
		 this,
		 SLOT(slotComboBoxChanged(void)));
    }
}

void glitch_object_variable_arduino::prepareHighlights(void)
{
  auto text(m_ui.name->text().trimmed());

  if(glitch_structures_arduino::isReserved(text) || text.isEmpty())
    m_ui.name->setStyleSheet("QLineEdit {background-color: #ffc0cb;}");
  else if(text.remove(QRegularExpression("^[a-zA-Z_][a-zA-Z0-9_]*$")).isEmpty())
    m_ui.name->setStyleSheet("");
  else
    m_ui.name->setStyleSheet("QLineEdit {background-color: #ffc0cb;}");
}

void glitch_object_variable_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["variable_array"] = m_ui.array->isChecked();
  properties["variable_name"] = m_ui.name->text().trimmed();
  properties["variable_pointer_access"] = m_ui.pointer_access->currentText();
  properties["variable_progmem"] = m_ui.progmem->isChecked();
  properties["variable_qualifier"] = m_ui.qualifier->currentText();
  properties["variable_type"] = m_ui.type->currentText();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_variable_arduino::setName(const QString &n)
{
  auto name(n.trimmed());

  if(name.isEmpty())
    name = "arduino_variable";

  glitch_object::setName(name);
}

void glitch_object_variable_arduino::setProperties(const QStringList &list)
{
  connectSignals(false);
  glitch_object::setProperties(list);
  m_properties[Properties::VARIABLE_ARRAY] = false;
  m_properties[Properties::VARIABLE_NAME] = "";
  m_properties[Properties::VARIABLE_POINTER_ACCESS] = "";
  m_properties[Properties::VARIABLE_PROGMEM] = false;
  m_properties[Properties::VARIABLE_QUALIFIER] = "";
  m_properties[Properties::VARIABLE_TYPE] = "";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("variable_array = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_ARRAY] =
	    QVariant(string.trimmed()).toBool();
	  m_ui.array->setChecked(QVariant(string.trimmed()).toBool());
	}
      else if(string.simplified().startsWith("variable_name = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_NAME] = string.trimmed();
	  m_ui.name->setText(string.trimmed());
	  m_ui.name->setCursorPosition(0);
	  prepareHighlights();
	  setName(m_ui.name->text());
	}
      else if(string.simplified().startsWith("variable_pointer_access = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_POINTER_ACCESS] = string.trimmed();
	  m_ui.pointer_access->setCurrentIndex
	    (m_ui.pointer_access->findText(string.trimmed()));

	  if(m_ui.pointer_access->currentIndex() < 0)
	    m_ui.pointer_access->setCurrentIndex
	      (m_ui.pointer_access->findText(""));
	}
      else if(string.simplified().startsWith("variable_progmem = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_PROGMEM] =
	    QVariant(string.trimmed()).toBool();
	  m_ui.progmem->setChecked(QVariant(string.trimmed()).toBool());
	}
      else if(string.simplified().startsWith("variable_qualifier = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_QUALIFIER] = string.trimmed();
	  m_ui.qualifier->setCurrentIndex
	    (m_ui.qualifier->findText(string.trimmed()));

	  if(m_ui.qualifier->currentIndex() < 0)
	    m_ui.qualifier->setCurrentIndex(m_ui.qualifier->findText(""));
	}
      else if(string.simplified().startsWith("variable_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::VARIABLE_TYPE] = string.trimmed();
	  m_ui.type->setCurrentIndex
	    (m_ui.type->findText(string.trimmed(), Qt::MatchFixedString));

	  if(m_ui.type->currentIndex() < 0)
	    m_ui.type->setCurrentIndex(0);
	}
    }

  compressWidget(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
  connectSignals(true);
}

void glitch_object_variable_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::VARIABLE_ARRAY:
      {
	m_ui.array->blockSignals(true);
	m_ui.array->setChecked(value.toBool());
	m_ui.array->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_NAME:
      {
	m_ui.name->setText(value.toString().trimmed());
	m_ui.name->setCursorPosition(0);
	prepareHighlights();
	setName(m_ui.name->text());
	break;
      }
    case Properties::VARIABLE_POINTER_ACCESS:
      {
	m_ui.pointer_access->blockSignals(true);
	m_ui.pointer_access->setCurrentIndex
	  (m_ui.pointer_access->findText(value.toString()));
	m_ui.pointer_access->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_PROGMEM:
      {
	m_ui.progmem->blockSignals(true);
	m_ui.progmem->setChecked(value.toBool());
	m_ui.progmem->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_QUALIFIER:
      {
	m_ui.qualifier->blockSignals(true);
	m_ui.qualifier->setCurrentIndex
	  (m_ui.qualifier->findText(value.toString()));
	m_ui.qualifier->blockSignals(false);
	break;
      }
    case Properties::VARIABLE_TYPE:
      {
	auto menu = m_actions.value(DefaultMenuActions::SET_VARIABLE_TYPE) ?
	  m_actions.value(DefaultMenuActions::SET_VARIABLE_TYPE)->menu() :
	  nullptr;

	if(menu)
	  {
	    foreach(auto action, menu->actions())
	      if(action && action->text() == value.toString())
		{
		  disconnect
		    (action,
		     &QAction::triggered,
		     this,
		     &glitch_object_variable_arduino::
		     slotVariableTypeActionTriggered);
		  action->setChecked(true);
		  connect
		    (action,
		     &QAction::triggered,
		     this,
		     &glitch_object_variable_arduino::
		     slotVariableTypeActionTriggered,
		     Qt::QueuedConnection);
		  break;
		}
	  }

	m_ui.type->blockSignals(true);
	m_ui.type->setCurrentIndex
	  (m_ui.type->
	   findText(value.toString().trimmed(), Qt::MatchFixedString));

	if(m_ui.type->currentIndex() < 0)
	  m_ui.type->setCurrentIndex(0);

	m_ui.type->blockSignals(false);
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_variable_arduino::slotAdjustSize(void)
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

void glitch_object_variable_arduino::slotComboBoxChanged(void)
{
  auto comboBox = qobject_cast<QComboBox *> (sender());

  if(!comboBox)
    return;

  if(m_actions.value(DefaultMenuActions::SET_VARIABLE_TYPE, nullptr) &&
     m_ui.type == sender())
    {
      auto menu = m_actions.value
	(DefaultMenuActions::SET_VARIABLE_TYPE)->menu();

      if(menu)
	{
	  foreach(auto action, menu->actions())
	    if(action && action->text() == m_ui.type->currentText())
	      {
		disconnect
		  (action,
		   &QAction::triggered,
		   this,
		   &glitch_object_variable_arduino::slotComboBoxChanged);
		action->setChecked(true);
		connect
		  (action,
		   &QAction::triggered,
		   this,
		   &glitch_object_variable_arduino::slotComboBoxChanged,
		   Qt::QueuedConnection);
		break;
	      }
	}
    }

  auto property = glitch_object::Properties::Z_Z_Z_PROPERTY;

  if(comboBox == m_ui.pointer_access)
    property = glitch_object::Properties::VARIABLE_POINTER_ACCESS;
  else if(comboBox == m_ui.qualifier)
    property = glitch_object::Properties::VARIABLE_QUALIFIER;
  else
    property = glitch_object::Properties::VARIABLE_TYPE;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(comboBox->currentText(),
	 m_properties.value(property),
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 property,
	 this);

      m_properties[property] = comboBox->currentText();
      undoCommand->setText
	(tr("variable property changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[property] = comboBox->currentText();

  emit changed();
}

void glitch_object_variable_arduino::slotLineEditSet(void)
{
  auto lineEdit = qobject_cast<QLineEdit *> (sender());

  if(!lineEdit)
    return;

  lineEdit->setText(lineEdit->text().trimmed());
  lineEdit->setCursorPosition(0);
  prepareHighlights();

  auto const property = glitch_object::Properties::VARIABLE_NAME;

  if(lineEdit->text() == m_properties.value(property).toString())
    return;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(lineEdit->text(),
	 m_properties.value(property),
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 property,
	 this);

      m_properties[property] = lineEdit->text();
      undoCommand->setText
	(tr("variable property changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[property] = lineEdit->text();

  lineEdit->selectAll();
  emit changed();
}

void glitch_object_variable_arduino::slotToolButtonChecked(void)
{
  auto toolButton = qobject_cast<QToolButton *> (sender());

  if(!toolButton)
    return;

  auto property = glitch_object::Properties::VARIABLE_ARRAY;

  if(m_ui.progmem == toolButton)
    property = glitch_object::Properties::VARIABLE_PROGMEM;

  if(m_undoStack)
    {
      auto undoCommand = new glitch_undo_command
	(toolButton->isChecked(),
	 m_properties.value(property),
	 glitch_undo_command::Types::PROPERTY_CHANGED,
	 property,
	 this);

      m_properties[property] = toolButton->isChecked();
      undoCommand->setText
	(tr("variable property changed (%1, %2)").
	 arg(scenePos().x()).arg(scenePos().y()));
      m_undoStack->push(undoCommand);
    }
  else
    m_properties[property] = toolButton->isChecked();

  emit changed();
}

void glitch_object_variable_arduino::slotVariableTypeActionTriggered(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  auto const index = m_ui.type->findText(action->text(), Qt::MatchFixedString);

  if(index >= 0)
    m_ui.type->setCurrentIndex(index);
}
