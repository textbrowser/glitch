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

#include <QSqlQuery>

#include "glowbot-object-loop-arduino.h"
#include "glowbot-object-setup-arduino.h"
#include "glowbot-proxy-widget.h"
#include "glowbot-scene.h"
#include "glowbot-view-arduino.h"

glowbot_view_arduino::glowbot_view_arduino
(const QString &name,
 const bool fromFile,
 const glowbot_common::ProjectType projectType,
 QWidget *parent):glowbot_view(name, projectType, parent)
{
  if(fromFile)
    {
      m_loopObject = new glowbot_object_loop_arduino(LOOP_OBJECT_ID, this);
      m_setupObject = new glowbot_object_setup_arduino(SETUP_OBJECT_ID, this);
    }
  else
    {
      m_loopObject = new glowbot_object_loop_arduino(this);
      m_setupObject = new glowbot_object_setup_arduino(this);
    }

  glowbot_proxy_widget *proxy = 0;

  proxy = new glowbot_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_loopObject);
  proxy->resize(m_loopObject->size());
  m_scene->addItem(proxy);
  proxy->setPos(10.0, 10.0);
  proxy = new glowbot_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_setupObject);
  proxy->resize(m_setupObject->size());
  m_scene->addItem(proxy);
  proxy->setPos(m_loopObject->width() + 20.0, 10.0);
  connect(m_loopObject,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
  connect(m_setupObject,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
}

glowbot_view_arduino::~glowbot_view_arduino()
{
}

QString glowbot_view_arduino::nextUniqueFunctionName(void) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString name("function_0()");
  quint64 i = 0;

  while(m_functionNames.contains(name))
    {
      i += 1;
      name = QString("function_%1()").arg(i);
    }

  QApplication::restoreOverrideCursor();
  return name;
}

bool glowbot_view_arduino::containsFunctionName(const QString &name) const
{
  return m_functionNames.contains(name);
}

bool glowbot_view_arduino::open(const QString &fileName, QString &error)
{
  bool ok = glowbot_view::open(fileName, error);

  if(!ok)
    return ok;

  QString connectionName("");

  {
    QSqlDatabase db(glowbot_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec("SELECT stylesheet, type FROM objects WHERE "
		      "type IN ('arduino-loop', 'arduino-setup')"))
	  while(query.next())
	    {
	      QString styleSheet(query.value(0).toString().trimmed());
	      QString type(query.value(1).toString().toLower().trimmed());

	      if(type == "arduino-loop")
		m_loopObject->setStyleSheet(styleSheet);
	      else
		m_setupObject->setStyleSheet(styleSheet);
	    }
	else
	  {
	    error = tr("An error occurred while accessing the objects table.");
	    ok = false;
	  }
      }

    db.close();
  }

  glowbot_common::discardDatabase(connectionName);
  return ok;
}

void glowbot_view_arduino::consumeFunctionName(const QString &name)
{
  m_functionNames[name] = 0;
}

void glowbot_view_arduino::removeFunctionName(const QString &name)
{
  m_functionNames.remove(name);
}
