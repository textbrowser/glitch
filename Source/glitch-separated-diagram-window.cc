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

#include <QClipboard>
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
  m_ui.action_Generate_Source->setEnabled(false);
  m_ui.action_Generate_Source_Clipboard->setEnabled(false);
  m_ui.action_Generate_Source_View->setEnabled(false);
  m_ui.miscellaneous_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.miscellaneous_toolbar->setIconSize(QSize(24, 24));
  m_ui.tools_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
  m_ui.tools_toolbar->setIconSize(QSize(24, 24));
  connect(&m_statusBarTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_separated_diagram_window::slotStatusBarTimerTimeout);
  connect(m_ui.action_Close_Diagram,
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
  connect(m_ui.action_Generate_Source_Clipboard,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotGenerateSourceClipboard);
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
  connect(m_ui.action_Unite_Canvas,
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
      m_ui.action_Generate_Source->setEnabled(true);
      m_ui.action_Generate_Source_Clipboard->setEnabled
	(QApplication::clipboard());
      m_ui.action_Generate_Source_View->setEnabled(true);
      m_ui.action_Paste->setEnabled(!glitch_ui::s_copiedObjects.isEmpty());
      m_ui.action_Save_Diagram->setEnabled(true);
      m_ui.action_Select_All->setEnabled(m_view->scene()->items().size() > 2);
    }
  else
    {
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Generate_Source->setEnabled(false);
      m_ui.action_Generate_Source_Clipboard->setEnabled(false);
      m_ui.action_Generate_Source_View->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Save_Diagram->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
    }

  prepareRedoUndoActions();
}

void glitch_separated_diagram_window::prepareIcons(void)
{
  m_ui.action_Close_Diagram->setIcon(QIcon(":/close.png"));
  m_ui.action_Copy->setIcon(QIcon(":/copy.png"));
  m_ui.action_Delete->setIcon(QIcon(":/delete.png"));
  m_ui.action_Find->setIcon(QIcon(":/find.png"));
  m_ui.action_Generate_Source_View->setIcon(QIcon(":/source.png"));
  m_ui.action_Paste->setIcon(QIcon(":/paste.png"));
  m_ui.action_Redo->setIcon(QIcon(":/redo.png"));
  m_ui.action_Save_Diagram->setIcon(QIcon(":/save.png"));
  m_ui.action_Select_All->setIcon(QIcon(":/select-all.png"));
  m_ui.action_Undo->setIcon(QIcon(":/undo.png"));
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
  if(m_ui.edit_toolbar->actions().isEmpty())
    {
      m_ui.edit_toolbar->addAction(m_ui.action_Undo);
      m_ui.edit_toolbar->addAction(m_ui.action_Redo);
      m_ui.edit_toolbar->addSeparator();
      m_ui.edit_toolbar->addAction(m_ui.action_Copy);
      m_ui.edit_toolbar->addAction(m_ui.action_Paste);
      m_ui.edit_toolbar->addSeparator();
      m_ui.edit_toolbar->addAction(m_ui.action_Delete);
      m_ui.edit_toolbar->addAction(m_ui.action_Select_All);
      m_ui.edit_toolbar->addSeparator();
      m_ui.edit_toolbar->addAction(m_ui.action_Find);
    }

  if(m_ui.file_toolbar->actions().isEmpty())
    {
      m_ui.file_toolbar->addAction(m_ui.action_Save_Diagram);
      m_ui.file_toolbar->addSeparator();
      m_ui.file_toolbar->addAction(m_ui.action_Close_Diagram);
    }

  m_ui.miscellaneous_toolbar->clear();
  m_ui.tools_toolbar->clear();

  if(m_view)
    {
      /*
      ** Miscellaneous.
      */

      auto menu = new QMenu(this);
      auto toolButton = new QToolButton(this);

#ifdef Q_OS_ANDROID
      connect(menu,
	      SIGNAL(triggered(QAction *)),
	      this,
	      SLOT(slotHideTearOffMenu(void)));
      connect(toolButton,
	      &QToolButton::clicked,
	      this,
	      &glitch_separated_diagram_window::slotShowTearOffMenu);
#else
      connect(toolButton,
	      &QToolButton::clicked,
	      toolButton,
	      &QToolButton::showMenu);
#endif
      m_view->populateToolsMenu(menu, this);
      toolButton->setArrowType(Qt::NoArrow);
      toolButton->setIcon(QIcon(":/wire.png"));
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
      toolButton->setToolTip(tr("Connection Tools"));
      m_ui.miscellaneous_toolbar->addWidget(toolButton);

      QList<QIcon> icons;
      QStringList data;
      QStringList texts;

      data << "adjust-sizes"
	   << "compress-widgets"
	   << "disconnect-widgets"
	   << "fonts"
	   << "lock-positions"
	   << "widget-properties";
      icons << QIcon(":/adjust-size.png")
	    << QIcon(":/compress.png")
	    << QIcon(":/disconnect.png")
	    << QIcon(":/font.png")
	    << QIcon(":/pin.png")
	    << QIcon(":/widget-properties.png");
      menu = new QMenu(this);
      texts << tr("Adjust Size(s) (Selected Widget(s))")
	    << tr("(De)compress Selected Widget(s)")
	    << tr("Disconnect Selected Widget(s)")
	    << tr("Fonts (All Widgets)...")
	    << tr("(Un)lock Position(s) (Selected Widget(s))")
	    << tr("Widget(s) Properties (Selected Widget(s))...");

      for(int i = 0; i < data.size(); i++)
	{
	  auto action = menu->addAction(icons.at(i), texts.at(i));

	  action->setData(data.at(i));
	  connect(action,
		  &QAction::triggered,
		  this,
		  &glitch_separated_diagram_window::slotSpecialTools);
	}

      toolButton = new QToolButton(this);
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
	      &glitch_separated_diagram_window::slotShowTearOffMenu);
#else
      connect(toolButton,
	      &QToolButton::clicked,
	      toolButton,
	      &QToolButton::showMenu);
#endif
      m_ui.miscellaneous_toolbar->addWidget(toolButton);
    }

  if(m_view)
    /*
    ** Tools.
    */

    m_ui.tools_toolbar->addActions(m_view->alignmentActions());
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
      slotToolsOperationChanged(m_view->toolsOperation());
    }

  prepareToolBar();
  prepareActionWidgets();
}

void glitch_separated_diagram_window::slotCopy(void)
{
  emit copy(m_view);
  m_ui.action_Paste->setEnabled(!glitch_ui::s_copiedObjects.empty());

  if(statusBar())
    {
      statusBar()->showMessage
	(tr("%1 widget(s) copied.").arg(glitch_ui::s_copiedObjects.size()),
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

void glitch_separated_diagram_window::slotGenerateSourceClipboard(void)
{
  if(m_view)
    {
      auto clipboard = QApplication::clipboard();

      if(clipboard)
	clipboard->setText(m_view->source());
    }
}

void glitch_separated_diagram_window::slotGenerateSourceView(void)
{
  if(m_view)
    m_view->generateSourceView();
}

void glitch_separated_diagram_window::slotHideTearOffMenu(void)
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

void glitch_separated_diagram_window::slotShowTearOffMenu(void)
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

void glitch_separated_diagram_window::slotSpecialTools(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  if(!m_view || !m_view->scene())
    return;

  auto type(action->data().toString());

  if(type == "adjust-sizes")
    m_view->scene()->slotSelectedWidgetsAdjustSize();
  else if(type == "compress-widgets")
    m_view->scene()->slotSelectedWidgetsCompress();
  else if(type == "disconnect-widgets")
    m_view->scene()->slotSelectedWidgetsDisconnect();
  else if(type == "fonts")
    m_view->slotFonts();
  else if(type == "lock-positions")
    m_view->scene()->slotSelectedWidgetsLock();
  else if(type == "widget-properties")
    m_view->slotSelectedWidgetsProperties();
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
