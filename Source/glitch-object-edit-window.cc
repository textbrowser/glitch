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
    (tr("&Save Current Diagram"), tr("Ctrl+S"), this, SIGNAL(saveSignal(void)));
#else
  m_actions["save"] =
    menu->addAction
    (tr("&Save Current Diagram"), this, SIGNAL(saveSignal(void)), tr("Ctrl+S"));
#endif
  menu->addSeparator();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
  m_actions["close"] =
    menu->addAction(tr("&Close"), tr("Ctrl+W"), this, SLOT(close(void)));
#else
  m_actions["close"] =
    menu->addAction(tr("&Close"), this, SLOT(close(void)), tr("Ctrl+W"));
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
  m_dockedWidgetPropertyEditors->setMinimumWidth(250);
  m_header = new QLineEdit(this);
  m_header->setReadOnly(true);
  m_header->setVisible(false);
  m_object = object;
  m_projectType = projectType;
  m_splitter = nullptr;
  m_toolBar = new QToolBar(tr("Tools Tool Bar"), this);
  m_toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_toolBar->setIconSize(QSize(24, 24));
  m_toolBar->setObjectName("tools_tool_bar");
  m_toolBar->setVisible(true);
  addToolBar(m_toolBar);
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);
  prepareIcons();
  resize(800, 600);
  setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | windowFlags());
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
    if(qobject_cast
       <glitch_object_view *> (frame->layout()->itemAt(i)->widget()))
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
  m_actions.value("close")->setIcon(QIcon::fromTheme("window-close"));
  m_actions.value("copy")->setIcon(QIcon::fromTheme("edit-copy"));
  m_actions.value("delete")->setIcon(QIcon::fromTheme("edit-delete"));
  m_actions.value("paste")->setIcon(QIcon::fromTheme("edit-paste"));
  m_actions.value("redo")->setIcon(QIcon::fromTheme("edit-redo"));
  m_actions.value("save")->setIcon(QIcon::fromTheme("document-save"));
  m_actions.value("screen mode")->setIcon(QIcon::fromTheme("view-fullscreen"));
  m_actions.value("select all")->setIcon(QIcon::fromTheme("edit-select-all"));
  m_actions.value("undo")->setIcon(QIcon::fromTheme("edit-undo"));
}

void glitch_object_edit_window::prepareToolBar(const QList<QAction *> &actions)
{
  m_toolBar->clear();
  m_toolBar->addActions(actions);
  m_toolBar->addSeparator();

  auto menu = new QMenu(this);
  auto toolButton = new QToolButton(this);

  menu->addAction(tr("Adjust Size(s)"));
  menu->addAction(tr("Compress Widget(s)"));
  toolButton->setArrowType(Qt::NoArrow);
  toolButton->setIcon(QIcon(":/tools.png"));
  toolButton->setMenu(menu);
  toolButton->setPopupMode(QToolButton::MenuButtonPopup);
  toolButton->setToolTip(tr("Miscellaneous Tools"));
  connect(toolButton,
	  &QToolButton::clicked,
	  toolButton,
	  &QToolButton::showMenu);
  m_toolBar->addWidget(toolButton);
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

  if(m_object)
    m_object->setProperty
      (glitch_object::Properties::EDIT_WINDOW_GEOMETRY, saveGeometry());
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
      m_splitter = new QSplitter(this);
      connect(m_splitter,
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
      m_splitter->addWidget(m_arduinoStructures->frame());
      m_splitter->addWidget(widget);
      m_splitter->setStretchFactor(0, 0);
      m_splitter->setStretchFactor(1, 1);
    }
  else
    {
      m_splitter->addWidget(widget);
      m_splitter->setStretchFactor(0, 1);
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
  m_toolBar->setVisible(state);
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

void glitch_object_edit_window::showEvent(QShowEvent *event)
{
  QMainWindow::showEvent(event);

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
  m_actions.value("paste")->setEnabled(!glitch_ui::copiedObjects().isEmpty());
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

void glitch_object_edit_window::slotSplitterMoved(void)
{
  if(m_object)
    m_object->setProperty
      (glitch_object::Properties::STRUCTURES_VIEW_SPLITTER_STATE,
       m_splitter->saveState());
}

void glitch_object_edit_window::slotViewTools(void)
{
  m_toolBar->setVisible(m_actions.value("tools")->isChecked());
  emit propertyChanged
    ("tool_bar_visible", m_actions.value("tools")->isChecked());
}
