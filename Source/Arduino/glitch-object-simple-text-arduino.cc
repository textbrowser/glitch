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

#include "glitch-object-simple-text-arduino.h"

glitch_object_simple_text_arduino::glitch_object_simple_text_arduino
(QWidget *parent):glitch_object_simple_text_arduino(1, parent)
{
}

glitch_object_simple_text_arduino::glitch_object_simple_text_arduino
(const QString &text, QWidget *parent):
  glitch_object_simple_text_arduino(1, parent)
{
  m_text = text;
}

glitch_object_simple_text_arduino::glitch_object_simple_text_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_properties[Properties::BACKGROUND_COLOR] = QColor(230, 230, 250);
  setAttribute(Qt::WA_OpaquePaintEvent, false);
  setStyleSheet("background-color: transparent;");
}

glitch_object_simple_text_arduino::~glitch_object_simple_text_arduino()
{
}

void glitch_object_simple_text_arduino::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);
  auto brush
    (QBrush(m_properties.value(Properties::BACKGROUND_COLOR).value<QColor> (),
	    Qt::SolidPattern));
  auto color(brush.color());
  auto font(this->font());

  color.setAlpha(255);
  brush.setColor(color);
  font.setBold(true);
  painter.setBrush(brush);
  painter.setFont(font);
  painter.setPen(m_properties.value(Properties::FONT_COLOR).value<QColor> ());
  painter.setRenderHints(QPainter::Antialiasing |
			 QPainter::SmoothPixmapTransform |
			 QPainter::TextAntialiasing,
			 true);

  if(m_path.isEmpty())
    m_path.addRoundedRect(rect(), 5.0, 5.0);

  painter.drawPath(m_path);
  painter.drawText(rect(), Qt::AlignCenter, m_text);
}

void glitch_object_simple_text_arduino::preparePromotionMenu(QMenu *menu)
{
  if(!menu || !menu->actions().isEmpty() || m_functionsList.isEmpty())
    return;

  menu->addSeparator();

  for(int i = 0; i < m_functionsList.size(); i++)
    {
      auto action = new QAction
	(QString(m_functionsList.at(i)).replace('&', "&&"), this);

      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_simple_text_arduino::slotPromoted);
      menu->addAction(action);
    }
}

void glitch_object_simple_text_arduino::resizeEvent(QResizeEvent *event)
{
  glitch_object::resizeEvent(event);
  m_path = QPainterPath();
}

void glitch_object_simple_text_arduino::slotPromoted(void)
{
}
