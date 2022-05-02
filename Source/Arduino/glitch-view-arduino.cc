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

#include <QSqlQuery>
#include <QtDebug>

#include "glitch-object-loop-arduino.h"
#include "glitch-object-setup-arduino.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-view-arduino.h"

glitch_view_arduino::glitch_view_arduino
(const QString &fileName,
 const QString &name,
 const bool fromFile,
 const glitch_common::ProjectTypes projectType,
 QWidget *parent):glitch_view(fileName, name, projectType, parent)
{
  Q_UNUSED(fromFile);
  m_loopObject = new glitch_object_loop_arduino(this);
  m_setupObject = new glitch_object_setup_arduino(this);

  glitch_proxy_widget *proxy = nullptr;

  proxy = new glitch_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsMovable, false);
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_loopObject);
  proxy->resize(m_loopObject->size());
  m_loopObject->setProxy(proxy);
  m_scene->addItem(proxy);
  proxy->setPos(QPointF(10.0, 10.0));
  proxy = new glitch_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsMovable, false);
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_setupObject);
  proxy->resize(m_setupObject->size());
  m_scene->addItem(proxy);
  m_setupObject->setProxy(proxy);
  proxy->setPos(QPointF(m_loopObject->width() + 20.0, 10.0));
  connect(m_loopObject,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
  connect(m_scene,
	  SIGNAL(functionAdded(const QString &, const bool)),
	  this,
	  SLOT(slotFunctionAdded(const QString &, const bool)));
  connect(m_scene,
	  SIGNAL(functionDeleted(const QString &)),
	  this,
	  SLOT(slotFunctionDeleted(const QString &)));
  connect(m_setupObject,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotChanged(void)));
}

glitch_view_arduino::~glitch_view_arduino()
{
}

QString glitch_view_arduino::nextUniqueFunctionName(void) const
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

bool glitch_view_arduino::containsFunctionName(const QString &name) const
{
  return m_functionNames.contains(name);
}

bool glitch_view_arduino::open(const QString &fileName, QString &error)
{
  auto ok = glitch_view::open(fileName, error);

  if(!ok)
    return ok;

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec(QString("SELECT "
			      "SUBSTR(stylesheet, 1, %1), "
			      "SUBSTR(type, 1, 100) "
			      "FROM objects WHERE "
			      "type IN ('arduino-loop', 'arduino-setup')").
		      arg(static_cast<int> (glitch_view::Limits::
					    STYLESHEET_MAXIMUM_LENGTH))))
	  while(query.next())
	    {
	      auto styleSheet(query.value(0).toString().trimmed());
	      auto type(query.value(1).toString().toLower().trimmed());

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

  glitch_common::discardDatabase(connectionName);
  return ok;
}

glitch_object_loop_arduino *glitch_view_arduino::loopObject(void) const
{
  return m_loopObject;
}

glitch_object_setup_arduino *glitch_view_arduino::setupObject(void) const
{
  return m_setupObject;
}

void glitch_view_arduino::consumeFunctionName(const QString &name)
{
  if(!name.trimmed().isEmpty())
    m_functionNames[name] = '0';
}

void glitch_view_arduino::generateSource(void)
{
  /*
  ** Generate loop().
  */

  /*
  ** Generate setup().
  */
}

void glitch_view_arduino::removeFunctionName(const QString &name)
{
  m_functionNames.remove(name);
}

void glitch_view_arduino::slotFunctionAdded(const QString &name,
					    const bool isClone)
{
  if(!isClone)
    consumeFunctionName(name);
}

void glitch_view_arduino::slotFunctionDeleted(const QString &name)
{
  removeFunctionName(name);
}
