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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "glowbot-object-setup-arduino.h"
#include "glowbot-object-start.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"
#include "glowbot-view-arduino.h"

glowbot_view_arduino::glowbot_view_arduino
(const QString &name,
 const glowbot_common::ProjectType projectType,
 QWidget *parent):glowbot_view(name, projectType, parent)
{
  m_setupObject = new glowbot_object_setup_arduino(0);
  m_startObject = new glowbot_object_start(0);

  glowbot_proxy_widget *proxy = 0;

  proxy = new glowbot_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_setupObject);
  m_scene->addItem(proxy);
  proxy->setPos(150.0, 10.0);
  proxy = new glowbot_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_startObject);
  proxy->resize(m_startObject->size());
  m_scene->addItem(proxy);
  proxy->setPos(10.0, 10.0);
}

glowbot_view_arduino::~glowbot_view_arduino()
{
}
