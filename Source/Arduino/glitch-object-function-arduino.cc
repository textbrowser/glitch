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

#include <QInputDialog>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QtDebug>

#include "glitch-object-edit-window.h"
#include "glitch-object-function-arduino.h"
#include "glitch-misc.h"
#include "glitch-object-view.h"
#include "glitch-view-arduino.h"

glitch_object_function_arduino::glitch_object_function_arduino
(QWidget *parent):glitch_object(parent)
{
  initialize(parent);

  if(parent)
    {
      if(!qobject_cast<glitch_view_arduino *> (parent))
	m_view = qobject_cast<glitch_view_arduino *> (parent->parent());
      else
	m_view = qobject_cast<glitch_view_arduino *> (parent);
    }
  else
    m_view = nullptr;

  /*
  ** Do not initialize the function's name in initialize().
  */

  QString name("");

  if(m_view)
    name = m_view->nextUniqueFunctionName();
  else
    name = "function_" +
      QUuid::createUuid().toString().remove("{").remove("}").remove("-") +
      "()";

  m_editWindow->setWindowTitle(tr("Glitch: %1").arg(name));
  m_ui.label->setText(name);

  if(m_view)
    m_view->consumeFunctionName(name);

  setObjectName(name);
}

glitch_object_function_arduino::glitch_object_function_arduino
(const QString &name, QWidget *parent):glitch_object(parent)
{
  m_editView = nullptr;
  m_editWindow = nullptr;
  m_initialized = true;
  m_isFunctionReference = true;
  m_type = "arduino-function";
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_ui.label->setText(name);
  m_view = nullptr;
  setObjectName(name);
}

glitch_object_function_arduino::glitch_object_function_arduino
(const qint64 parentId, const quint64 id, QWidget *parent):
  glitch_object(id, parent)
{
  if(parentId == -1)
    {
      /*
      ** parent is a glitch_view.
      */

      initialize(parent);
      m_view = qobject_cast<glitch_view_arduino *> (parent);
    }
  else
    {
      /*
      ** parent is a glitch_object_view.
      */

      m_editView = nullptr;
      m_editWindow = nullptr;
      m_initialized = true;
      m_isFunctionReference = true;
      m_type = "arduino-function";
      m_ui.setupUi(this);
      m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
      m_ui.label->setAutoFillBackground(true);
      m_view = nullptr;
    }
}

glitch_object_function_arduino::~glitch_object_function_arduino()
{
  if(m_editWindow)
    m_editWindow->deleteLater();
}

QString glitch_object_function_arduino::name(void) const
{
  return m_ui.label->text();
}

bool glitch_object_function_arduino::hasView(void) const
{
  return true;
}

bool glitch_object_function_arduino::isMandatory(void) const
{
  return false;
}

glitch_object_function_arduino *glitch_object_function_arduino::
clone(QWidget *parent) const
{
  return new glitch_object_function_arduino(parent);
}

glitch_object_function_arduino *glitch_object_function_arduino::
createFromValues
(const QMap<QString, QVariant> &values, QString &error, QWidget *parent)
{
  Q_UNUSED(error);

  glitch_object_function_arduino *object = new glitch_object_function_arduino
    (values.value("parentId").toLongLong(),
     values.value("myoid").toULongLong(),
     parent);

  object->setProperties(values.value("properties").toString());
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

glitch_object_view *glitch_object_function_arduino::editView(void) const
{
  return m_editView;
}

void glitch_object_function_arduino::addActions(QMenu &menu) const
{
  if(m_isFunctionReference)
    {
      /*
      ** This is a function reference.
      */

      addDefaultActions(menu);
      return;
    }

  menu.addAction(tr("&Edit..."),
		 this,
		 SLOT(slotEdit(void)));
  menu.addAction(tr("Set Function &Name..."),
		 this,
		 SLOT(slotSetFunctionName(void)));
  addDefaultActions(menu);
}

void glitch_object_function_arduino::addChild
(const QPointF &point, glitch_object *object)
{
  if(!object)
    return;

  if(m_editView)
    m_editView->artificialDrop(point, object);
}

void glitch_object_function_arduino::closeEditWindow(void)
{
  if(m_editWindow)
    m_editWindow->close();
}

void glitch_object_function_arduino::initialize(QWidget *parent)
{
  if(m_initialized)
    return;
  else
    m_initialized = true;

  m_editView = new glitch_object_view
    (glitch_common::ArduinoProject, m_id, this);
  m_editWindow = new glitch_object_edit_window(parent);
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  m_editWindow->resize(600, 600);
  m_isFunctionReference = false;
  m_type = "arduino-function";
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  connect(m_editView,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
  connect(m_editWindow,
	  SIGNAL(closed(void)),
	  m_editView,
	  SLOT(slotParentWindowClosed(void)));
}

void glitch_object_function_arduino::mouseDoubleClickEvent(QMouseEvent *event)
{
  slotEdit();
  QWidget::mouseDoubleClickEvent(event);
}

void glitch_object_function_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QSqlQuery query(db);

  query.prepare("UPDATE objects SET properties = ? WHERE myoid = ?");
  query.addBindValue(QString("name = \"%1\"").arg(m_ui.label->text()));
  query.addBindValue(m_id);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
  else if(m_editView)
    m_editView->save(db, error);
}

void glitch_object_function_arduino::setName(const QString &name)
{
  glitch_object::setName(name);
}

void glitch_object_function_arduino::setProperties(const QString &properties)
{
  QStringList list(properties.split("&"));

  for(int i = 0; i < list.size(); i++)
    if(list.at(i).startsWith("name = "))
      {
	QString str(list.at(i).mid(7));

	str.remove("\"");

	if(m_view && m_view->containsFunctionName(str))
	  str = m_view->nextUniqueFunctionName();

	setObjectName(str);

	if(m_editWindow)
	  m_editWindow->setWindowTitle(tr("Glitch: %1").arg(str));

	m_ui.label->setText(str);

	if(m_view)
	  m_view->consumeFunctionName(str);
      }
}

void glitch_object_function_arduino::slotEdit(void)
{
  if(m_editWindow)
    {
      m_editWindow->showNormal();
      m_editWindow->raise();
    }
}

void glitch_object_function_arduino::slotSetFunctionName(void)
{
  if(!m_editWindow)
    return;

  QInputDialog dialog(m_parent);

  dialog.setLabelText(tr("Set Function Name"));
  dialog.setTextEchoMode(QLineEdit::Normal); // A line edit!
  dialog.setTextValue(m_ui.label->text());
  dialog.setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  dialog.setWindowTitle(tr("Glitch: Set Function Name"));

  QLineEdit *lineEdit = dialog.findChild<QLineEdit *> ();

  if(lineEdit)
    {
      lineEdit->selectAll();
      lineEdit->setValidator

	/*
	** A mandatory letter followed by an optional word. Allow
	** trailing parentheses.
	*/

	(new QRegExpValidator(QRegExp("[A-Za-z][\\w]*\\(\\)"), &dialog));
    }
  else
    qDebug() << "glitch_object_function_arduino::slotSetFunctionName(): "
	     << "QInputDialog does not have a textfield! Cannot set "
	     << "an input validator.";

 restart_label:

  if(dialog.exec() == QDialog::Accepted)
    {
      QString text(dialog.textValue().remove("(").remove(")").trimmed());

      if(text.isEmpty())
	return;
      else
	text.append("()");

      if(m_ui.label->text() == text)
	return;

      if(m_view && m_view->containsFunctionName(text))
	{
	  glitch_misc::showErrorDialog
	    (tr("The function %1 is already defined. "
		"Please select another name.").arg(text), m_parent);
	  goto restart_label;
	}

      QString name(m_ui.label->text());

      if(m_view)
	m_view->removeFunctionName(m_ui.label->text());

      setObjectName(text);
      m_editWindow->setWindowTitle(tr("Glitch: %1").arg(text));
      m_ui.label->setText(text);

      if(m_view)
	m_view->consumeFunctionName(text);

      emit changed();
      emit nameChanged(name, text, this);
    }
}
