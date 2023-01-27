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

#include <QQueue>

#include "glitch-object-arrow.h"

glitch_object_arrow::glitch_object_arrow(QWidget *parent):
  glitch_object_arrow(1, parent)
{
}

glitch_object_arrow::glitch_object_arrow
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "decoration-arrow";
  resize(100, 25);
  setAttribute(Qt::WA_OpaquePaintEvent, false);
  setName("decoration-arrow");
  setStyleSheet("background-color: transparent;");
}

glitch_object_arrow::~glitch_object_arrow()
{
}

glitch_object_arrow *glitch_object_arrow::clone(QWidget *parent) const
{
  auto clone = new glitch_object_arrow(parent);

  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->compressWidget
    (m_properties.value(Properties::COMPRESSED_WIDGET).toBool());
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
  const qreal sizeHeight = static_cast<qreal> (size().height());
  const qreal sizeWidth = static_cast<qreal> (size().width());
  QPointF block[4] =
    {
      QPointF(0.0, 0.0),
      QPointF(sizeWidth, 0.0),
      QPointF(sizeWidth, sizeHeight),
      QPointF(0.0, sizeHeight)
    };
  const qreal arrowPercentOfWidth = 0.10;
  const qreal linePercentOfHeight = 0.30;
  const qreal x0 = arrowPercentOfWidth * sizeWidth;
  qreal xi = x0;

  transparentColor = canvasBrush.color();
  transparentColor.setAlpha(0);
  canvasBrush.setColor(transparentColor);
  painter.setBrush(canvasBrush);
  painter.setPen(Qt::NoPen);
  painter.save();
  painter.drawConvexPolygon(block, 4);
  painter.restore();
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
      auto brush(QBrush(QColor(Qt::blue), fillPattern));
      auto color(brush.color());

      color.setAlpha(255);
      brush.setColor(color);
      painter.setBrush(brush);
      painter.save();
      painter.drawConvexPolygon(block, 4);
      painter.restore();
      widths.enqueue(width);
      xi += width;

      if(sizeHeight <= xi)
	break;
    }
  while(true);

  {
    QPointF block[4] =
      {
	QPointF(0.0, 0.0),
	QPointF(arrowPercentOfWidth * sizeWidth, 0.0),
	QPointF(arrowPercentOfWidth * sizeWidth, sizeHeight),
	QPointF(0.0, sizeHeight)
      };
    QPointF leftArrow[3] =
      {
	QPointF(0.0, sizeHeight / 2.0),
	QPointF(arrowPercentOfWidth * sizeWidth, 0.0),
	QPointF(arrowPercentOfWidth * sizeWidth, sizeHeight)
      };
    auto brush(QBrush(QColor(Qt::blue), fillPattern));
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
    painter.drawConvexPolygon(leftArrow, 3);
    painter.restore();
  }
}

void glitch_object_arrow::save(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;
}
