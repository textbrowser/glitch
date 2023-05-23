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

#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QToolButton>

#include "glitch-misc.h"
#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-ui.h"
#include "glitch-view.h"

glitch_separated_diagram_window::
glitch_separated_diagram_window(QWidget *parent):QMainWindow(parent)
{
  m_statusBarTimer.start(500);
  m_ui.setupUi(this);
  m_ui.tools_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.tools_toolbar->setIconSize(QSize(24, 24));
  connect(&m_statusBarTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_separated_diagram_window::slotStatusBarTimerTimeout);
  connect(m_ui.action_Close,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::close);
  connect(m_ui.action_Context_Menu,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotShowContextMenu);
  connect(m_ui.action_Copy,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotCopy);
  connect(m_ui.action_Find,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotFind);
  connect(m_ui.action_Generate_Source,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotGenerateSource);
  connect(m_ui.action_Generate_Source_View,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotGenerateSourceView);
  connect(m_ui.action_Delete,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotDelete);
  connect(m_ui.action_Paste,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotPaste);
  connect(m_ui.action_Redo,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotRedo);
  connect(m_ui.action_Save_Diagram,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotSaveDiagram);
  connect(m_ui.action_Select_All,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotSelectAll);
  connect(m_ui.action_Undo,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotUndo);
  connect(m_ui.action_Unite,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::unite);
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);
  prepareIcons();
  slotPreferencesAccepted();
  statusBar(); // Create a status bar.
}

glitch_separated_diagram_window::~glitch_separated_diagram_window()
{
  if(m_view)
    disconnect(m_view, nullptr, this, nullptr);

  m_statusBarTimer.stop();
}

void glitch_separated_diagram_window::closeEvent(QCloseEvent *event)
{
  if(event && m_view && m_view->hasChanged())
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText
	(tr("The display has not been saved. Are you sure that "
	    "you wish to close it?"));
      mb.setWindowIcon(windowIcon());
      mb.setWindowModality(Qt::ApplicationModal);
      mb.setWindowTitle(tr("Glitch: Confirmation"));

      if(mb.exec() != QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  event->ignore();
	  return;
	}

      QApplication::processEvents();
    }

  deleteLater();
}

void glitch_separated_diagram_window::prepareActionWidgets(void)
{
  if(m_view)
    {
      m_ui.action_Copy->setEnabled(!m_view->scene()->selectedItems().empty());
      m_ui.action_Delete->setEnabled(!m_view->scene()->selectedItems().empty());
      m_ui.action_Paste->setEnabled(!glitch_ui::copiedObjects().isEmpty());
      m_ui.action_Save_Diagram->setEnabled(true);
      m_ui.action_Select_All->setEnabled(m_view->scene()->items().size() > 2);
    }
  else
    {
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Save_Diagram->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
    }

  prepareRedoUndoActions();
}

void glitch_separated_diagram_window::prepareIcons(void)
{
  m_ui.action_Close->setIcon(QIcon::fromTheme("window-close"));
  m_ui.action_Copy->setIcon(QIcon::fromTheme("edit-copy"));
  m_ui.action_Delete->setIcon(QIcon::fromTheme("edit-delete"));
  m_ui.action_Find->setIcon(QIcon::fromTheme("edit-find"));
  m_ui.action_Paste->setIcon(QIcon::fromTheme("edit-paste"));
  m_ui.action_Redo->setIcon(QIcon::fromTheme("edit-redo"));
  m_ui.action_Save_Diagram->setIcon(QIcon::fromTheme("document-save"));
  m_ui.action_Select_All->setIcon(QIcon::fromTheme("edit-select-all"));
  m_ui.action_Undo->setIcon(QIcon::fromTheme("edit-undo"));
}

void glitch_separated_diagram_window::prepareRedoUndoActions(void)
{
  if(m_view)
    {
      m_ui.action_Redo->setEnabled(m_view->canRedo());

      if(m_ui.action_Redo->isEnabled())
	m_ui.action_Redo->setText(tr("Redo (%1)").arg(m_view->redoText()));
      else
	m_ui.action_Redo->setText(tr("Redo"));

      m_ui.action_Undo->setEnabled(m_view->canUndo());

      if(m_ui.action_Undo->isEnabled())
	m_ui.action_Undo->setText(tr("Undo (%1)").arg(m_view->undoText()));
      else
	m_ui.action_Undo->setText(tr("Undo"));
    }
  else
    {
      m_ui.action_Redo->setEnabled(false);
      m_ui.action_Redo->setText(tr("Redo"));
      m_ui.action_Undo->setEnabled(false);
      m_ui.action_Undo->setText(tr("Undo"));
    }
}

void glitch_separated_diagram_window::prepareToolBar(void)
{
  m_ui.tools_toolbar->clear();

  if(m_view)
    {
      for(int i = 0; i < m_view->alignmentActions().size(); i++)
	m_ui.tools_toolbar->addAction(m_view->alignmentActions().at(i));

      m_ui.tools_toolbar->addSeparator();

      QAction *action1 = nullptr;
      QAction *action2 = nullptr;
      auto menu = new QMenu(this);
      auto toolButton = new QToolButton(this);

      action1 = menu->addAction
	(QIcon(":/adjust-size.png"), tr("Adjust Size(s)"));
      action2 = menu->addAction
	(QIcon(":/compress.png"), tr("(De)compress Widget(s)"));
      connect(action1,
	      &QAction::triggered,
	      this,
	      &glitch_separated_diagram_window::slotAdjustSizesTool);
      connect(action2,
	      &QAction::triggered,
	      this,
	      &glitch_separated_diagram_window::slotCompressWidgetsTool);
      toolButton->setArrowType(Qt::NoArrow);
      toolButton->setIcon(QIcon(":/tools.png"));
      toolButton->setMenu(menu);
      toolButton->setPopupMode(QToolButton::MenuButtonPopup);
      toolButton->setToolTip(tr("Miscellaneous Tools"));
      connect(toolButton,
	      &QToolButton::clicked,
	      toolButton,
	      &QToolButton::showMenu);
      m_ui.tools_toolbar->addWidget(toolButton);
    }
}

void glitch_separated_diagram_window::setCentralWidget(QWidget *widget)
{
  if(centralWidget())
    centralWidget()->deleteLater();

  if(m_view)
    {
      disconnect(m_view,
		 &glitch_view::changed,
		 this,
		 &glitch_separated_diagram_window::slotPageChanged);
      disconnect(m_view,
		 &glitch_view::saved,
		 this,
		 &glitch_separated_diagram_window::slotPageSaved);
      disconnect(m_view,
		 &glitch_view::selectionChanged,
		 this,
		 &glitch_separated_diagram_window::slotSelectionChanged);
      disconnect
	(m_view,
	 SIGNAL(toolsOperationChanged(const glitch_tools::Operations)),
	 this,
	 SLOT(slotToolsOperationChanged(const glitch_tools::Operations)));
      disconnect(this,
		 &glitch_separated_diagram_window::unite,
		 m_view,
		 &glitch_view::slotUnite);
    }

  m_view = qobject_cast<glitch_view *> (widget);

  if(m_view)
    {
      connect(m_view,
	      &glitch_view::changed,
	      this,
	      &glitch_separated_diagram_window::slotPageChanged);
      connect(m_view,
	      &glitch_view::saved,
	      this,
	      &glitch_separated_diagram_window::slotPageSaved);
      connect(m_view,
	      &glitch_view::selectionChanged,
	      this,
	      &glitch_separated_diagram_window::slotSelectionChanged);
      connect(m_view,
	      SIGNAL(toolsOperationChanged(const glitch_tools::Operations)),
	      this,
	      SLOT(slotToolsOperationChanged(const glitch_tools::Operations)));
      connect(this,
	      &glitch_separated_diagram_window::unite,
	      m_view,
	      &glitch_view::slotUnite);
      QMainWindow::setCentralWidget(m_view);
      prepareActionWidgets();
      prepareToolBar();
      slotToolsOperationChanged(m_view->toolsOperation());
    }
}

void glitch_separated_diagram_window::slotAdjustSizesTool(void)
{
  if(m_view && m_view->scene())
    m_view->scene()->slotSelectedWidgetsAdjustSize();
}

void glitch_separated_diagram_window::slotCompressWidgetsTool(void)
{
  if(m_view && m_view->scene())
    m_view->scene()->slotSelectedWidgetsCompress();
}

void glitch_separated_diagram_window::slotCopy(void)
{
  emit copy(m_view);
  m_ui.action_Paste->setEnabled(!glitch_ui::copiedObjects().empty());

  if(statusBar())
    {
      statusBar()->showMessage
	(tr("%1 widget(s) copied.").arg(glitch_ui::copiedObjects().size()),
	 5000);
      statusBar()->repaint();
    }
}

void glitch_separated_diagram_window::slotDelete(void)
{
  if(m_view)
    {
      m_view->deleteItems();
      m_ui.action_Undo->setEnabled(m_view->canUndo());

      if(m_ui.action_Undo->isEnabled())
	m_ui.action_Undo->setText(tr("Undo (%1)").arg(m_view->undoText()));
      else
	m_ui.action_Undo->setText(tr("Undo"));
    }
}

void glitch_separated_diagram_window::slotFind(void)
{
  if(m_view)
    m_view->find();
}

void glitch_separated_diagram_window::slotGenerateSource(void)
{
  if(m_view)
    {
      if(statusBar())
	{
	  statusBar()->showMessage(tr("Generating source. Please be patient."));
	  statusBar()->repaint();
	}

      m_view->generateSourceFile();

      if(statusBar())
	{
	  statusBar()->showMessage("");
	  statusBar()->repaint();
	}
    }
}

void glitch_separated_diagram_window::slotGenerateSourceView(void)
{
  if(m_view)
    m_view->generateSourceView();
}

void glitch_separated_diagram_window::slotPageChanged(void)
{
  if(m_view)
    {
      if(m_view->hasChanged())
	QMainWindow::setWindowTitle(tr("Glitch: %1 (*)").arg(m_view->name()));
      else
	QMainWindow::setWindowTitle(tr("Glitch: %1").arg(m_view->name()));
    }
  else
    QMainWindow::setWindowTitle(tr("Glitch"));

  prepareActionWidgets();
}

void glitch_separated_diagram_window::slotPageSaved(void)
{
  slotPageChanged();
}

void glitch_separated_diagram_window::slotPaste(void)
{
  emit paste(m_view);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  prepareRedoUndoActions();
  QApplication::restoreOverrideCursor();
}

void glitch_separated_diagram_window::slotPreferencesAccepted(void)
{
  QSettings settings;

  m_ui.menu_Edit->setTearOffEnabled
    (settings.value("preferences/tear_off_menus", true).toBool());
}

void glitch_separated_diagram_window::slotRedo(void)
{
  if(m_view)
    {
      m_view->redo();
      prepareRedoUndoActions();
    }
}

void glitch_separated_diagram_window::slotSaveDiagram(void)
{
  if(m_view)
    {
      QString error("");

      if(!m_view->save(error))
	glitch_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(m_view->name()).arg(error), this);
    }

  prepareActionWidgets();
  slotPageSaved();
}

void glitch_separated_diagram_window::slotSelectAll(void)
{
  if(m_view)
    m_view->selectAll();
}

void glitch_separated_diagram_window::slotSelectionChanged(void)
{
  prepareActionWidgets();
}

void glitch_separated_diagram_window::slotShowContextMenu(void)
{
  if(m_view)
    {
      auto menu = m_view->defaultContextMenu();

      if(menu)
	{
	  menu->update();
	  menu->raise();
	  menu->exec(mapToGlobal(QPoint(size().width() / 2, 0)));
	}
    }
}

void glitch_separated_diagram_window::slotStatusBarTimerTimeout(void)
{
  if(statusBar() &&
     statusBar()->currentMessage().trimmed().isEmpty() &&
     m_view)
    slotToolsOperationChanged(m_view->toolsOperation());
}

void glitch_separated_diagram_window::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  if(statusBar())
    {
      if(operation == glitch_tools::Operations::INTELLIGENT)
	statusBar()->showMessage(tr("Connections Mode: Intelligent"));
      else if(operation == glitch_tools::Operations::SELECT)
	statusBar()->showMessage(tr("Connections Mode: Select"));
      else if(operation == glitch_tools::Operations::WIRE_CONNECT)
	statusBar()->showMessage(tr("Connections Mode: Wire (Connect)"));
      else
	statusBar()->showMessage(tr("Connections Mode: Wire (Disconnect)"));

      statusBar()->repaint();
    }
}

void glitch_separated_diagram_window::slotUndo(void)
{
  if(m_view)
    {
      m_view->undo();
      prepareRedoUndoActions();
      slotPageChanged();
    }
}
