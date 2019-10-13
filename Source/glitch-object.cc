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

#include <QSqlError>
#include <QSqlQuery>
#include <QUndoStack>
#include <QtDebug>

#include "Arduino/glitch-object-analog-read-arduino.h"
#include "Arduino/glitch-object-function-arduino.h"
#include "glitch-object-view.h"
#include "glitch-object.h"
#include "glitch-scene.h"
#include "glitch-style-sheet.h"
#include "glitch-view.h"

glitch_object::glitch_object(QWidget *parent):
  QWidget(nullptr)
{
  m_initialized = false;
  m_positionLocked = false;

  QWidget *p = parent;
  glitch_view *view = nullptr;

  do
    {
      if(!p)
	break;

      view = qobject_cast<glitch_view *> (p);

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

glitch_object::glitch_object(const quint64 id, QWidget *parent):
  QWidget(nullptr)
{
  m_editView = nullptr;
  m_id = id;
  m_initialized = false;
  m_parent = parent;
  m_positionLocked = false;
}

glitch_object::~glitch_object()
{
  qDebug() << "Done!";
}

QPointF glitch_object::scenePos(void) const
{
  if(m_proxy)
    return m_proxy->scenePos();
  else
    return QPointF(0.0, 0.0);
}

QPointer<glitch_proxy_widget> glitch_object::proxy(void) const
{
  return m_proxy;
}

QString glitch_object::name(void) const
{
  return m_name;
}

QString glitch_object::type(void) const
{
  return m_type;
}

bool glitch_object::positionLocked(void) const
{
  return m_positionLocked;
}

glitch_object *glitch_object::createFromValues
(const QMap<QString, QVariant> &values, QString &error, QWidget *parent)
{
  QString type(values.value("type").toString().toLower().trimmed());
  glitch_object *object = nullptr;

  if(type == "arduino-analogread")
    object = glitch_object_analog_read_arduino::createFromValues
      (values, error, parent);
  else if(type == "arduino-function")
    object = glitch_object_function_arduino::createFromValues
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

glitch_object_view *glitch_object::editView(void) const
{
  return nullptr;
}

quint64 glitch_object::id(void) const
{
  return m_id;
}

void glitch_object::addChild(const QPointF &point, glitch_object *object)
{
  Q_UNUSED(object);
  Q_UNUSED(point);
}

void glitch_object::addDefaultActions(QMenu &menu) const
{
  if(!menu.actions().isEmpty())
    menu.addSeparator();

  QAction *action = nullptr;

  menu.addAction(tr("&Delete"),
		 this,
		 SIGNAL(deletedViaContextMenu(void)))->
    setEnabled(!isMandatory());
  action = menu.addAction(tr("&Lock Position"),
			  this,
			  SLOT(slotLockPosition(void)));
  action->setCheckable(true);
  action->setChecked(m_positionLocked);
  action->setEnabled(!isMandatory());
  menu.addAction(tr("&Set Style Sheet..."),
		 this,
		 SLOT(slotSetStyleSheet(void)));
}

void glitch_object::closeEditWindow(void)
{
}

void glitch_object::move(const QPoint &point)
{
  move(point.x(), point.y());
}

void glitch_object::move(int x, int y)
{
  if(m_positionLocked)
    return;

  bool isChanged = false;

  if(pos().x() != x || pos().y() != y)
    isChanged = true;

  QWidget::move(x, y);

  if(isChanged)
    emit changed();
}

void glitch_object::save(const QSqlDatabase &db, QString &error)
{
  QSqlQuery query(db);

  query.prepare("INSERT OR REPLACE INTO objects "
		"(myoid, parent_oid, position, stylesheet, type) "
		"VALUES(?, ?, ?, ?, ?)");
  query.addBindValue(m_id);

  if(qobject_cast<glitch_object_view *> (m_parent))
    query.addBindValue(qobject_cast<glitch_object_view *> (m_parent)->id());
  else
    query.addBindValue(-1);

  query.addBindValue(QString("(%1,%2)").arg(pos().x()).arg(pos().y()));
  query.addBindValue(styleSheet());
  query.addBindValue(m_type);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
}

void glitch_object::saveProperties(const QMap<QString, QVariant> &properties,
				   const QSqlDatabase &db,
				   QString &error)
{
  QMapIterator<QString, QVariant> it(properties);
  QSqlQuery query(db);
  QString string("");

  while(it.hasNext())
    {
      it.next();
      string.append(it.key());
      string.append(" = ");
      string.append("\"");
      string.append(it.value().toString());
      string.append("\"");

      if(it.hasNext())
	string.append("&");
    }

  query.prepare("UPDATE objects SET properties = ? WHERE myoid = ?");
  query.addBindValue(string);
  query.addBindValue(m_id);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
}

void glitch_object::setName(const QString &name)
{
  if(!name.trimmed().isEmpty())
    m_name = name.trimmed();
}

void glitch_object::setProxy(const QPointer<glitch_proxy_widget> &proxy)
{
  m_proxy = proxy;
}

void glitch_object::setUndoStack(QUndoStack *undoStack)
{
  if(m_editView)
    m_editView->scene()->setUndoStack(undoStack);
}

void glitch_object::slotLockPosition(void)
{
  m_positionLocked = !m_positionLocked;

  if(m_proxy)
    m_proxy->setFlag(QGraphicsItem::ItemIsMovable, !m_positionLocked);

  emit changed();
}

void glitch_object::slotSetStyleSheet(void)
{
  QString string(styleSheet());
  glitch_style_sheet *styleSheet = new glitch_style_sheet(m_parent);

  styleSheet->setWidget(this);
  QApplication::processEvents();

  if(styleSheet->exec() == QDialog::Accepted)
    {
      setStyleSheet(styleSheet->styleSheet());
      emit changed();
    }
  else
    setStyleSheet(string);
}
