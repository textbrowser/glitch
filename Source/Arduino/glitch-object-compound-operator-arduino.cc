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

#include "glitch-object-compound-operator-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-undo-command.h"

glitch_object_compound_operator_arduino::
glitch_object_compound_operator_arduino
(QWidget *parent):glitch_object_compound_operator_arduino(1, parent)
{
}

glitch_object_compound_operator_arduino::
glitch_object_compound_operator_arduino
(const QString &operatorType, QWidget *parent):
  glitch_object_compound_operator_arduino(1, parent)
{
  setOperatorType(operatorType);
  m_properties[Properties::COMPOUND_OPERATOR] =
    m_ui.compound_operator->currentText();
  m_properties[Properties::COMPOUND_OPERATOR_PRE] = false;
}

glitch_object_compound_operator_arduino::
glitch_object_compound_operator_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_operatorType = OperatorTypes::MODULO_OPERATOR;
  m_type = "arduino-compoundoperator";
  m_ui.setupUi(this);
  m_ui.compound_operator->installEventFilter(new glitch_scroll_filter(this));
  m_ui.pre->setVisible(false);
  connect(m_ui.compound_operator,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotCompoundOperatorChanged(void)));
  connect(m_ui.pre,
	  SIGNAL(toggled(bool)),
	  this,
	  SLOT(slotPreToggled(bool)));
  prepareContextMenu();
  setOperatorType(m_operatorType);
  m_properties[Properties::COMPOUND_OPERATOR] =
    m_ui.compound_operator->currentText();
  m_properties[Properties::COMPOUND_OPERATOR_PRE] = false;
}

glitch_object_compound_operator_arduino::
~glitch_object_compound_operator_arduino()
{
}

QSize glitch_object_compound_operator_arduino::preferredSize(void) const
{
  if(m_operatorType == OperatorTypes::DECREMENT_OPERATOR ||
     m_operatorType == OperatorTypes::INCREMENT_OPERATOR)
    {
      QFontMetrics fontMetrics(font());
      auto width = 5 * qCeil
	(fontMetrics.boundingRect(m_ui.compound_operator->
				  currentText().trimmed()).width() /
	 5.0) +
	(compressed() == false ?
	 5 * qCeil(fontMetrics.boundingRect(m_ui.pre->text().trimmed()).
		   width() / 5.0) : 0) +
	s_widthTweak;

      return QSize
	(qMax(5 * qCeil(minimumSizeHint().width() / 5.0), width),
	 minimumHeight(sizeHint().height()));
    }
  else
    return QSize(5 * qCeil(sizeHint().width() / 5.0),
		 minimumHeight(sizeHint().height()));
}

QString glitch_object_compound_operator_arduino::
compoundOperator(void) const
{
  return m_ui.compound_operator->currentText();
}

QString glitch_object_compound_operator_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  switch(m_operatorType)
    {
    case OperatorTypes::DECREMENT_OPERATOR:
    case OperatorTypes::INCREMENT_OPERATOR:
      {
	QString string("");

	if(m_ui.pre->isChecked())
	  {
	    string.append(m_ui.compound_operator->currentText());
	    string.append("(");
	    string.append(inputs().value(0));
	    string.append(")");
	  }
	else
	  {
	    string.append("(");
	    string.append(inputs().value(0));
	    string.append(")");
	    string.append(m_ui.compound_operator->currentText());
	  }

	string.append(";");
	return string;
      }
    default:
      {
	QString string("");

	string.append(m_ui.compound_operator->currentText());
	string.append(" (");
	string.append(inputs().value(0));
	string.append(")");
	return string;
      }
    }
}

bool glitch_object_compound_operator_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_compound_operator_arduino::hasOutput(void) const
{
  switch(m_operatorType)
    {
    case OperatorTypes::DECREMENT_OPERATOR:
    case OperatorTypes::INCREMENT_OPERATOR:
      {
	return false;
      }
    default:
      {
	return true;
      }
    }
}

bool glitch_object_compound_operator_arduino::isFullyWired(void) const
{
  switch(m_operatorType)
    {
    default:
      {
	return inputs().size() >= 1;
      }
    }
}

bool glitch_object_compound_operator_arduino::isOperator(const QString &o)
{
  auto op(o.trimmed());

  return op.startsWith("%= ") ||
    op.startsWith("&= ") ||
    op.startsWith("*= ") ||
    op.startsWith("+= ") ||
    op.startsWith("-= ") ||
    op.startsWith("/= ") ||
    op.startsWith("^= ") ||
    op.startsWith("|= ");
}

bool glitch_object_compound_operator_arduino::shouldPrint(void) const
{
  switch(m_operatorType)
    {
    case OperatorTypes::DECREMENT_OPERATOR:
    case OperatorTypes::INCREMENT_OPERATOR:
      {
	return !inputs().isEmpty();
      }
    default:
      {
	return false;
      }
    }
}

glitch_object_compound_operator_arduino *
glitch_object_compound_operator_arduino::clone(QWidget *parent) const
{
  auto clone = new glitch_object_compound_operator_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->m_ui.pre->blockSignals(true);
  clone->m_ui.pre->setChecked(m_ui.pre->isChecked());
  clone->m_ui.pre->blockSignals(false);
  clone->setCanvasSettings(m_canvasSettings);
  clone->setOperatorType(m_operatorType);
  clone->setStyleSheet(styleSheet());
  clone->compressWidget
    (m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
  clone->resize(size());
  return clone;
}

glitch_object_compound_operator_arduino *
glitch_object_compound_operator_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_compound_operator_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (values.value("properties").toString().split(s_splitRegularExpression));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_compound_operator_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
}

void glitch_object_compound_operator_arduino::compressWidget(const bool state)
{
  glitch_object::compressWidget(state);

  if(state)
    m_ui.pre->setVisible(false);
  else
    m_ui.pre->setVisible(m_operatorType == OperatorTypes::DECREMENT_OPERATOR ||
			 m_operatorType == OperatorTypes::INCREMENT_OPERATOR);

  adjustSize();
  resize(preferredSize());
}

void glitch_object_compound_operator_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["compound_operator"] = m_ui.compound_operator->currentText();
  properties["compound_operator_pre"] = m_ui.pre->isChecked();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_compound_operator_arduino::setOperatorType
(const OperatorTypes operatorType)
{
  m_operatorType = operatorType;
  m_ui.compound_operator->blockSignals(true);

  switch(m_operatorType)
    {
    case OperatorTypes::ADDITION_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("+="));
	break;
      }
    case OperatorTypes::BITWISE_AND_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("&="));
	break;
      }
    case OperatorTypes::BITWISE_OR_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("|="));
	break;
      }
    case OperatorTypes::BITWISE_XOR_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("^="));
	break;
      }
    case OperatorTypes::DECREMENT_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("--"));
	break;
      }
    case OperatorTypes::DIVISION_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("/="));
	break;
      }
    case OperatorTypes::INCREMENT_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("++"));
	break;
      }
    case OperatorTypes::MODULO_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("%="));
	break;
      }
    case OperatorTypes::MULTIPLICATION_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("*="));
	break;
      }
    case OperatorTypes::SUBTRACTION_OPERATOR:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("-="));
	break;
      }
    default:
      {
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText("%="));
	break;
      }
    }

  m_ui.compound_operator->blockSignals(false);

  if(m_properties.value(Properties::COMPRESSED_WIDGET).toBool())
    m_ui.pre->setVisible(false);
  else
    m_ui.pre->setVisible(m_operatorType == OperatorTypes::DECREMENT_OPERATOR ||
			 m_operatorType == OperatorTypes::INCREMENT_OPERATOR);

  setName(m_ui.compound_operator->currentText());
}

void glitch_object_compound_operator_arduino::setOperatorType
(const QString &op)
{
  auto operatorType(op);

  operatorType.remove("glitch-arduino-compound operators-");

  if(operatorType.contains("%="))
    setOperatorType(OperatorTypes::MODULO_OPERATOR);
  else if(operatorType.contains("&="))
    setOperatorType(OperatorTypes::BITWISE_AND_OPERATOR);
  else if(operatorType.contains("*="))
    setOperatorType(OperatorTypes::MULTIPLICATION_OPERATOR);
  else if(operatorType.contains("++"))
    setOperatorType(OperatorTypes::INCREMENT_OPERATOR);
  else if(operatorType.contains("+="))
    setOperatorType(OperatorTypes::ADDITION_OPERATOR);
  else if(operatorType.contains("--"))
    setOperatorType(OperatorTypes::DECREMENT_OPERATOR);
  else if(operatorType.contains("-="))
    setOperatorType(OperatorTypes::SUBTRACTION_OPERATOR);
  else if(operatorType.contains("/="))
    setOperatorType(OperatorTypes::DIVISION_OPERATOR);
  else if(operatorType.contains("^="))
    setOperatorType(OperatorTypes::BITWISE_XOR_OPERATOR);
  else if(operatorType.contains("|="))
    setOperatorType(OperatorTypes::BITWISE_OR_OPERATOR);
  else
    setOperatorType(OperatorTypes::MODULO_OPERATOR);
}

void glitch_object_compound_operator_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMPOUND_OPERATOR] = "%=";
  m_properties[Properties::COMPOUND_OPERATOR_PRE] = false;

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("compound_operator = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMPOUND_OPERATOR] = string.trimmed();
	}
      else if(string.simplified().startsWith("compound_operator_pre = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMPOUND_OPERATOR_PRE] =
	    QVariant(string.trimmed()).toBool();
	  m_ui.pre->blockSignals(true);
	  m_ui.pre->setChecked
	    (m_properties.value(Properties::COMPOUND_OPERATOR_PRE).toBool());
	  m_ui.pre->blockSignals(false);
	}
    }

  compressWidget(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
  setOperatorType(m_properties.value(Properties::COMPOUND_OPERATOR).toString());
}

void glitch_object_compound_operator_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::COMPOUND_OPERATOR:
      {
	m_ui.compound_operator->blockSignals(true);
	m_ui.compound_operator->setCurrentIndex
	  (m_ui.compound_operator->findText(value.toString()));
	m_ui.compound_operator->blockSignals(false);
	setOperatorType(value.toString());
	break;
      }
    case Properties::COMPOUND_OPERATOR_PRE:
      {
	m_ui.pre->blockSignals(true);
	m_ui.pre->setChecked(value.toBool());
	m_ui.pre->blockSignals(false);
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_compound_operator_arduino::slotAdjustSize(void)
{
  auto before(size());

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

void glitch_object_compound_operator_arduino::slotCompoundOperatorChanged
(void)
{
  setOperatorType(m_ui.compound_operator->currentText());

  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (m_ui.compound_operator->currentText(),
     m_properties.value(Properties::COMPOUND_OPERATOR).toString(),
     glitch_undo_command::Types::PROPERTY_CHANGED,
     Properties::COMPOUND_OPERATOR,
     this);

  m_properties[Properties::COMPOUND_OPERATOR] =
    m_ui.compound_operator->currentText();
  undoCommand->setText
    (tr("compound operator changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}

void glitch_object_compound_operator_arduino::slotPreToggled(bool state)
{
  if(!m_undoStack)
    return;

  auto undoCommand = new glitch_undo_command
    (state,
     m_properties.value(Properties::COMPOUND_OPERATOR_PRE).toString(),
     glitch_undo_command::Types::PROPERTY_CHANGED,
     Properties::COMPOUND_OPERATOR_PRE,
     this);

  m_properties[Properties::COMPOUND_OPERATOR] =
    m_ui.compound_operator->currentText();
  undoCommand->setText
    (tr("compound operator changed (%1, %2)").
     arg(scenePos().x()).arg(scenePos().y()));
  m_undoStack->push(undoCommand);
  emit changed();
}
