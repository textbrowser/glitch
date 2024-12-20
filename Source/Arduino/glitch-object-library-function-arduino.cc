/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
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

#include <QInputDialog>

#include "glitch-object-library-function-arduino.h"
#include "glitch-structures-arduino.h"
#include "glitch-undo-command.h"
#include "glitch-variety.h"

glitch_object_library_function_arduino::glitch_object_library_function_arduino
(const QString &functionType, QWidget *parent):
  glitch_object_library_function_arduino(1, parent)
{
  m_text = QString("%1()").arg(functionType);
  setName(m_text);
  setToolTip(description());
}

glitch_object_library_function_arduino::glitch_object_library_function_arduino
(const qint64 id, QWidget *parent):glitch_object_simple_text_arduino(id, parent)
{
  m_properties[Properties::BACKGROUND_COLOR] = QColor(0, 71, 171);
  m_properties[Properties::FONT_COLOR] = QColor(Qt::white);
  m_type = "arduino-library-function";
  prepareContextMenu();
  setName(m_text);
}

glitch_object_library_function_arduino::
~glitch_object_library_function_arduino()
{
}

QString glitch_object_library_function_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  QString code("");

  code.append(QString(m_text).remove("()"));
  code.append("(");

  auto const list(inputs());

  for(int i = 0; i < list.size(); i++)
    {
      code.append(list.at(i));

      if(i != list.size() - 1)
	code.append(", ");
    }

  code = code.trimmed();
  code.append(");");
  return code;
}

bool glitch_object_library_function_arduino::hasInput(void) const
{
  return m_properties.value(Properties::LIBRARY_FUNCTION_HAS_INPUT).toBool();
}

bool glitch_object_library_function_arduino::hasOutput(void) const
{
  return m_properties.value(Properties::LIBRARY_FUNCTION_HAS_OUTPUT).toBool();
}

bool glitch_object_library_function_arduino::isFullyWired(void) const
{
  return !m_properties.value(Properties::LIBRARY_FUNCTION_HAS_INPUT).toBool();
}

bool glitch_object_library_function_arduino::shouldPrint(void) const
{
  return outputs().isEmpty();
}

glitch_object_library_function_arduino *glitch_object_library_function_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_library_function_arduino(m_text, parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_text = m_text;
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_library_function_arduino *glitch_object_library_function_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_library_function_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(splitPropertiesAmpersand(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_library_function_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["library_function_has_input"] = m_properties.value
    (Properties::LIBRARY_FUNCTION_HAS_INPUT).toBool();
  properties["library_function_has_output"] = m_properties.value
    (Properties::LIBRARY_FUNCTION_HAS_OUTPUT).toBool();
  properties["library_function_type"] = m_text;
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_library_function_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPRESSED_WIDGET] = false;

  QString function("none()");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("library_function_has_input = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::LIBRARY_FUNCTION_HAS_INPUT] =
	    QVariant(string.trimmed()).toBool();
	}
      else if(string.simplified().startsWith("library_function_has_output = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::LIBRARY_FUNCTION_HAS_OUTPUT] =
	    QVariant(string.trimmed()).toBool();
	}
      else if(string.simplified().startsWith("library_function_type = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  string = QString("%1").arg(string);
	  function = string.trimmed();
	}
    }

  if(m_actions.value(DefaultMenuActions::PORT_COLORS, nullptr))
    m_actions.value(DefaultMenuActions::PORT_COLORS)->setEnabled
      (hasInput() || hasOutput());

  m_text = function;
  setName(m_text);
  setToolTip(description());
}

void glitch_object_library_function_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object_simple_text_arduino::setProperty(property, value);

  switch(property)
    {
    case Properties::LIBRARY_FUNCTION_HAS_INPUT:
      {
	if(m_actions.value(DefaultMenuActions::PORT_COLORS, nullptr))
	  m_actions.value(DefaultMenuActions::PORT_COLORS)->setEnabled
	    (hasInput() || hasOutput());

	break;
      }
    case Properties::LIBRARY_FUNCTION_HAS_OUTPUT:
      {
	if(m_actions.value(DefaultMenuActions::PORT_COLORS, nullptr))
	  m_actions.value(DefaultMenuActions::PORT_COLORS)->setEnabled
	    (hasInput() || hasOutput());

	break;
      }
    case Properties::LIBRARY_FUNCTION_TYPE:
      {
	m_text = value.toString();
	setName(m_text);
	break;
      }
    default:
      {
	break;
      }
    }

  setToolTip(description());
}

void glitch_object_library_function_arduino::slotSetFunctionName(void)
{
  QInputDialog dialog(m_parent);

  dialog.setLabelText(tr("Set Function Name"));
  dialog.setTextEchoMode(QLineEdit::Normal); // A line edit!
  dialog.setTextValue(m_text);
  dialog.setWindowIcon(QIcon(":/Logo/glitch-logo.png"));
  dialog.setWindowTitle(tr("Glitch: Set Function Name"));
  dialog.resize(350, dialog.sizeHint().height());

  auto lineEdit = dialog.findChild<QLineEdit *> ();

  if(lineEdit)
    {
      lineEdit->selectAll();
      lineEdit->setMaxLength(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));
      lineEdit->setValidator

	/*
	** A mandatory letter or underscore followed by an optional word. Allow
	** trailing parentheses.
	*/

	(new QRegularExpressionValidator
	 (QRegularExpression("[A-Za-z_][\\.\\w]*\\(\\)"), &dialog));
    }
  else
    qDebug() << tr("glitch_object_library_function_arduino::"
		   "slotSetFunctionName(): "
		   "QInputDialog does not have a textfield! Cannot set "
		   "an input validator.");

 restart_label:
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      auto text(dialog.textValue().remove("(").remove(")").trimmed());

      if(text.isEmpty())
	return;
      else
	text += "()";

      if(m_text == text)
	return;

      if(glitch_structures_arduino::isReserved(text))
	{
	  glitch_variety::showErrorDialog
	    (tr("The function name %1 is a reserved keyword. "
		"Please select another name.").arg(text), m_parent);
	  goto restart_label;
	}

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (text,
	     m_text,
	     glitch_undo_command::Types::PROPERTY_CHANGED,
	     Properties::Z_Z_Z_PROPERTY,
	     this);

	  undoCommand->setText
	    (tr("item transformed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}
      else
	m_text = text;

      if(m_proxy)
	m_proxy->updateGeometry();

      emit changed();
    }
  else
    QApplication::processEvents();
}
