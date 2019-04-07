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

#include <QSqlError>
#include <QSqlQuery>
#include <QUndoStack>
#include <QtDebug>

#include "Arduino/glowbot-object-analog-read-arduino.h"
#include "Arduino/glowbot-object-function-arduino.h"
#include "glowbot-object-view.h"
#include "glowbot-object.h"
#include "glowbot-scene.h"
#include "glowbot-style-sheet.h"
#include "glowbot-view.h"

glowbot_object::glowbot_object(QWidget *parent):
  QWidget(nullptr), m_initialized(false)
{
  QWidget *p = parent;
  glowbot_view *view = nullptr;

  do
    {
      if(!p)
	break;

      view = qobject_cast<glowbot_view *> (p);

      if(view)
	break;

      p = p->parentWidget();
    }
  while(true);

  m_editView = nullptr;

  if(view)
    m_id = view->nextId();
  else
    m_id = 1;

  m_parent = parent;
}

glowbot_object::glowbot_object(const quint64 id, QWidget *parent):
  QWidget(nullptr)
{
  m_editView = nullptr;
  m_id = id;
  m_initialized = false;
  m_parent = parent;
}

glowbot_object::~glowbot_object()
{
  qDebug() << "Done!";
}

QPointF glowbot_object::scenePos(void) const
{
  if(m_proxy)
    return m_proxy->scenePos();
  else
    return QPointF(0.0, 0.0);
}

QPointer<glowbot_proxy_widget> glowbot_object::proxy(void) const
{
  return m_proxy;
}

QString glowbot_object::type(void) const
{
  return m_type;
}

glowbot_object *glowbot_object::createFromValues
(const QMap<QString, QVariant> &values, QString &error, QWidget *parent)
{
  QString type(values.value("type").toString().toLower().trimmed());
  glowbot_object *object = nullptr;

  if(type == "arduino-analogread")
    object = glowbot_object_analog_read_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-function")
    object = glowbot_object_function_arduino::createFromValues
      (values, error, parent);
  else
    {
      if(type.isEmpty())
	error = tr("Empty object type.");
      else
	error = tr("The type %1 is not supported.").arg(type);
    }

  return object;
}

quint64 glowbot_object::id(void) const
{
  return m_id;
}

void glowbot_object::addDefaultActions(QMenu &menu) const
{
  if(!menu.actions().isEmpty())
    menu.addSeparator();

  menu.addAction(tr("&Delete"),
		 this,
		 SIGNAL(deletedViaContextMenu(void)))->
    setEnabled(!isMandatory());
  menu.addAction(tr("&Set Style Sheet..."),
		 this,
		 SLOT(slotSetStyleSheet(void)));
}

void glowbot_object::move(const QPoint &point)
{
  move(point.x(), point.y());
}

void glowbot_object::move(int x, int y)
{
  bool isChanged = false;

  if(pos().x() != x || pos().y() != y)
    isChanged = true;

  QWidget::move(x, y);

  if(isChanged)
    emit changed();
}

void glowbot_object::save(const QSqlDatabase &db, QString &error)
{
  QSqlQuery query(db);

  query.prepare("INSERT OR REPLACE INTO objects "
		"(myoid, parent_oid, position, stylesheet, type) "
		"VALUES(?, ?, ?, ?, ?)");
  query.addBindValue(m_id);

  if(qobject_cast<glowbot_object_view *> (m_parent))
    query.addBindValue(qobject_cast<glowbot_object_view *> (m_parent)->id());
  else
    query.addBindValue(-1);

  query.addBindValue
    (QString("(%1,%2)").arg(scenePos().x()).arg(scenePos().y()));
  query.addBindValue(styleSheet());
  query.addBindValue(m_type);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
}

void glowbot_object::setProxy(const QPointer<glowbot_proxy_widget> &proxy)
{
  m_proxy = proxy;
}

void glowbot_object::setUndoStack(QUndoStack *undoStack)
{
  if(m_editView)
    m_editView->scene()->setUndoStack(undoStack);
}

void glowbot_object::slotSetStyleSheet(void)
{
  QString string(styleSheet());
  glowbot_style_sheet *styleSheet = new glowbot_style_sheet(m_parent);

  styleSheet->setWidget(this);

  if(styleSheet->exec() == QDialog::Accepted)
    {
      setStyleSheet(styleSheet->styleSheet());
      emit changed();
    }
  else
    setStyleSheet(string);
}
