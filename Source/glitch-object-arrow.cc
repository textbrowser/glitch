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

#include <QColorDialog>
#include <QQueue>

#include "glitch-object-arrow.h"
#include "glitch-undo-command.h"

glitch_object_arrow::glitch_object_arrow(const QString &text, QWidget *parent):
  glitch_object_arrow(1, parent)
{
  if(text.contains("left"))
    m_arrow = Arrows::LEFT;
  else if(text.contains("right"))
    m_arrow = Arrows::RIGHT;
  else
    m_arrow = Arrows::LEFT_RIGHT;
}

glitch_object_arrow::glitch_object_arrow
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_properties[Properties::BACKGROUND_COLOR] = QColor(70, 130, 180);
  m_type = "decoration-arrow";
  resize(100, 30);
  setAttribute(Qt::WA_OpaquePaintEvent, false);
  setName("decoration-arrow");
  setStyleSheet("background-color: transparent;");
}

glitch_object_arrow::~glitch_object_arrow()
{
}

glitch_object_arrow *glitch_object_arrow::clone(QWidget *parent) const
{
  auto clone = new glitch_object_arrow(arrowToString(), parent);

  clone->m_arrow = m_arrow;
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

glitch_object_arrow *glitch_object_arrow::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_arrow
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (values.value("properties").toString().split(s_splitRegularExpression));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_arrow::addActions(QMenu &menu)
{
  addDefaultActions(menu);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setEnabled(false);
}

void glitch_object_arrow::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QColor transparentColor;
  QPainter painter(this);
  QQueue<qreal> widths;
  auto canvasBrush(m_proxy && m_proxy->scene() ?
		   m_proxy->scene()->backgroundBrush() : QBrush(Qt::white));
  const Qt::BrushStyle fillPattern = Qt::SolidPattern;
  const auto sizeHeight = static_cast<qreal> (size().height());
  const auto sizeWidth = static_cast<qreal> (size().width());
  QPointF block[4] =
    {
      QPointF(0.0, 0.0),
      QPointF(sizeWidth, 0.0),
      QPointF(sizeWidth, sizeHeight),
      QPointF(0.0, sizeHeight)
    };
  const qreal arrowPercentOfWidth = 0.10;
  const qreal linePercentOfHeight = 0.30;
  const qreal x0 = (m_arrow == Arrows::LEFT || m_arrow == Arrows::LEFT_RIGHT) ?
    arrowPercentOfWidth * sizeWidth : 0.0;
  qreal xi = x0;

  transparentColor = canvasBrush.color();
  transparentColor.setAlpha(0);
  canvasBrush.setColor(transparentColor);
  painter.setBrush(canvasBrush);
  painter.setPen(Qt::NoPen);
  painter.setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
			 QPainter::LosslessImageRendering |
#endif
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing,
			 true);
  painter.save();
  painter.drawConvexPolygon(block, 4);
  painter.restore();

  if(m_arrow == Arrows::LEFT)
    widths.enqueue(sizeWidth);
  else if(m_arrow == Arrows::RIGHT)
    widths.enqueue((1.0 - arrowPercentOfWidth) * sizeWidth);
  else
    widths.enqueue((1.0 - 2.0 * arrowPercentOfWidth) * sizeWidth);

  do
    {
      const auto width = widths.dequeue();

      QPointF block[4] =
	{
	  QPointF(xi, linePercentOfHeight * sizeHeight),
	  QPointF(width + xi, linePercentOfHeight * sizeHeight),
	  QPointF(width + xi, (1.0 - linePercentOfHeight) * sizeHeight),
	  QPointF(xi, (1.0 - linePercentOfHeight) * sizeHeight)
	};
      auto brush
	(QBrush(m_properties.value(Properties::BACKGROUND_COLOR).
		value<QColor> (), fillPattern));
      auto color(brush.color());

      color.setAlpha(255);
      brush.setColor(color);
      painter.setBrush(brush);
      painter.save();
      painter.drawConvexPolygon(block, 4);
      painter.restore();
      widths.enqueue(width);
      xi += width;
      break;
    }
  while(true);

  if(m_arrow == Arrows::LEFT || m_arrow == Arrows::LEFT_RIGHT)
    {
      QPointF arrow[3] =
	{
	  QPointF(0.0, sizeHeight / 2.0),
	  QPointF(arrowPercentOfWidth * sizeWidth, 0.0),
	  QPointF(arrowPercentOfWidth * sizeWidth, sizeHeight)
	};
      QPointF block[4] =
	{
	  QPointF(0.0, 0.0),
	  QPointF(arrowPercentOfWidth * sizeWidth, 0.0),
	  QPointF(arrowPercentOfWidth * sizeWidth, sizeHeight),
	  QPointF(0.0, sizeHeight)
	};
      auto brush
	(QBrush(m_properties.value(Properties::BACKGROUND_COLOR).
		value<QColor> (), fillPattern));
      auto color(brush.color());

      painter.setBrush(canvasBrush);
      painter.setPen(Qt::NoPen);
      painter.save();
      painter.drawConvexPolygon(block, 4);
      painter.restore();
      color.setAlpha(255);
      brush.setColor(color);
      painter.setBrush(brush);
      painter.setPen(Qt::NoPen);
      painter.save();
      painter.drawConvexPolygon(arrow, 3);
      painter.restore();
    }

  if(m_arrow == Arrows::LEFT_RIGHT || m_arrow == Arrows::RIGHT)
    {
      QPointF arrow[3] =
	{
	  QPointF(sizeWidth, sizeHeight / 2.0),
	  QPointF((1.0 - arrowPercentOfWidth) * sizeWidth, 0.0),
	  QPointF((1.0 - arrowPercentOfWidth) * sizeWidth, sizeHeight)
	};
      QPointF block[4] =
	{
	  QPointF((1.0 - arrowPercentOfWidth) * sizeWidth, 0.0),
	  QPointF(sizeWidth, 0.0),
	  QPointF(sizeWidth, sizeHeight),
	  QPointF((1.0 - arrowPercentOfWidth) * sizeWidth, sizeHeight)
	};
      auto brush
	(QBrush(m_properties.value(Properties::BACKGROUND_COLOR).
		value<QColor> (), fillPattern));
      auto color(brush.color());

      painter.setBrush(canvasBrush);
      painter.setPen(Qt::NoPen);
      painter.save();
      painter.drawConvexPolygon(block, 4);
      painter.restore();
      color.setAlpha(255);
      brush.setColor(color);
      painter.setBrush(brush);
      painter.setPen(Qt::NoPen);
      painter.save();
      painter.drawConvexPolygon(arrow, 3);
      painter.restore();
    }
}

void glitch_object_arrow::save(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["arrows"] = arrowToString();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_arrow::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_arrow = Arrows::LEFT_RIGHT;

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("arrows = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_arrow = stringToArrow(string);
	}
    }

  compressWidget(m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
}

void glitch_object_arrow::setProperty
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
    default:
      {
	break;
      }
    }
}

void glitch_object_arrow::slotSelectColor(void)
{
  QColorDialog dialog(m_parent);

  dialog.setCurrentColor
    (m_properties.value(Properties::BACKGROUND_COLOR).value<QColor> ());
  dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  dialog.setWindowIcon(windowIcon());
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      auto color(dialog.selectedColor());

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
