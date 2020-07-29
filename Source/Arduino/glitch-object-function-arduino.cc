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
#include <QUuid>
#include <QtDebug>

#include "glitch-floating-context-menu.h"
#include "glitch-object-function-arduino.h"
#include "glitch-misc.h"
#include "glitch-object-edit-window.h"
#include "glitch-object-view.h"
#include "glitch-scene.h"
#include "glitch-structures-arduino.h"
#include "glitch-view-arduino.h"

glitch_object_function_arduino::glitch_object_function_arduino
(QWidget *parent):glitch_object(parent)
{
  initialize(parent);

  if(parent)
    {
      if(!qobject_cast<glitch_view_arduino *> (parent))
	m_parentView = qobject_cast<glitch_view_arduino *> (parent->parent());
      else
	m_parentView = qobject_cast<glitch_view_arduino *> (parent);
    }
  else
    m_parentView = nullptr;

  /*
  ** Do not initialize the function's name in initialize().
  */

  QString name("");

  if(m_parentView)
    name = m_parentView->nextUniqueFunctionName();
  else
    name = "function_" +
      QUuid::createUuid().toString().remove("{").remove("}").remove("-") +
      "()";

  m_editWindow->setWindowTitle(tr("Glitch: %1").arg(name));

  if(m_parentView)
    m_parentView->consumeFunctionName(name);

  m_ui.label->setText(name);
}

glitch_object_function_arduino::glitch_object_function_arduino
(const QString &name, QWidget *parent):glitch_object(parent)
{
  /*
  ** Clone.
  */

  if(parent)
    {
      connect(&m_findParentFunctionTimer,
	      SIGNAL(timeout(void)),
	      this,
	      SLOT(slotFindParentFunctionTimeout(void)));
      m_findParentFunctionTimer.start(100);
    }

  m_editView = nullptr;
  m_editWindow = nullptr;
  m_initialized = true;
  m_isFunctionClone = true;
  m_parentView = nullptr;
  m_type = "arduino-function";
  m_ui.setupUi(this);
  m_ui.function_definition->setVisible(false);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_ui.label->setText(name);
  m_ui.return_type->addItems(glitch_structures_arduino::nonArrayTypes());
  m_ui.return_type->setEnabled(false);
  m_ui.return_type->setToolTip
    (tr("Return type is disabled on function clone."));
  prepareContextMenu();
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
      m_parentView = qobject_cast<glitch_view_arduino *> (parent);
    }
  else
    {
      /*
      ** parent is a glitch_object_view.
      */

      if(parent)
	{
	  connect(&m_findParentFunctionTimer,
		  SIGNAL(timeout(void)),
		  this,
		  SLOT(slotFindParentFunctionTimeout(void)));
	  m_findParentFunctionTimer.start(100);
	}

      m_editView = nullptr;
      m_editWindow = nullptr;
      m_initialized = true;
      m_isFunctionClone = true;
      m_parentView = nullptr;
      m_type = "arduino-function";
      m_ui.setupUi(this);
      m_ui.function_definition->setVisible(false);
      m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
      m_ui.label->setAutoFillBackground(true);
      m_ui.return_type->addItems(glitch_structures_arduino::nonArrayTypes());
      m_ui.return_type->setEnabled(false);
      m_ui.return_type->setToolTip
	(tr("Return type is disabled on function clone."));
      prepareContextMenu();
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

QString glitch_object_function_arduino::returnType(void) const
{
  return m_ui.return_type->currentText();
}

bool glitch_object_function_arduino::hasView(void) const
{
  return true;
}

bool glitch_object_function_arduino::isClone(void) const
{
  return m_isFunctionClone;
}

bool glitch_object_function_arduino::isMandatory(void) const
{
  return false;
}

glitch_object_function_arduino *glitch_object_function_arduino::
clone(QWidget *parent) const
{
  auto *clone = new glitch_object_function_arduino(m_ui.label->text(), parent);

  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_function_arduino *glitch_object_function_arduino::
createFromValues
(const QMap<QString, QVariant> &values, QString &error, QWidget *parent)
{
  Q_UNUSED(error);

  auto *object = new glitch_object_function_arduino
    (values.value("parentId").toLongLong(),
     values.value("myoid").toULongLong(),
     parent);

  object->setProperties(values.value("properties").toString());
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

glitch_object_function_arduino *glitch_object_function_arduino::
findParentFunction(void) const
{
  if(m_parentFunction)
    return m_parentFunction;

  if(qobject_cast<glitch_view *> (m_parent))
    {
      QList<QGraphicsItem *> list;
      auto *scene = qobject_cast<glitch_view *> (m_parent)->scene();

      if(scene)
	list = scene->items();

      for(auto i : list)
	{
	  auto *proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

	  if(!proxy)
	    continue;

	  auto *object = qobject_cast<glitch_object_function_arduino *>
	    (proxy->widget());

	  if(!object || object->isClone())
	    continue;

	  if(name() == object->name())
	    return object;
	}
    }

  return nullptr;
}

glitch_object_view *glitch_object_function_arduino::editView(void) const
{
  return m_editView;
}

void glitch_object_function_arduino::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::EDIT))
    {
      auto *action = new QAction(tr("&Edit..."), this);

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotEdit(void)));
      m_actions[DefaultMenuActions::EDIT] = action;
      menu.addAction(action);
    }
  else
    menu.addAction(m_actions.value(DefaultMenuActions::EDIT));

  if(m_isFunctionClone)
    {
      addDefaultActions(menu);
      return;
    }

  if(!m_actions.contains(DefaultMenuActions::SET_FUNCTION_NAME))
    {
      auto *action = new QAction(tr("Set Function &Name..."), this);

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotSetFunctionName(void)));
      m_actions[DefaultMenuActions::SET_FUNCTION_NAME] = action;
      menu.addAction(action);
    }
  else
    menu.addAction(m_actions.value(DefaultMenuActions::SET_FUNCTION_NAME));

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
  m_isFunctionClone = false;
  m_type = "arduino-function";
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_ui.return_type->addItems(glitch_structures_arduino::nonArrayTypes());
  connect(m_editView,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
  connect(m_editWindow,
	  SIGNAL(closed(void)),
	  m_editView,
	  SLOT(slotParentWindowClosed(void)));
  connect(m_editWindow,
	  SIGNAL(selectAll(void)),
	  m_editView,
	  SLOT(slotSelectAll(void)));
  connect(m_ui.return_type,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotReturnTypeChanged(void)));
  m_previousReturnType = m_ui.return_type->currentText();
  prepareContextMenu();
  prepareEditSignals();
}

void glitch_object_function_arduino::mouseDoubleClickEvent(QMouseEvent *event)
{
  slotEdit();
  QWidget::mouseDoubleClickEvent(event);
}

void glitch_object_function_arduino::prepareEditSignals(void)
{
  if(m_editView && m_editWindow && m_parentView)
    {
      connect(m_editView,
	      SIGNAL(copy(void)),
	      m_parentView,
	      SLOT(slotCopy(void)));
      connect(m_editView,
	      SIGNAL(paste(void)),
	      m_parentView,
	      SLOT(slotPaste(void)));
      connect(m_editWindow,
	      SIGNAL(copy(void)),
	      m_editView,
	      SIGNAL(copy(void)));
      connect(m_editWindow,
	      SIGNAL(paste(void)),
	      m_editView,
	      SIGNAL(paste(void)));
    }
}

void glitch_object_function_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["clone"] = m_isFunctionClone;
  properties["name"] = m_ui.label->text().trimmed();
  properties["return_type"] = m_ui.return_type->currentText();
  glitch_object::saveProperties(properties, db, error);

  if(error.isEmpty() && m_editView)
    m_editView->save(db, error);
}

void glitch_object_function_arduino::setName(const QString &name)
{
  if(m_parentView)
    {
      m_parentView->consumeFunctionName(name);
      m_parentView->removeFunctionName(m_name);
    }

  glitch_object::setName(name);

  if(m_editWindow)
    m_editWindow->setWindowTitle(tr("Glitch: %1").arg(m_name));

  m_ui.label->setText(m_name);

  if(!m_isFunctionClone)
    emit changed();
}

void glitch_object_function_arduino::setProperties(const QString &properties)
{
  QStringList list(properties.split("&"));

  std::sort(list.begin(), list.end());
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    if(list.at(i).startsWith("clone = "))
      {
	QString str(list.at(i).mid(8));

	str.remove("\"");

	if((m_isFunctionClone = QVariant(str).toBool()))
	  {
	    connect(&m_findParentFunctionTimer,
		    SIGNAL(timeout(void)),
		    this,
		    SLOT(slotFindParentFunctionTimeout(void)),
		    Qt::UniqueConnection);
	    m_findParentFunctionTimer.start(100);
	  }

	m_ui.function_definition->setVisible(!m_isFunctionClone);
	m_ui.return_type->setEnabled(!m_isFunctionClone);

	if(!m_ui.return_type->isEnabled())
	  m_ui.return_type->setToolTip
	    (tr("Return type is disabled on function clone."));
	else
	  m_ui.return_type->setToolTip("");
      }
    else if(list.at(i).startsWith("name = "))
      {
	QString str(list.at(i).mid(7));

	str.remove("\"");

	if(!m_isFunctionClone &&
	   m_parentView &&
	   m_parentView->containsFunctionName(str))
	  str = m_parentView->nextUniqueFunctionName();

	if(m_editWindow)
	  m_editWindow->setWindowTitle(tr("Glitch: %1").arg(str));

	if(!m_isFunctionClone && m_parentView)
	  m_parentView->consumeFunctionName(str);

	m_ui.label->setText(str);
      }
    else if(list.at(i).startsWith("return_type = "))
      {
	if(!m_isFunctionClone)
	  {
	    QString str(list.at(i).mid(14));

	    str.remove("\"");
	    setReturnType(str);
	    m_previousReturnType = m_ui.return_type->currentText();
	  }
	else
	  slotParentFunctionChanged();
      }
}

void glitch_object_function_arduino::setReturnType(const QString &returnType)
{
  int index = m_ui.return_type->findText(returnType);

  m_ui.return_type->blockSignals(true);

  if(index >= 0)
    m_ui.return_type->setCurrentIndex(index);
  else
    m_ui.return_type->setCurrentIndex(0);

  m_ui.return_type->blockSignals(false);

  if(!m_isFunctionClone)
    emit changed();
}

void glitch_object_function_arduino::slotEdit(void)
{
  if(m_editWindow && !m_isFunctionClone)
    {
      m_editWindow->showNormal();
      m_editWindow->raise();
      return;
    }

  if(m_parentFunction)
    m_parentFunction->slotEdit();
}

void glitch_object_function_arduino::slotFindParentFunctionTimeout(void)
{
  if(!m_isFunctionClone || m_parentFunction)
    {
      m_findParentFunctionTimer.stop();
      return;
    }

  m_parentFunction = findParentFunction();

  if(m_parentFunction)
    {
      connect(m_parentFunction,
	      SIGNAL(changed(void)),
	      this,
	      SLOT(slotParentFunctionChanged(void)),
	      Qt::UniqueConnection);
      m_findParentFunctionTimer.stop();
      slotParentFunctionChanged();
    }
}

void glitch_object_function_arduino::slotParentFunctionChanged(void)
{
  if(!m_parentFunction)
    return;

  setName(m_parentFunction->name());
  setReturnType(m_parentFunction->returnType());
}

void glitch_object_function_arduino::slotReturnTypeChanged(void)
{
  if(m_isFunctionClone)
    return;

  emit changed();
  emit returnTypeChanged
    (m_ui.return_type->currentText(), m_previousReturnType, this);
  m_previousReturnType = m_ui.return_type->currentText();
}

void glitch_object_function_arduino::slotSetFunctionName(void)
{
  if(!m_editWindow || m_isFunctionClone)
    return;

  QInputDialog dialog(m_parent);

  dialog.setLabelText(tr("Set Function Name"));
  dialog.setTextEchoMode(QLineEdit::Normal); // A line edit!
  dialog.setTextValue(m_ui.label->text());
  dialog.setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  dialog.setWindowTitle(tr("Glitch: Set Function Name"));

  auto *lineEdit = dialog.findChild<QLineEdit *> ();

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
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QString text(dialog.textValue().remove("(").remove(")").trimmed());

      if(text.isEmpty())
	return;
      else
	text.append("()");

      if(m_ui.label->text() == text)
	return;

      if(m_parentView && m_parentView->containsFunctionName(text))
	{
	  glitch_misc::showErrorDialog
	    (tr("The function %1 is already defined. "
		"Please select another name.").arg(text), m_parent);
	  goto restart_label;
	}

      QString name(m_ui.label->text());

      if(m_parentView)
	m_parentView->removeFunctionName(m_ui.label->text());

      m_editWindow->setWindowTitle(tr("Glitch: %1").arg(text));

      if(m_parentView)
	m_parentView->consumeFunctionName(text);

      m_ui.label->setText(text);
      emit changed();
      emit nameChanged(text, name, this);
    }
}
