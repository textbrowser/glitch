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

#include <QFileDialog>
#include <QToolButton>

#include "Arduino/glitch-object-function-arduino.h"
#include "glitch-object.h"
#include "glitch-recent-diagram.h"
#include "glitch-scene.h"
#include "glitch-serial-port-window.h"
#include "glitch-ui.h"
#include "glitch-variety.h"
#include "glitch-view.h"

QList<glitch_object *> glitch_ui::copySelected
(QGraphicsView *view,
 QList<QPointF> *points,
 const bool deselectOriginal,
 const bool selected)
{
  QList<glitch_object *> objects;

  if(!view || !view->scene())
    return objects;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto list
    (!selected ? view->scene()->items() : view->scene()->selectedItems());

  foreach(auto i, list)
    {
      if(!i)
	continue;

      auto proxy = qgraphicsitem_cast<glitch_proxy_widget *> (i);

      if(!proxy || proxy->isMandatory())
	continue;
      else if((!(proxy->flags() & QGraphicsItem::ItemIsSelectable) ||
	       !proxy->isSelected()) &&
	      selected)
	continue;

      auto object = proxy->object();

      if(!object)
	continue;

      auto point(object->scenePos());
      glitch_object *clone = nullptr;

      if(qobject_cast<glitch_object_function_arduino *> (object))
	{
	  /*
	  ** Clone the real function.
	  */

	  object = qobject_cast<glitch_object_function_arduino *>
	    (object)->parentFunction();

	  if(!object)
	    object = proxy->object();

	  if(object)
	    {
	      clone = object->clone(nullptr);
	      clone->setOriginalPosition(point);
	    }
	}
      else
	clone = object->clone(nullptr);

      if(deselectOriginal)
	proxy->setSelected(false);

      if(!clone)
	continue;

      objects << clone;

      if(points)
	points->append(point);
    }

  QApplication::restoreOverrideCursor();
  return objects;
}

QString glitch_ui::about(void) const
{
  return m_about.text();
}

#ifdef Q_OS_ANDROID
void glitch_ui::copyExamplesForAndroid(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QDir dir(glitch_variety::homePath());

  dir.mkdir("Examples");
  dir = QDir("assets:/Examples");

  foreach(const auto &str, dir.entryList())
    if(!QFileInfo(glitch_variety::homePath() +
		  QDir::separator() +
		  "Examples" +
		  QDir::separator() +
		  str).exists())
      {
	QFile file(dir.absolutePath() + QDir::separator() + str);

	Q_UNUSED(file.copy(glitch_variety::homePath() +
			   QDir::separator() +
			   "Examples" +
			   QDir::separator() +
			   str));
      }

  QApplication::restoreOverrideCursor();
}
#endif

void glitch_ui::slotAboutToShowProjectMenu(void)
{
  m_ui.action_Generate_Source_Clipboard->setEnabled
    (QApplication::clipboard() && m_currentView);
}

void glitch_ui::slotAboutToShowRecentDiagrams(void)
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_MACOS)
  return;
#endif

  for(int i = 0; i < m_ui.menu_Recent_Diagrams->actions().size(); i++)
    {
      auto action = qobject_cast<glitch_recent_diagram *>
	(m_ui.menu_Recent_Diagrams->actions().at(i));

      if(!action)
	continue;

      if(action->label())
	{
	  QFileInfo fileInfo(action->fileName());

	  if(!fileInfo.exists() || !fileInfo.isReadable())
	    {
	      action->label()->setStyleSheet
		("QLabel {color: rgb(240, 128, 128);}");

	      if(!fileInfo.exists())
		action->label()->setToolTip(tr("File does not exist."));
	      else
		action->label()->setToolTip(tr("File is not readable."));
	    }
	  else
	    {
	      action->label()->setStyleSheet("");
	      action->label()->setToolTip("");
	    }
	}
    }
}

void glitch_ui::slotGenerateSourceClipboard(void)
{
  if(m_currentView)
    m_currentView->generateSourceClipboard();
}

void glitch_ui::slotHideTearOffMenu(void)
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

void glitch_ui::slotRecentDiagramHovered(QAction *action)
{
  if(m_recentDiagramHoveredAction)
    {
      auto widget = qobject_cast<glitch_recent_diagram *>
	(m_recentDiagramHoveredAction);

      if(widget)
	widget->highlight(false);
    }

  auto widget = qobject_cast<glitch_recent_diagram *> (action);

  if(widget)
    widget->highlight(true);

  m_recentDiagramHoveredAction = action;
}

void glitch_ui::slotSaveAsPNG(void)
{
  if(!m_currentView || !m_currentView->scene())
    return;

  QFileDialog dialog(this, tr("Glitch: Save Current Diagram As PNG"));

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setNameFilter("PNG Files (*.png)");
  dialog.setOption(QFileDialog::DontConfirmOverwrite, true);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowIcon(windowIcon());
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      auto i = 0;

      foreach(auto object, m_currentView->scene()->allObjects())
	if(object)
	  {
	    auto image(object->image());

	    if(image.isNull() == false)
	      {
		auto fileName(dialog.selectedFiles().value(0));

		fileName.append(QDir::separator());
		fileName.append(m_currentView->name().replace(' ', '_'));
		fileName.append("_");
		fileName.append(object->objectType());
		fileName.append("_");
		fileName.append(QString::number(i));
		fileName.append(".png");
		i += 1;
		image.save(fileName, "PNG", 100);
	      }
	  }

      QApplication::restoreOverrideCursor();
    }
}

void glitch_ui::slotSeparate(void)
{
  slotSeparate(m_currentView);
}

void glitch_ui::slotShowSerialPortWindow(void)
{
  if(!m_serialPortWindow)
    m_serialPortWindow = new glitch_serial_port_window(this);

  m_serialPortWindow->showNormal();
  m_serialPortWindow->activateWindow();
  m_serialPortWindow->raise();
}

void glitch_ui::slotShowSourcePreview(void)
{
  if(m_currentView)
    m_currentView->showSourcePreview();
}

void glitch_ui::slotShowTearOffMenu(void)
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

void glitch_ui::slotSpecialTools(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  if(!m_currentView || !m_currentView->scene())
    return;

  auto type(action->data().toString());

  if(type == "adjust-all-sizes")
    m_currentView->slotAllWidgetsAdjustSize();
  else if(type == "adjust-sizes")
    m_currentView->scene()->slotSelectedWidgetsAdjustSize();
  else if(type == "compress-widgets")
    m_currentView->scene()->slotSelectedWidgetsCompress();
  else if(type == "disconnect-widgets")
    m_currentView->scene()->slotSelectedWidgetsDisconnect();
  else if(type == "fonts")
    m_currentView->slotFonts();
  else if(type == "lock-positions")
    m_currentView->scene()->slotSelectedWidgetsLock();
  else if(type == "widget-properties")
    m_currentView->slotSelectedWidgetsProperties();
}

void glitch_ui::slotZoom(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action || !m_currentView)
    return;

  if(action == m_ui.action_Zoom_In)
    m_currentView->zoom(1);
  else if(action == m_ui.action_Zoom_Out)
    m_currentView->zoom(-1);
  else
    m_currentView->zoom(0);
}
