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

#include "glitch-canvas-preview.h"

glitch_canvas_preview::glitch_canvas_preview(QWidget *parent):QWidget(parent)
{
  m_ui.setupUi(this);
}

glitch_canvas_preview::~glitch_canvas_preview()
{
}

void glitch_canvas_preview::setScene(QGraphicsScene *scene)
{
  if(scene)
    {
      connect(scene,
	      SIGNAL(sceneRectChanged(const QRectF &)),
	      this,
	      SLOT(slotSceneRectChanged(const QRectF &)),
	      Qt::UniqueConnection);
      m_ui.view->fitInView
	(0.0, 0.0, 350.0, 350.0, Qt::KeepAspectRatioByExpanding);
      m_ui.view->centerOn(QPointF(0.0, 0.0));
      m_ui.view->setScene(scene);
    }
}

void glitch_canvas_preview::slotSceneRectChanged(const QRectF &rect)
{
  Q_UNUSED(rect);
  m_ui.view->fitInView(0.0, 0.0, 350.0, 350.0, Qt::KeepAspectRatioByExpanding);
  m_ui.view->centerOn(QPointF(0.0, 0.0));
}
