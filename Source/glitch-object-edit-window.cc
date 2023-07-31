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

#include <QLineEdit>
#include <QMenuBar>
#include <QResizeEvent>
#include <QSettings>
#include <QSplitter>
#include <QTimer>
#include <QToolButton>
#include <QtDebug>

#include "Arduino/glitch-structures-arduino.h"
#include "glitch-docked-container.h"
#include "glitch-floating-context-menu.h"
#include "glitch-misc.h"
#include "glitch-object-edit-window.h"
#include "glitch-object-view.h"
#include "glitch-scene.h"
#include "glitch-ui.h"
#include "glitch-user-functions.h"

glitch_object_edit_window::glitch_object_edit_window
(const glitch_common::ProjectTypes projectType,
 glitch_object *object,
 QWidget *parent):QMainWindow(parent)
{
  Q_UNUSED(statusBar());

  auto menu = menuBar()->addMenu(tr("&File"));

#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
  m_actions["save"] =
    menu->addAction
    (tr("&Save Diagram"), tr("Ctrl+S"), this, SIGNAL(saveSignal(void)));
#else
  m_actions["save"] =
    menu->addAction
    (tr("&Save Diagram"), this, SIGNAL(saveSignal(void)), tr("Ctrl+S"));
#endif
  menu->addSeparator();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
#ifdef Q_OS_ANDROID
  m_actions["close"] =
    menu->addAction(tr("&Close"), this, SLOT(hide(void)));
#else
  m_actions["close"] =
    menu->addAction(tr("&Close"), tr("Ctrl+W"), this, SLOT(close(void)));
#endif
#else
#ifdef Q_OS_ANDROID
  m_actions["close"] =
    menu->addAction(tr("&Close"), this, SLOT(hide(void)));
#else
  m_actions["close"] =
    menu->addAction(tr("&Close"), this, SLOT(close(void)), tr("Ctrl+W"));
#endif
#endif
  menu = menuBar()->addMenu(tr("&Edit"));
  connect(menu,
	  &QMenu::aboutToShow,
	  this,
	  &glitch_object_edit_window::slotAboutToShowEditMenu);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
  m_actions["undo"] =
    menu->addAction(tr("Undo"), tr("Ctrl+Z"), this, SIGNAL(undo(void)));
  m_actions["redo"] =
    menu->addAction(tr("Redo"), tr("Ctrl+Shift+Z"), this, SIGNAL(redo(void)));
#else
  m_actions["undo"] =
    menu->addAction(tr("Undo"), this, SIGNAL(undo(void)), tr("Ctrl+Z"));
  m_actions["redo"] =
    menu->addAction(tr("Redo"), this, SIGNAL(redo(void)), tr("Ctrl+Shift+Z"));
#endif
  menu->addSeparator();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
  m_actions["copy"] =
    menu->addAction(tr("&Copy"), tr("Ctrl+C"), this, SIGNAL(copy(void)));
  m_actions["paste"] =
    menu->addAction(tr("Paste"), tr("Ctrl+V"), this, SIGNAL(paste(void)));
#else
  m_actions["copy"] =
    menu->addAction(tr("&Copy"), this, SIGNAL(copy(void)), tr("Ctrl+C"));
  m_actions["paste"] =
    menu->addAction(tr("Paste"), this, SIGNAL(paste(void)), tr("Ctrl+V"));
#endif
  menu->addSeparator();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
  m_actions["delete"] =
    menu->addAction(tr("&Delete"), tr("Del"), this, SIGNAL(deleteSignal(void)));
  m_actions["select all"] = menu->addAction
    (tr("Select &All"), tr("Ctrl+A"), this, SIGNAL(selectAll(void)));
#else
  m_actions["delete"] =
    menu->addAction(tr("&Delete"), this, SIGNAL(deleteSignal(void)), tr("Del"));
  m_actions["select all"] = menu->addAction
    (tr("Select &All"), this, SIGNAL(selectAll(void)), tr("Ctrl+A"));
#endif
  menu = menuBar()->addMenu(tr("&View"));
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
  m_actions["screen mode"] = menu->addAction
    (tr("&Full Screen"),
     tr("F11"),
     this,
     SLOT(slotShowFullScreenMode(void)));
#else
  m_actions["screen mode"] = menu->addAction
    (tr("&Full Screen"),
     this,
     SLOT(slotShowFullScreenMode(void)),
     tr("F11"));
#endif
  menu->addSeparator();
  m_actions["tools"] = menu->addAction(tr("&Tools Tool Bar"));
  m_actions["tools"]->setCheckable(true);
  m_actions["tools"]->setChecked(true);
  connect(m_actions.value("copy"),
	  &QAction::triggered,
	  this,
	  &glitch_object_edit_window::slotAboutToShowEditMenu);
  connect(m_actions.value("tools"),
	  &QAction::triggered,
	  this,
	  &glitch_object_edit_window::slotViewTools);
  m_dockedWidgetPropertyEditors = new glitch_docked_container(this);
  m_dockedWidgetPropertyEditors->resize
    (m_dockedWidgetPropertyEditors->sizeHint());
  m_dockedWidgetPropertyEditors->setMinimumWidth(250);
  m_editToolBar = new QToolBar(tr("Edit Tool Bar"), this);
  m_editToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_editToolBar->setIconSize(QSize(24, 24));
  m_editToolBar->setObjectName("edit_tool_bar");
  m_editToolBar->setVisible(true);
  m_fileToolBar = new QToolBar(tr("File Tool Bar"), this);
  m_fileToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_fileToolBar->setIconSize(QSize(24, 24));
  m_fileToolBar->setObjectName("file_tool_bar");
  m_fileToolBar->setVisible(true);
  m_header = new QLineEdit(this);
  m_header->setReadOnly(true);
  m_header->setVisible(false);
  m_leftSplitter = nullptr;
  m_miscellaneousToolBar = new QToolBar(tr("Miscellaneous Tool Bar"), this);
  m_miscellaneousToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_miscellaneousToolBar->setIconSize(QSize(24, 24));
  m_miscellaneousToolBar->setObjectName("miscellaneous_tool_bar");
  m_miscellaneousToolBar->setVisible(true);
  m_object = object;
  m_projectType = projectType;
  m_splitter = new QSplitter(this);
  m_toolsToolBar = new QToolBar(tr("Tools Tool Bar"), this);
  m_toolsToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_toolsToolBar->setIconSize(QSize(24, 24));
  m_toolsToolBar->setObjectName("tools_tool_bar");
  m_toolsToolBar->setVisible(true);
  m_userFunctions = new glitch_user_functions(this);
  m_userFunctions->setProjectType(m_projectType);
  addToolBar(m_fileToolBar);
  addToolBar(m_editToolBar);
  addToolBar(m_toolsToolBar);
  addToolBar(m_miscellaneousToolBar);
  connect(m_splitter,
	  SIGNAL(splitterMoved(int, int)),
	  this,
	  SLOT(slotSplitterMoved(void)));
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);
  prepareIcons();
  resize(800, 600);
}

QWidget *glitch_object_edit_window::centralWidget(void) const
{
  auto frame = qobject_cast<QFrame *> (QMainWindow::centralWidget());

  if(!frame || !frame->layout())
    return nullptr;

  /*
  ** Discover the glitch_object_view child.
  */

  for(int i = 0; i < frame->layout()->count(); i++)
    if(frame->layout()->itemAt(i) &&
       qobject_cast<glitch_object_view *> (frame->layout()->itemAt(i)->
					   widget()))
      return frame->layout()->itemAt(i)->widget();

  return nullptr;
}

glitch_object_edit_window::~glitch_object_edit_window()
{
  if(m_editView)
    disconnect(m_editView->scene(), nullptr, this, nullptr);

  if(m_undoStack)
    disconnect(m_undoStack, nullptr, this, nullptr);
}

bool glitch_object_edit_window::event(QEvent *event)
{
  if(event && event->type() == QEvent::Show)
    QTimer::singleShot
      (1500, this, &glitch_object_edit_window::slotAboutToShowEditMenu);

  return QMainWindow::event(event);
}

void glitch_object_edit_window::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
  emit closed();
}

void glitch_object_edit_window::hideEvent(QHideEvent *event)
{
  QMainWindow::hideEvent(event);

  if(!isVisible()) // Minimized window?
    emit closed();
}

void glitch_object_edit_window::prepareHeader(const QString &text)
{
  if(!text.trimmed().isEmpty())
    {
      m_header->setText(text.trimmed());
      m_header->setCursorPosition(0);
      m_header->setVisible(true);
      m_header->setReadOnly(false);
      glitch_misc::highlight(m_header);
      m_header->setReadOnly(true);
      setWindowTitle(tr("Glitch: %1").arg(m_header->text()));
    }
  else
    {
      m_header->clear();
      m_header->setVisible(false);
      setWindowTitle(tr("Glitch"));
    }
}

void glitch_object_edit_window::prepareIcons(void)
{
  m_actions.value("close")->setIcon(QIcon(":/close.png"));
  m_actions.value("copy")->setIcon(QIcon(":/copy.png"));
  m_actions.value("delete")->setIcon(QIcon(":/delete.png"));
  m_actions.value("paste")->setIcon(QIcon(":/paste.png"));
  m_actions.value("redo")->setIcon(QIcon(":/redo.png"));
  m_actions.value("save")->setIcon(QIcon(":/save.png"));
  m_actions.value("screen mode")->setIcon(QIcon(":/fullscreen.png"));
  m_actions.value("select all")->setIcon(QIcon(":/select-all.png"));
  m_actions.value("undo")->setIcon(QIcon(":/undo.png"));
}

void glitch_object_edit_window::prepareToolBars(const QList<QAction *> &actions)
{
  m_editToolBar->clear();
  m_editToolBar->addAction(m_actions.value("undo"));
  m_editToolBar->addAction(m_actions.value("redo"));
  m_editToolBar->addSeparator();
  m_editToolBar->addAction(m_actions.value("copy"));
  m_editToolBar->addAction(m_actions.value("paste"));
  m_editToolBar->addSeparator();
  m_editToolBar->addAction(m_actions.value("delete"));
  m_editToolBar->addAction(m_actions.value("select all"));
  m_fileToolBar->clear();
  m_fileToolBar->addAction(m_actions.value("save"));
  m_fileToolBar->addSeparator();
  m_fileToolBar->addAction(m_actions.value("close"));
  m_miscellaneousToolBar->clear();
  m_toolsToolBar->clear();
  m_toolsToolBar->addActions(actions);

  QList<QIcon> icons;
  QStringList data;
  QStringList texts;
  auto menu = new QMenu(this);

  data << "adjust-sizes"
       << "compress-widgets"
       << "disconnect-widgets"
       << "lock-positions"
       << "widget-properties";
  icons << QIcon(":/adjust-size.png")
	<< QIcon(":/compress.png")
	<< QIcon(":/disconnect.png")
	<< QIcon(":/pin.png")
	<< QIcon(":/widget-properties.png");
  texts << tr("Adjust Size(s) (Selected Widget(s))")
	<< tr("(De)compress Selected Widget(s)")
	<< tr("Disconnect Selected Widget(s)")
	<< tr("(Un)lock Position(s) (Selected Widget(s))")
	<< tr("Widget(s) Properties (Selected Widget(s))...");

  for(int i = 0; i < data.size(); i++)
    {
      auto action = menu->addAction(icons.at(i), texts.at(i));

      action->setData(data.at(i));
      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_edit_window::slotSpecialTools);
    }

  auto toolButton = new QToolButton(this);

  toolButton->setArrowType(Qt::NoArrow);
  toolButton->setIcon(QIcon(":/tools.png"));
  toolButton->setMenu(menu);
#ifdef Q_OS_MACOS
#else
  toolButton->setPopupMode(QToolButton::MenuButtonPopup);
#endif
#ifdef Q_OS_MACOS
  toolButton->setStyleSheet
    ("QToolButton {border: none;}"
     "QToolButton::menu-button {border: none;}"
     "QToolButton::menu-indicator {image: none;}");
#endif
  toolButton->setToolTip(tr("Miscellaneous Tools"));
#ifdef Q_OS_ANDROID
  connect(menu,
	  SIGNAL(triggered(QAction *)),
	  this,
	  SLOT(slotHideTearOffMenu(void)));
  connect(toolButton,
	  &QToolButton::clicked,
	  this,
	  &glitch_object_edit_window::slotShowTearOffMenu);
#else
  connect(toolButton,
	  &QToolButton::clicked,
	  toolButton,
	  &QToolButton::showMenu);
#endif
  m_miscellaneousToolBar->addWidget(toolButton);
}

void glitch_object_edit_window::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);

  if(event)
    {
      auto view = qobject_cast<glitch_object_view *> (centralWidget());

      if(view)
	view->setSceneRect(event->size());
    }
}

void glitch_object_edit_window::setCategoriesIconSize(const QString &text)
{
  if(m_arduinoStructures)
    m_arduinoStructures->setIconSize(text);
}

void glitch_object_edit_window::setCentralWidget(QWidget *widget)
{
  if(!widget || centralWidget())
    {
      if(centralWidget())
	qDebug() << tr("Error! A central widget is already assigned!");

      return;
    }

  if(m_arduinoStructures == nullptr &&
     m_projectType == glitch_common::ProjectTypes::ArduinoProject)
    {
      m_arduinoStructures = new glitch_structures_arduino(this);
      m_leftSplitter = new QSplitter(Qt::Vertical, this);
      connect(m_leftSplitter,
	      SIGNAL(splitterMoved(int, int)),
	      this,
	      SLOT(slotSplitterMoved(void)));
    }

  auto frame = new QFrame(this);

  delete frame->layout();
  frame->setLayout(new QVBoxLayout());
  frame->layout()->addWidget(m_header);

  if(m_arduinoStructures)
    {
      m_leftSplitter->addWidget(m_arduinoStructures->frame());
      m_leftSplitter->addWidget(m_userFunctions->frame());
      m_leftSplitter->setStretchFactor(0, 1);
      m_leftSplitter->setStretchFactor(1, 0);
      m_splitter->addWidget(m_leftSplitter);
      m_splitter->addWidget(widget);
      m_splitter->setStretchFactor(0, 0);
      m_splitter->setStretchFactor(1, 1);
    }
  else
    {
      m_splitter->addWidget(m_userFunctions->frame());
      m_splitter->addWidget(widget);
      m_splitter->setStretchFactor(0, 0);
      m_splitter->setStretchFactor(1, 1);
    }

  frame->layout()->addWidget(m_splitter);
  frame->layout()->setContentsMargins(5, 5, 5, 5);
  frame->layout()->setSpacing(5);
  m_splitter->addWidget(m_dockedWidgetPropertyEditors);
  m_splitter->setStretchFactor(m_splitter->count() - 1, 0);
  QMainWindow::setCentralWidget(frame);
}

void glitch_object_edit_window::setEditView(glitch_object_view *view)
{
  if(m_editView)
    {
      disconnect(m_editView->scene(),
		 &glitch_scene::changed,
		 this,
		 &glitch_object_edit_window::slotAboutToShowEditMenu);
      disconnect(m_editView->scene(),
		 &glitch_scene::mousePressed,
		 this,
		 &glitch_object_edit_window::slotAboutToShowEditMenu);
      disconnect(m_editView->scene(),
		 &glitch_scene::selectionChanged,
		 this,
		 &glitch_object_edit_window::slotAboutToShowEditMenu);
    }

  m_editView = view;

  if(m_editView)
    {
      connect(m_editView->scene(),
	      &glitch_scene::changed,
	      this,
	      &glitch_object_edit_window::slotAboutToShowEditMenu,
	      Qt::QueuedConnection);
      connect(m_editView->scene(),
	      &glitch_scene::mousePressed,
	      this,
	      &glitch_object_edit_window::slotAboutToShowEditMenu);
      connect(m_editView->scene(),
	      &glitch_scene::selectionChanged,
	      this,
	      &glitch_object_edit_window::slotAboutToShowEditMenu);
    }
}

void glitch_object_edit_window::setToolBarVisible(const bool state)
{
  m_actions.value("tools")->setChecked(state);
  m_editToolBar->setVisible(state);
  m_fileToolBar->setVisible(state);
  m_miscellaneousToolBar->setVisible(state);
  m_toolsToolBar->setVisible(state);
}

void glitch_object_edit_window::setUndoStack(QUndoStack *undoStack)
{
  if(m_undoStack)
    {
      disconnect(m_undoStack,
		 SIGNAL(cleanChanged(bool)),
		 this,
		 SLOT(slotAboutToShowEditMenu(void)));
      disconnect(m_undoStack,
		 SIGNAL(indexChanged(int)),
		 this,
		 SLOT(slotAboutToShowEditMenu(void)));
    }

  m_undoStack = undoStack;

  if(m_undoStack)
    {
      connect(m_undoStack,
	      SIGNAL(cleanChanged(bool)),
	      this,
	      SLOT(slotAboutToShowEditMenu(void)));
      connect(m_undoStack,
	      SIGNAL(indexChanged(int)),
	      this,
	      SLOT(slotAboutToShowEditMenu(void)));
    }
}

void glitch_object_edit_window::setUserFunctionsModel
(glitch_user_functions_model *model)
{
  m_userFunctions->setModel(model);
}

void glitch_object_edit_window::showEvent(QShowEvent *event)
{
  QMainWindow::showEvent(event);

  if(m_leftSplitter)
    m_leftSplitter->restoreState
      (m_object ?
       m_object->property(glitch_object::Properties::
			  STRUCTURES_VIEW_LEFT_SPLITTER_STATE).toByteArray() :
       QByteArray());

  if(m_splitter)
    m_splitter->restoreState
      (m_object ?
       m_object->property(glitch_object::Properties::
			  STRUCTURES_VIEW_SPLITTER_STATE).toByteArray() :
       QByteArray());

  auto view = qobject_cast<glitch_object_view *> (centralWidget());

  if(view)
    view->setSceneRect(size());
}

void glitch_object_edit_window::slotAboutToShowEditMenu(void)
{
  m_actions.value("copy")->setEnabled
    (m_editView && !m_editView->scene()->selectedItems().isEmpty());
  m_actions.value("delete")->setEnabled
    (m_editView && !m_editView->scene()->selectedItems().isEmpty());
  m_actions.value("paste")->setEnabled(!glitch_ui::s_copiedObjects.isEmpty());
  m_actions.value("redo")->setEnabled(m_undoStack && m_undoStack->canRedo());

  if(m_actions.value("redo")->isEnabled() && m_undoStack)
    m_actions.value("redo")->setText
      (tr("Redo (%1)").arg(m_undoStack->redoText()));
  else
    m_actions.value("redo")->setText(tr("Redo"));

  m_actions.value("select all")->setEnabled
    (m_editView && !m_editView->scene()->items().isEmpty());
  m_actions.value("undo")->setEnabled(m_undoStack && m_undoStack->canUndo());

  if(m_actions.value("undo")->isEnabled() && m_undoStack)
    m_actions.value("undo")->setText
      (tr("Undo (%1)").arg(m_undoStack->undoText()));
  else
    m_actions.value("undo")->setText(tr("Undo"));

  if(m_editView && statusBar())
    statusBar()->showMessage
      (tr("%1 Item(s) Selected").
       arg(m_editView->scene()->selectedItems().size()));
}

void glitch_object_edit_window::slotDockPropertyEditor(QWidget *widget)
{
  m_dockedWidgetPropertyEditors->add
    (qobject_cast<glitch_floating_context_menu *> (widget));
}

void glitch_object_edit_window::slotHideTearOffMenu(void)
{
  auto menu = qobject_cast<QMenu *> (sender());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
#ifdef Q_OS_ANDROID
  if(menu)
    menu->hideTearOffMenu();
#else
  if(menu)
    menu->hide();
#endif
#else
  if(menu)
    menu->hide();
#endif
}

void glitch_object_edit_window::slotPreferencesAccepted(void)
{
  QSettings settings;
  auto state = settings.value
    ("preferences/docked_widget_property_editors", true).toBool();

  if(m_editView && state)
    {
      foreach(auto object, m_editView->scene()->objects())
	if(object && object->menu() && object->menu()->isVisible())
	  slotDockPropertyEditor(object->menu());
    }
  else
    m_dockedWidgetPropertyEditors->detach();

  m_dockedWidgetPropertyEditors->setVisible(state);
}

void glitch_object_edit_window::slotShowFullScreenMode(void)
{
  if(isFullScreen())
    {
      m_actions.value("screen mode")->setText(tr("&Full Screen"));
      showNormal();
    }
  else
    {
      m_actions.value("screen mode")->setText(tr("&Normal Screen"));
      showFullScreen();
    }
}

void glitch_object_edit_window::slotShowTearOffMenu(void)
{
  auto toolButton = qobject_cast<QToolButton *> (sender());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
#ifdef Q_OS_ANDROID
  if(toolButton && toolButton->menu())
    toolButton->menu()->showTearOffMenu();
#else
  if(toolButton)
    toolButton->showMenu();
#endif
#else
  if(toolButton)
    toolButton->showMenu();
#endif
}

void glitch_object_edit_window::slotSpecialTools(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  if(!m_editView || !m_editView->scene())
    return;

  auto type(action->data().toString());

  if(type == "adjust-sizes")
    m_editView->scene()->slotSelectedWidgetsAdjustSize();
  else if(type == "compress-widgets")
    m_editView->scene()->slotSelectedWidgetsCompress();
  else if(type == "disconnect-widgets")
    m_editView->scene()->slotSelectedWidgetsDisconnect();
  else if(type == "lock-positions")
    m_editView->scene()->slotSelectedWidgetsLock();
  else if(type == "widget-properties")
    m_editView->slotSelectedWidgetsProperties();
}

void glitch_object_edit_window::slotSplitterMoved(void)
{
  auto splitter = qobject_cast<QSplitter *> (sender());

  if(m_object)
    {
      if(m_leftSplitter == splitter)
	m_object->setProperty
	  (glitch_object::Properties::STRUCTURES_VIEW_LEFT_SPLITTER_STATE,
	   splitter->saveState());

      if(m_splitter == splitter)
	m_object->setProperty
	  (glitch_object::Properties::STRUCTURES_VIEW_SPLITTER_STATE,
	   splitter->saveState());
    }
}

void glitch_object_edit_window::slotViewTools(void)
{
  m_editToolBar->setVisible(m_actions.value("tools")->isChecked());
  m_fileToolBar->setVisible(m_actions.value("tools")->isChecked());
  m_miscellaneousToolBar->setVisible(m_actions.value("tools")->isChecked());
  m_toolsToolBar->setVisible(m_actions.value("tools")->isChecked());
  emit propertyChanged
    ("tool_bar_visible", m_actions.value("tools")->isChecked());
}
