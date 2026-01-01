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
#include "glitch-object-frame.h"
#include "glitch-undo-command.h"

glitch_object_frame::glitch_object_frame(QWidget *parent):
  glitch_object_frame(1, parent)
{
}

glitch_object_frame::glitch_object_frame
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_properties[Properties::BACKGROUND_COLOR] = QColor(70, 130, 180, 255);
  m_properties[Properties::FRAME_OBJECT_RADIUS] = 10.0;
  m_type = "decoration-frame";
  resize(100, 30);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setName("decoration-frame");
  setStyleSheet("background-color: transparent;");
}

glitch_object_frame::~glitch_object_frame()
{
}

glitch_object_frame *glitch_object_frame::clone(QWidget *parent) const
{
  auto clone = new glitch_object_frame(parent);

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

glitch_object_frame *glitch_object_frame::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_frame
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (splitPropertiesRegularExpression(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_frame::addActions(QMenu &menu)
{
  addDefaultActions(menu);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setChecked(false);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setEnabled(false);
  m_actions.value(DefaultMenuActions::GENERATE_SOURCE)->setChecked(false);
  m_actions.value(DefaultMenuActions::GENERATE_SOURCE)->setEnabled(false);
  m_actions.value(DefaultMenuActions::SOURCE_PREVIEW)->setEnabled(false);
}

void glitch_object_frame::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  QPainterPath path;
  QPen pen;
  auto const frameWidth = static_cast<qreal> (5.0);
  auto const frameWidth1 = frameWidth / 2.0;
  auto const height = static_cast<qreal> (size().height());
  auto const radius = static_cast<qreal>
    (m_properties.value(Properties::FRAME_OBJECT_RADIUS).toDouble());
  auto const width = static_cast<qreal> (size().width());
  const QColor color
    (m_properties.value(Properties::BACKGROUND_COLOR).toString());

  path.addRoundedRect
    (QRectF(frameWidth1, frameWidth1, width - frameWidth, height - frameWidth),
     radius,
     radius);
  painter.fillPath(path, QBrush(QColor(Qt::transparent)));
  pen.setColor(color);
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setWidthF(frameWidth);
  painter.setPen(pen);
  painter.save();
  painter.drawPath(path);
  painter.restore();
}

void glitch_object_frame::save(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["frame_object_radius"] = m_properties.value
    (Properties::FRAME_OBJECT_RADIUS);
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_frame::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("frame_object_radius = "))
	{
	  string = string.mid(string.indexOf('=') + 1).toLower();
	  string.remove("\"");
	  string = string.trimmed();
	  m_properties[Properties::FRAME_OBJECT_RADIUS] =
	    qAbs(string.toDouble());
	  break;
	}
    }

  compressWidget(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
}

void glitch_object_frame::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::BACKGROUND_COLOR:
      {
	m_properties[Properties::BACKGROUND_COLOR] = QColor(value.toString());
	break;
      }
    case Properties::FRAME_OBJECT_RADIUS:
      {
	m_properties[Properties::FRAME_OBJECT_RADIUS] = value;
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_frame::slotSelectColor(void)
{
  QColorDialog dialog(m_parent);

  dialog.setCurrentColor
    (m_properties.value(Properties::BACKGROUND_COLOR).value<QColor> ());
  dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  dialog.setWindowIcon(windowIcon());
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      auto const color(dialog.selectedColor());

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (color.name(QColor::HexArgb),
	     m_properties.value(Properties::BACKGROUND_COLOR),
	     glitch_undo_command::Types::PROPERTY_CHANGED,
	     Properties::BACKGROUND_COLOR,
	     this);

	  undoCommand->setText
	    (tr("color changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}
    }
  else
    QApplication::processEvents();
}
