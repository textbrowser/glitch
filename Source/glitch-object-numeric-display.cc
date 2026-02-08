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

#include <QColorDialog>

#include "glitch-floating-context-menu.h"
#include "glitch-object-numeric-display.h"
#include "glitch-undo-command.h"

glitch_object_numeric_display::glitch_object_numeric_display(QWidget *parent):
  glitch_object_numeric_display(1, parent)
{
}

glitch_object_numeric_display::glitch_object_numeric_display
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_properties[Properties::BORDER_COLOR] = QColor(70, 130, 180, 255);
  m_type = "digitalio-numeric-display";
  resize(100, 30);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setName("digitalio-numeric-display");
  setStyleSheet("background-color: transparent;");
}

glitch_object_numeric_display::~glitch_object_numeric_display()
{
}

glitch_object_numeric_display *glitch_object_numeric_display::clone
(QWidget *parent) const
{
  auto clone = new glitch_object_numeric_display(parent);

  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->resize(size());
  clone->setAttribute
    (Qt::WA_OpaquePaintEvent, testAttribute(Qt::WA_OpaquePaintEvent));
  clone->setCanvasSettings(m_canvasSettings);
  clone->setName(clone->name());
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_numeric_display *glitch_object_numeric_display::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_numeric_display
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (splitPropertiesRegularExpression(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_numeric_display::addActions(QMenu &menu)
{
  addDefaultActions(menu);
  m_actions.value(DefaultMenuActions::BACKGROUND_COLOR)->setEnabled(false);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setChecked(false);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setEnabled(false);
  m_actions.value(DefaultMenuActions::GENERATE_SOURCE)->setChecked(false);
  m_actions.value(DefaultMenuActions::GENERATE_SOURCE)->setEnabled(false);
  m_actions.value(DefaultMenuActions::SOURCE_PREVIEW)->setEnabled(false);
}

void glitch_object_numeric_display::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QPainterPath path;
  QPen pen;
  auto const frameWidth = static_cast<qreal> (1.5);
  auto const frameWidth1 = frameWidth / 2.0;
  auto const height = static_cast<qreal> (size().height());
  auto const radius = static_cast<qreal> (5.0);
  auto const width = static_cast<qreal> (size().width());
  const QColor color(m_properties.value(Properties::BORDER_COLOR).toString());

  painter.setFont(font());
  path.addRoundedRect
    (QRectF(frameWidth1, frameWidth1, width - frameWidth, height - frameWidth),
     radius,
     radius);
  painter.fillPath
  (path,
   QBrush(QColor(m_properties.value(Properties::BACKGROUND_COLOR).toString())));
  pen.setColor(color);
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setWidthF(frameWidth);
  painter.setPen(pen);
  painter.save();
  painter.drawPath(path);
  painter.restore();
  pen.setColor(QColor(m_properties.value(Properties::FONT_COLOR).toString()));
  painter.setPen(pen);
  painter.save();
  painter.drawText
  (path.boundingRect().adjusted(5.0, 5.0, -5.0, -5.0),
   Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
   m_properties.value(Properties::NUMERIC_DISPLAY_VALUE).toString().trimmed());
  painter.restore();
}

void glitch_object_numeric_display::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_numeric_display::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));
    }

  compressWidget(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
}

void glitch_object_numeric_display::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::BORDER_COLOR:
      {
	m_properties[Properties::BORDER_COLOR] = QColor(value.toString());
	break;
      }
    default:
      {
	break;
      }
    }
}
