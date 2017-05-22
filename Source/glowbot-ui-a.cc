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

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSqlQuery>

#include "glowbot-alignment.h"
#include "glowbot-misc.h"
#include "glowbot-separated-diagram-window.h"
#include "glowbot-structures-arduino.h"
#include "glowbot-ui.h"
#include "glowbot-view-arduino.h"

glowbot_ui::glowbot_ui(void):QMainWindow(0)
{
  m_arduinoStructures = 0;
  m_ui.setupUi(this);
  connect(m_ui.action_Alignment,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowAlignment(void)));
  connect(m_ui.action_Close_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotCloseDiagram(void)));
  connect(m_ui.action_Delete,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDelete(void)));
  connect(m_ui.action_New_Arduino,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotNewArduinoDiagram(void)));
  connect(m_ui.action_Open_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotOpenDiagram(void)));
  connect(m_ui.action_Quit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotQuit(void)));
  connect(m_ui.action_Save_Current_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveCurrentDiagram(void)));
  connect(m_ui.action_Select_All,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectAll(void)));
  connect(m_ui.action_Structures,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowStructures(void)));
  connect(m_ui.menu_Tabs,
	  SIGNAL(aboutToShow(void)),
	  this,
	  SLOT(slotAboutToShowTabsMenu(void)));
  connect(m_ui.tab,
	  SIGNAL(currentChanged(int)),
	  this,
	  SLOT(slotPageSelected(int)));
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotCloseDiagram(int)));
  connect(m_ui.tab->tabBar(),
	  SIGNAL(tabMoved(int, int)),
	  this,
	  SLOT(slotTabMoved(int, int)),
	  Qt::QueuedConnection);
  m_ui.menu_Tabs->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setMovable(true);
  m_ui.tab->setTabsClosable(true);
  prepareActionWidgets();
}

glowbot_ui::~glowbot_ui()
{
  if(m_arduinoStructures)
    m_arduinoStructures->deleteLater();
}

bool glowbot_ui::openDiagram(const QString &fileName, QString &error)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString connectionName("");
  bool ok = true;

  {
    QSqlDatabase db(glowbot_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);
	QString name("");
	QString settings("");
	QString type("");

	if(query.exec("SELECT name, settings, type FROM diagram"))
	  if(query.next())
	    {
	      name = query.value(0).toString().trimmed();
	      settings = query.value(1).toString().trimmed();
	      type = query.value(2).toString().trimmed();
	    }

	if(name.isEmpty() || type != "ArduinoProject")
	  {
	    db.close();

	    if(name.isEmpty())
	      error = tr("Empty diagram name.");
	    else
	      error = tr("Expecting a diagram type of ArduinoProject.");

	    ok = false;
	    goto done_label;
	  }

	glowbot_view *view = newArduinoDiagram(name);

	ok = view->open(db, error);
      }
    else
      error = tr("Unable to open %1.").arg(fileName);

    db.close();
  }

 done_label:
  glowbot_common::discardDatabase(connectionName);
  QApplication::restoreOverrideCursor();
  return ok;
}

glowbot_view *glowbot_ui::newArduinoDiagram(const QString &n)
{
  QString name(n);

  name.replace(" ", "-");

  if(name.isEmpty())
    name = "Arduino-Diagram";

  glowbot_view_arduino *view = new glowbot_view_arduino
    (name, glowbot_common::ArduinoProject, this);

  connect(view,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotPageChanged(void)));
  connect(view,
	  SIGNAL(separate(glowbot_view *)),
	  this,
	  SLOT(slotSeparate(glowbot_view *)));
  connect(view,
	  SIGNAL(unite(glowbot_view *)),
	  this,
	  SLOT(slotUnite(glowbot_view *)));
  connect(view->menuAction(),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectPage(void)));
  m_ui.tab->addTab(view, view->menuAction()->icon(), name);
  m_ui.tab->setCurrentWidget(view);
  prepareActionWidgets();
  setWindowTitle(view);
  return view;
}

glowbot_view *glowbot_ui::page(const int index)
{
  return qobject_cast<glowbot_view *> (m_ui.tab->widget(index));
}

void glowbot_ui::closeEvent(QCloseEvent *event)
{
  /*
  ** Detect modified diagrams.
  */

  saveSettings();
  QMainWindow::closeEvent(event);
  QApplication::exit();
}

void glowbot_ui::parseCommandLineArguments(void)
{
  QStringList list(QApplication::arguments());

  for(int i = 1; i < list.size(); i++)
    if(list.at(i) == "--new-arduino-diagram")
      {
	i += 1;
	newArduinoDiagram(list.value(i));
      }
    else if(list.at(i) == "--version")
      {
      }
}

void glowbot_ui::prepareActionWidgets(void)
{
  if(m_ui.tab->count() == 0)
    {
      m_ui.action_Alignment->setEnabled(false);
      m_ui.action_Close_Diagram->setEnabled(false);

      if(findChildren<glowbot_separated_diagram_window *> ().size() > 0)
	m_ui.action_Structures->setEnabled(true);
      else
	m_ui.action_Structures->setEnabled(false);
    }
  else
    {
      m_ui.action_Alignment->setEnabled(true);
      m_ui.action_Close_Diagram->setEnabled(true);
      m_ui.action_Structures->setEnabled(true);
    }
}

void glowbot_ui::restoreSettings(void)
{
  QSettings settings;

  restoreGeometry(settings.value("main_window/geometry").toByteArray());
}

void glowbot_ui::saveSettings(void)
{
  QSettings settings;

  settings.setValue("main_window/geometry", saveGeometry());
}

void glowbot_ui::setWindowTitle(glowbot_view *view)
{
  if(view)
    {
      if(view->hasChanged())
	QMainWindow::setWindowTitle(tr("GlowBot: %1 (*)").arg(view->name()));
      else
	QMainWindow::setWindowTitle(tr("GlowBot: %1").arg(view->name()));
    }
  else
    QMainWindow::setWindowTitle(tr("GlowBot"));
}

void glowbot_ui::show(void)
{
  restoreSettings();
  QMainWindow::show();

  /*
  ** Some desktop managers are strange.
  */

  repaint();
  QApplication::flush();
  parseCommandLineArguments();
}

void glowbot_ui::slotAboutToShowTabsMenu(void)
{
  slotTabMoved(0, 0);

  if(m_ui.menu_Tabs->actions().isEmpty())
    m_ui.menu_Tabs->addAction(tr("Empty"))->setEnabled(false);
}

void glowbot_ui::slotCloseDiagram(int index)
{
  glowbot_view *view = page(index);

  if(view)
    view->deleteLater();

  m_ui.tab->removeTab(index);
  prepareActionWidgets();
}

void glowbot_ui::slotCloseDiagram(void)
{
  slotCloseDiagram(m_ui.tab->currentIndex());
}

void glowbot_ui::slotDelete(void)
{
  glowbot_view *view = page(m_ui.tab->currentIndex());

  if(!view)
    return;

  view->deleteItems();
}

void glowbot_ui::slotNewArduinoDiagram(void)
{
  QString name("");
  bool ok = true;

  name = QInputDialog::getText
    (this, tr("GlowBot: Arduino Project Name"), tr("Project Name"),
     QLineEdit::Normal, "Arduino-Diagram", &ok).trimmed();

  if(!ok)
    return;
  else if(name.isEmpty())
    name = "Arduino-Diagram";

  QString fileName(QString("%1%2%3.db").
		   arg(glowbot_misc::homePath()).
		   arg(QDir::separator()).
		   arg(name));

  if(QFile::exists(fileName))
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText(tr("The file %1 already exists. Continue?").arg(fileName));
      mb.setWindowModality(Qt::WindowModal);
      mb.setWindowTitle(tr("GlowBot: Question"));

      if(mb.exec() != QMessageBox::Yes)
	return;
    }

  newArduinoDiagram(name);
}

void glowbot_ui::slotOpenDiagram(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(glowbot_misc::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setWindowTitle(tr("GlowBot: Open Diagram"));

  if(dialog.exec() == QDialog::Accepted)
    {
      QString error("");

      if(openDiagram(dialog.selectedFiles().value(0), error))
	prepareActionWidgets();
    }
}

void glowbot_ui::slotPageChanged(void)
{
  prepareActionWidgets();
  setWindowTitle(qobject_cast<glowbot_view *> (sender()));
}

void glowbot_ui::slotPageSelected(int index)
{
  prepareActionWidgets();
  setWindowTitle(qobject_cast<glowbot_view *> (m_ui.tab->widget(index)));
}

void glowbot_ui::slotQuit(void)
{
  close();
}

void glowbot_ui::slotSaveCurrentDiagram(void)
{
  glowbot_view *view = page(m_ui.tab->currentIndex());

  if(view)
    {
      QString error("");

      if(!view->save(error))
	glowbot_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(view->name()).arg(error), this);

      setWindowTitle(view);
    }
}

void glowbot_ui::slotSelectAll(void)
{
  glowbot_view *view = page(m_ui.tab->currentIndex());

  if(!view)
    return;

  view->selectAll();
}

void glowbot_ui::slotSelectPage(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  m_ui.tab->setCurrentWidget(action->parentWidget());
  setWindowTitle(qobject_cast<glowbot_view *> (m_ui.tab->currentWidget()));
}

void glowbot_ui::slotSeparate(glowbot_view *view)
{
  if(!view)
    return;

  QMainWindow::setWindowTitle(tr("GlowBot"));
  m_ui.tab->removeTab(m_ui.tab->indexOf(view));

  glowbot_separated_diagram_window *window =
    new glowbot_separated_diagram_window(this);

  window->setCentralWidget(view);
  view->show();
  window->resize(view->size());
  window->setWindowTitle(tr("GlowBot: %1").arg(view->name()));
  window->show();
  prepareActionWidgets();
}

void glowbot_ui::slotShowAlignment(void)
{
  glowbot_view *view = qobject_cast<glowbot_view *> (m_ui.tab->currentWidget());

  if(view)
    view->showAlignment();
}

void glowbot_ui::slotShowStructures(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if(!findChildren<glowbot_view_arduino *> ().isEmpty())
    {
      QApplication::restoreOverrideCursor();

      if(!m_arduinoStructures)
	m_arduinoStructures = new glowbot_structures_arduino(0);

      m_arduinoStructures->showNormal();
      m_arduinoStructures->activateWindow();
      m_arduinoStructures->raise();
    }
  else
    QApplication::restoreOverrideCursor();
}

void glowbot_ui::slotTabMoved(int from, int to)
{
  Q_UNUSED(from);
  Q_UNUSED(to);

  m_ui.menu_Tabs->clear();

  for(int i = 0; i < m_ui.tab->count(); i++)
    {
      glowbot_view *view = qobject_cast<glowbot_view *> (m_ui.tab->widget(i));

      if(view)
	m_ui.menu_Tabs->addAction(view->menuAction());
    }
}

void glowbot_ui::slotUnite(glowbot_view *view)
{
  if(!view)
    return;

  QMainWindow *window = qobject_cast<QMainWindow *> (view->parentWidget());

  if(!window)
    return;

  m_ui.tab->addTab(view, view->menuAction()->icon(), view->name());
  m_ui.tab->setCurrentWidget(view);
  prepareActionWidgets();
  setWindowTitle(view);
  window->deleteLater();
}
