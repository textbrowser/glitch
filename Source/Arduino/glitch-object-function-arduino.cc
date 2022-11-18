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

#include "glitch-floating-context-menu.h"
#include "glitch-graphicsview.h"
#include "glitch-misc.h"
#include "glitch-object-edit-window.h"
#include "glitch-object-function-arduino.h"
#include "glitch-object-view.h"
#include "glitch-scene.h"
#include "glitch-scroll-filter.h"
#include "glitch-structures-arduino.h"
#include "glitch-view-arduino.h"

glitch_object_function_arduino::glitch_object_function_arduino
(QWidget *parent):glitch_object(parent)
{
  m_initialized = false;
  initialize(parent);
  m_parentView = qobject_cast<glitch_view_arduino *>
    (findNearestGlitchView(parent));

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

  if(m_editWindow)
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
	      &QTimer::timeout,
	      this,
	      &glitch_object_function_arduino::slotFindParentFunctionTimeout);
      m_findParentFunctionTimer.start(100);
    }

  m_editView = nullptr;
  m_editWindow = nullptr;
  m_initialized = true;
  m_isFunctionClone = true;
  m_parentView = nullptr;
  m_type = "arduino-function";
  m_ui.setupUi(this);
  m_ui.label->setText
    (name.mid(0, static_cast<int> (Limits::NAME_MAXIMUM_LENGTH)));
  m_ui.return_type->addItems
    (glitch_structures_arduino::nonArrayVariableTypes());
  m_ui.return_type->installEventFilter(new glitch_scroll_filter(this));
  m_ui.return_type->setEnabled(false);
  m_ui.return_type->setToolTip
    (tr("Return type is disabled on function clone."));
  prepareContextMenu();
}

glitch_object_function_arduino::glitch_object_function_arduino
(const qint64 parentId,
 const qint64 id,
 glitch_object *parentObject,
 QWidget *parent):glitch_object(id, parent)
{
  Q_UNUSED(parentObject);

  if(parentId == -1)
    {
      /*
      ** parent is a glitch_view.
      */

      m_initialized = false;
      initialize(parent);
      m_parentView = qobject_cast<glitch_view_arduino *> (parent);
    }
  else
    {
      m_ui.setupUi(this);

      /*
      ** parent is a glitch_object_view.
      */

      if(parent)
	{
	  connect
	    (&m_findParentFunctionTimer,
	     &QTimer::timeout,
	     this,
	     &glitch_object_function_arduino::slotFindParentFunctionTimeout);
	  m_findParentFunctionTimer.start(100);
	}

      m_editView = nullptr;
      m_editWindow = nullptr;
      m_initialized = true;
      m_isFunctionClone = true;
      m_parentView = nullptr;
      m_type = "arduino-function";
      m_ui.return_type->addItems
	(glitch_structures_arduino::nonArrayVariableTypes());
      m_ui.return_type->installEventFilter(new glitch_scroll_filter(this));
      m_ui.return_type->setEnabled(false);
      m_ui.return_type->setToolTip
	(tr("Return type is disabled on function clone."));
      prepareContextMenu();
    }
}

glitch_object_function_arduino::~glitch_object_function_arduino()
{
  if(m_parentFunction)
    disconnect(m_parentFunction, nullptr, this, nullptr);

  if(m_undoStack)
    disconnect(m_undoStack, nullptr, this, nullptr);
}

QPointer<glitch_object_function_arduino> glitch_object_function_arduino::
parentFunction(void) const
{
  return m_parentFunction;
}

QString glitch_object_function_arduino::code(void) const
{
  if(m_isFunctionClone)
    {
      QString code("");

      code.append(m_ui.label->text().remove("()"));
      code.append("(");

      auto list(inputs());

      for(int i = 0; i < list.size(); i++)
	{
	  code.append(list.at(i));

	  if(i != list.size() - 1)
	    code.append(", ");
	}

      code = code.trimmed();
      code.append(");");
      return code;
    }
  else
    {
      if(!m_editView)
	return "";

      QString code("");
      QTextStream stream(&code);

      stream << m_ui.return_type->currentText()
	     << " "
	     << m_ui.label->text().remove("()")
	     << "(";

      QString parameters("");
      auto list(inputs());

      for(int i = 0; i < list.size(); i++)
	{
	  parameters.append(list.at(i).trimmed());

	  if(i != list.size() - 1)
	    parameters.append(", ");
	}

      if(parameters.isEmpty())
	parameters = "void";

      stream << parameters
	     << ")"
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	     << endl
	     << "{"
	     << endl;
#else
	     << Qt::endl
	     << "{"
	     << Qt::endl;
#endif

      foreach(auto w, m_editView->scene()->orderedObjects())
	{
	  if(!w || !w->shouldPrint())
	    continue;

	  auto code(w->code());

	  if(!code.trimmed().isEmpty())
	    stream << glitch_common::s_indentationCharacter
		   << code
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
		   << endl;
#else
		   << Qt::endl;
#endif
	}

      stream << "}";
      return code;
    }
}

QString glitch_object_function_arduino::name(void) const
{
  return m_ui.label->text();
}

QString glitch_object_function_arduino::returnType(void) const
{
  return m_ui.return_type->currentText();
}

bool glitch_object_function_arduino::hasInput(void) const
{
  return true;
}

bool glitch_object_function_arduino::hasOutput(void) const
{
  return m_isFunctionClone && m_ui.return_type->currentText() != "void";
}

bool glitch_object_function_arduino::hasView(void) const
{
  return true;
}

bool glitch_object_function_arduino::isClone(void) const
{
  return m_isFunctionClone;
}

bool glitch_object_function_arduino::isFullyWired(void) const
{
  return false;
}

bool glitch_object_function_arduino::shouldPrint(void) const
{
  return m_ui.return_type->currentText() == "void" || outputs().isEmpty();
}

glitch_object_function_arduino *glitch_object_function_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_function_arduino(m_ui.label->text(), parent);

  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->resize(size());
  clone->setCanvasSettings(m_canvasSettings);
  clone->setReturnType(m_ui.return_type->currentText());
  clone->setStyleSheet(styleSheet());

  if(m_copiedChildren.isEmpty() && m_editView)
    /*
    ** First, copy!
    */

    foreach(auto object, m_editView->scene()->objects())
      {
	if(!object)
	  continue;

	auto child = object->clone(nullptr);

	if(child)
	  {
	    child->setProperty("position", object->scenePos());
	    clone->m_copiedChildren << child;
	  }
      }
  else if(!clone->findParentFunction())
    {
      /*
      ** Now, paste!
      */

      clone->m_editView = new glitch_object_view
	(glitch_common::ProjectTypes::ArduinoProject,
	 clone->m_id,
	 nullptr,
	 clone);
      clone->m_editView->scene()->setCanvasSettings(clone->m_canvasSettings);
      clone->m_editWindow = new glitch_object_edit_window
	(glitch_common::ProjectTypes::ArduinoProject, clone, parent);
      clone->m_editWindow->prepareToolBar
	(clone->m_editView->alignmentActions());
      clone->m_editWindow->setCentralWidget(clone->m_editView);
      clone->m_editWindow->setEditView(clone->m_editView);
      clone->m_editWindow->setWindowIcon(QIcon(":Logo/glitch-logo.png"));
      clone->m_editWindow->setWindowTitle(tr("Glitch: %1").arg(clone->name()));
      clone->m_editWindow->resize(800, 600);
      clone->m_isFunctionClone = false;
      clone->m_parentView = qobject_cast<glitch_view_arduino *>
	(clone->findNearestGlitchView(parent));
      clone->m_type = "arduino-function";
      clone->m_ui.return_type->addItems
	(glitch_structures_arduino::nonArrayVariableTypes());
      clone->m_ui.return_type->setEnabled(true);
      clone->m_ui.return_type->setToolTip("");
      connect(clone->m_editView,
	      &glitch_object_view::changed,
	      clone,
	      &glitch_object_function_arduino::changed);
      connect(clone->m_ui.return_type,
	      SIGNAL(currentIndexChanged(int)),
	      clone,
	      SLOT(slotReturnTypeChanged(void)));
      clone->m_previousReturnType = clone->m_ui.return_type->currentText();
      clone->prepareContextMenu();
      clone->prepareEditSignals(clone->findNearestGlitchView(parent));

      foreach(auto object, m_copiedChildren)
	{
	  if(!object)
	    continue;

	  auto child = object->clone(clone->m_editView);

	  if(child)
	    {
	      child->compressWidget
		(child->property(Properties::COMPRESSED_WIDGET).toBool());
	      child->setCanvasSettings(m_canvasSettings);
	      clone->addChild(object->property("position").toPoint(), child);
	    }
	}

      clone->cloneWires(m_copiedConnectionsPositions);
      clone->cloneWires(m_wires);
      clone->hideOrShowOccupied();
    }

  return clone;
}

glitch_object_function_arduino *glitch_object_function_arduino::
createFromValues
(const QMap<QString, QVariant> &values,
 glitch_object *parentObject,
 QString &error,
 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_function_arduino
    (values.value("parentId").toLongLong(),
     values.value("myoid").toLongLong(),
     parentObject,
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

  QGraphicsScene *scene = nullptr;

  if(qobject_cast<glitch_graphicsview *> (m_parent))
    scene = qobject_cast<glitch_graphicsview *> (m_parent)->scene();
  else if(qobject_cast<glitch_object_view *> (m_parent))
    {
      auto parent = qobject_cast<QObject *> (m_parent);

      do
	{
	  parent = parent->parent();

	  if(qobject_cast<glitch_view *> (parent))
	    {
	      scene = qobject_cast<glitch_view *> (parent)->scene();
	      break;
	    }
	}
      while(parent);
    }
  else if(qobject_cast<glitch_view *> (m_parent))
    scene = qobject_cast<glitch_view *> (m_parent)->scene();

  if(!scene)
    return nullptr;

  auto list(scene->items());

  foreach(auto i, list)
    {
      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy)
	continue;

      auto object = qobject_cast<glitch_object_function_arduino *>
	(proxy->widget());

      if(!object || object->isClone())
	continue;

      if(name() == object->name())
	return object;
    }

  return nullptr;
}

void glitch_object_function_arduino::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::EDIT))
    {
      auto action = new QAction(tr("&Edit..."), this);

      action->setIcon(QIcon::fromTheme("document-edit"));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_function_arduino::slotEdit);
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
      auto action = new QAction(tr("Set Function &Name..."), this);

      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_function_arduino::slotSetFunctionName);
      m_actions[DefaultMenuActions::SET_FUNCTION_NAME] = action;
      menu.addAction(action);
    }
  else
    menu.addAction(m_actions.value(DefaultMenuActions::SET_FUNCTION_NAME));

  addDefaultActions(menu);
}

void glitch_object_function_arduino::compressWidget(const bool state)
{
  glitch_object::compressWidget(state);
  m_ui.return_type->setVisible(!state);
}

void glitch_object_function_arduino::hideOrShowOccupied(void)
{
  auto scene = editScene();

  if(!scene)
    return;

  m_occupied = !scene->objects().isEmpty();

  auto font(m_ui.label->font());

  font.setUnderline(m_occupied);
  m_ui.label->setFont(font);
}

void glitch_object_function_arduino::initialize(QWidget *parent)
{
  if(m_initialized)
    return;
  else
    m_initialized = true;

  m_ui.setupUi(this);
  m_editView = new glitch_object_view
    (glitch_common::ProjectTypes::ArduinoProject,
     m_id,
     m_undoStack,
     this);
  m_editView->scene()->setCanvasSettings(m_canvasSettings);
  m_editWindow = new glitch_object_edit_window
    (glitch_common::ProjectTypes::ArduinoProject, this, parent);
  m_editWindow->prepareToolBar(m_editView->alignmentActions());
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setEditView(m_editView);
  m_editWindow->setUndoStack(m_undoStack);
  m_editWindow->setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  m_editWindow->setWindowTitle
    (tr("Glitch: %1").arg(glitch_object_function_arduino::name()));
  m_editWindow->resize(800, 600);
  m_isFunctionClone = false;
  m_type = "arduino-function";
  m_ui.return_type->addItems
    (glitch_structures_arduino::nonArrayVariableTypes());
  m_ui.return_type->installEventFilter(new glitch_scroll_filter(this));
  connect(m_editView,
	  &glitch_object_view::changed,
	  this,
	  &glitch_object_function_arduino::changed,
	  Qt::UniqueConnection);
  connect(m_ui.return_type,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotReturnTypeChanged(void)),
	  Qt::UniqueConnection);
  m_previousReturnType = m_ui.return_type->currentText();
  prepareContextMenu();
  prepareEditSignals(findNearestGlitchView(parent));
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
  glitch_object::setName(name);

  if(m_editWindow)
    m_editWindow->setWindowTitle
      (tr("Glitch: %1").arg(m_properties.value(Properties::NAME).toString()));

  if(m_parentView)
    {
      m_parentView->consumeFunctionName(name);
      m_parentView->removeFunctionName
	(m_properties.value(Properties::NAME).toString());
    }

  m_ui.label->setText(m_properties.value(Properties::NAME).toString());

  if(!m_isFunctionClone)
    emit changed();
}

void glitch_object_function_arduino::setProperties(const QString &properties)
{
  auto list(properties.split("&"));

  std::sort(list.begin(), list.end());
  glitch_object::setProperties(list);

  for(int i = 0; i < list.size(); i++)
    if(list.at(i).startsWith("clone = "))
      {
	auto string(list.at(i).mid(8));

	string.remove("\"");

	if((m_isFunctionClone = QVariant(string).toBool()))
	  {
	    connect
	      (&m_findParentFunctionTimer,
	       &QTimer::timeout,
	       this,
	       &glitch_object_function_arduino::slotFindParentFunctionTimeout,
	       Qt::UniqueConnection);
	    m_findParentFunctionTimer.start(100);
	  }

	m_ui.return_type->setEnabled(!m_isFunctionClone);

	if(!m_ui.return_type->isEnabled())
	  m_ui.return_type->setToolTip
	    (tr("Return type is disabled on function clone."));
	else
	  m_ui.return_type->setToolTip("");
      }
    else if(list.at(i).startsWith("name = "))
      {
	auto string(list.at(i).mid(7));

	string.remove("\"");
	string = string.mid
	  (0, static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));

	if(!m_isFunctionClone &&
	   m_parentView &&
	   m_parentView->containsFunctionName(string))
	  string = m_parentView->nextUniqueFunctionName();

	if(m_editWindow)
	  m_editWindow->setWindowTitle(tr("Glitch: %1").arg(string));

	if(!m_isFunctionClone && m_parentView)
	  m_parentView->consumeFunctionName(string);

	m_ui.label->setText(string);
      }
    else if(list.at(i).startsWith("return_type = "))
      {
	if(!m_isFunctionClone)
	  {
	    auto string(list.at(i).mid(14));

	    string.remove("\"");
	    setReturnType(string);
	    m_previousReturnType = m_ui.return_type->currentText();
	  }
	else
	  slotParentFunctionChanged();
      }
}

void glitch_object_function_arduino::setReturnType(const QString &returnType)
{
  auto index = m_ui.return_type->findText(returnType);

  m_ui.return_type->blockSignals(true);

  if(index >= 0)
    m_ui.return_type->setCurrentIndex(index);
  else
    m_ui.return_type->setCurrentIndex(0);

  m_ui.return_type->blockSignals(false);

  if(!m_isFunctionClone)
    emit changed();
}

void glitch_object_function_arduino::simulateDelete(void)
{
  glitch_object::simulateDelete();
}

void glitch_object_function_arduino::slotEdit(void)
{
  if(m_editWindow && !m_isFunctionClone)
    {
      m_editWindow->setToolBarVisible
	(m_properties.value(Properties::TOOL_BAR_VISIBLE).toBool());
      m_editWindow->showNormal();
      m_editWindow->activateWindow();
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
	      &glitch_object_function_arduino::changed,
	      this,
	      &glitch_object_function_arduino::slotParentFunctionChanged,
	      Qt::UniqueConnection);
      m_findParentFunctionTimer.stop();
      m_undoStack = m_parentFunction->m_undoStack;
      slotParentFunctionChanged();
    }
}

void glitch_object_function_arduino::slotHideOrShowOccupied(void)
{
  hideOrShowOccupied();
}

void glitch_object_function_arduino::slotParentFunctionChanged(void)
{
  if(!m_parentFunction)
    return;

  glitch_object_function_arduino::setName(m_parentFunction->name());
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

  auto lineEdit = dialog.findChild<QLineEdit *> ();

  if(lineEdit)
    {
      lineEdit->selectAll();
      lineEdit->setMaxLength(static_cast<int> (Limits::NAME_MAXIMUM_LENGTH));
      lineEdit->setValidator

	/*
	** A mandatory letter or underscore followed by an optional word. Allow
	** trailing parentheses.
	*/

	(new QRegularExpressionValidator
	 (QRegularExpression("[A-Za-z_][\\w]*\\(\\)"), &dialog));
    }
  else
    qDebug() << tr("glitch_object_function_arduino::slotSetFunctionName(): "
		   "QInputDialog does not have a textfield! Cannot set "
		   "an input validator.");

 restart_label:
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      auto text(dialog.textValue().remove("(").remove(")").trimmed());

      if(text.isEmpty())
	return;
      else
	text += "()";

      if(m_ui.label->text() == text)
	return;

      if(m_parentView && m_parentView->containsFunctionName(text))
	{
	  glitch_misc::showErrorDialog
	    (tr("The function %1 is already defined. "
		"Please select another name.").arg(text), m_parent);
	  goto restart_label;
	}

      auto name(m_ui.label->text());

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
