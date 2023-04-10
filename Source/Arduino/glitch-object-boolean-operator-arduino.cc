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

#include "glitch-object-boolean-operator-arduino.h"
#include "glitch-scroll-filter.h"
#include "glitch-undo-command.h"

glitch_object_boolean_operator_arduino::
glitch_object_boolean_operator_arduino
(QWidget *parent):glitch_object_boolean_operator_arduino(1, parent)
{
}

glitch_object_boolean_operator_arduino::
glitch_object_boolean_operator_arduino
(const QString &operatorType, QWidget *parent):
  glitch_object_boolean_operator_arduino(1, parent)
{
  setOperatorType(operatorType);
}

glitch_object_boolean_operator_arduino::
glitch_object_boolean_operator_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_color = QColor(230, 230, 250);
  m_operatorType = OperatorTypes::NOT_OPERATOR;
  m_type = "arduino-booleanoperator";
  prepareContextMenu();
  resize(50, 50);
  setAttribute(Qt::WA_OpaquePaintEvent, false);
  setOperatorType(m_operatorType);
  setStyleSheet("background-color: transparent;");
}

glitch_object_boolean_operator_arduino::
~glitch_object_boolean_operator_arduino()
{
}

QString glitch_object_boolean_operator_arduino::booleanOperator(void) const
{
  switch(m_operatorType)
    {
    case OperatorTypes::AND_OPERATOR:
      {
	return "&&";
      }
    case OperatorTypes::OR_OPERATOR:
      {
	return "||";
      }
    default:
      {
	return "!";
      }
    }
}

QString glitch_object_boolean_operator_arduino::code(void) const
{
  switch(m_operatorType)
    {
    case OperatorTypes::NOT_OPERATOR:
      {
	return QString("(!(%1))").arg(inputs().value(0));
      }
    default:
      {
	QString string("(");
	auto list(inputs());

	for(int i = 0; i < list.size(); i++)
	  {
	    string.append(QString("(%1)").arg(list.at(i)));

	    if(i != list.size() - 1)
	      string.append(QString(" %1 ").arg(booleanOperator()));
	  }

	string = string.trimmed();
	string.append(")");
	return string;
      }
    }
}

bool glitch_object_boolean_operator_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_boolean_operator_arduino::hasOutput(void) const
{
  return true;
}

bool glitch_object_boolean_operator_arduino::isFullyWired(void) const
{
  switch(m_operatorType)
    {
    case OperatorTypes::NOT_OPERATOR:
      {
	return inputs().size() >= 1;
      }
    default:
      {
	return false;
      }
    }
}

bool glitch_object_boolean_operator_arduino::shouldPrint(void) const
{
  return false;
}

glitch_object_boolean_operator_arduino *
glitch_object_boolean_operator_arduino::clone(QWidget *parent) const
{
  auto clone = new glitch_object_boolean_operator_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_color = m_color;
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->resize(size());
  clone->setAttribute
    (Qt::WA_OpaquePaintEvent, testAttribute(Qt::WA_OpaquePaintEvent));
  clone->setCanvasSettings(m_canvasSettings);
  clone->setOperatorType(m_operatorType);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_boolean_operator_arduino *
glitch_object_boolean_operator_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_boolean_operator_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (values.value("properties").toString().split(s_splitRegularExpression));
  object->setStyleSheet(values.value("stylesheet").toString());
  object->m_properties[Properties::TRANSPARENT] = true;
  return object;
}

void glitch_object_boolean_operator_arduino::addActions(QMenu &menu)
{
  addDefaultActions(menu);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setEnabled(false);
  m_actions.value(DefaultMenuActions::TRANSPARENT)->setEnabled(false);
}

void glitch_object_boolean_operator_arduino::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);
  auto brush(QBrush(m_color, Qt::SolidPattern));
  auto color(brush.color());
  const auto h = static_cast<qreal> (size().height());
  const auto w = static_cast<qreal> (size().width());

  color.setAlpha(255);
  brush.setColor(color);
  painter.setBrush(brush);
  painter.setPen(Qt::NoPen);
  painter.setRenderHints(QPainter::Antialiasing |
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing,
			 true);

  switch(m_operatorType)
    {
    case OperatorTypes::AND_OPERATOR:
      {
	if(m_path.isEmpty())
	  {
	    QPolygonF polygon;

	    polygon << QPointF(w / 2.0, 0.0)
		    << QPointF(0.0, 0.0)
		    << QPointF(0.0, h)
		    << QPointF(w / 2.0, h);
	    m_path.addPolygon(polygon);
	    m_path.arcTo(rect(), -90.0, 180.0);
	  }

	painter.fillPath(m_path, brush);
	break;
      }
    case OperatorTypes::OR_OPERATOR:
      {
	if(m_path.isEmpty())
	  {
	    m_path.moveTo(0.0, 0.0);
	    m_path.quadTo(QPointF(w / 2.0, 0), QPointF(w, h / 2.0));
	    m_path.quadTo(QPointF(w / 2.0, h), QPointF(0.0, h));
	    m_path.quadTo(QPointF(20.0, h / 2.0), QPointF(0.0, 0.0));
	  }

	painter.fillPath(m_path, brush);
	break;
      }
    default:
      {
	if(m_path.isEmpty())
	  {
	    QPolygonF polygon;

	    polygon << QPointF(0.0, 0.0)
		    << QPointF(w, h / 2.0)
		    << QPointF(0.0, h)
		    << QPointF(0.0, 0.0);
	    m_path.addPolygon(polygon);
	  }

	painter.fillPath(m_path, brush);
	break;
      }
    }
}

void glitch_object_boolean_operator_arduino::resizeEvent(QResizeEvent *event)
{
  glitch_object::resizeEvent(event);
  m_path.clear();
}

void glitch_object_boolean_operator_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["boolean_operator"] = booleanOperator();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_boolean_operator_arduino::setOperatorType
(const QString &operatorType)
{
  if(operatorType.contains("&&"))
    setOperatorType(OperatorTypes::AND_OPERATOR);
  else if(operatorType.contains("||"))
    setOperatorType(OperatorTypes::OR_OPERATOR);
  else
    setOperatorType(OperatorTypes::NOT_OPERATOR);
}

void glitch_object_boolean_operator_arduino::setOperatorType
(const OperatorTypes operatorType)
{
  m_operatorType = operatorType;
  setName(booleanOperator());
}

void glitch_object_boolean_operator_arduino::setProperties
(const QStringList &list)
{
  glitch_object::setProperties(list);

  QString value("!");

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("boolean_operator = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  value = string.trimmed();
	}
    }

  setOperatorType(value);
}

void glitch_object_boolean_operator_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::BOOLEAN_OPERATOR:
      {
	setOperatorType(value.toString());
	break;
      }
    default:
      {
	break;
      }
    }
}
