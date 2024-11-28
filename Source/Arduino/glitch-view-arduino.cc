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
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QSplitter>
#include <QSqlQuery>
#include <QTextBrowser>
#include <QTextStream>

#include "Arduino/glitch-object-function-arduino.h"
#include "Arduino/glitch-structures-arduino.h"
#include "glitch-alignment.h"
#include "glitch-canvas-preview.h"
#include "glitch-docked-container.h"
#include "glitch-find-objects.h"
#include "glitch-graphicsview.h"
#include "glitch-object-loop-arduino.h"
#include "glitch-object-setup-arduino.h"
#include "glitch-proxy-widget.h"
#include "glitch-scene.h"
#include "glitch-view-arduino.h"

glitch_view_arduino::glitch_view_arduino
(const QString &fileName,
 const QString &name,
 const bool fromFile,
 const glitch_common::ProjectTypes projectType,
 QWidget *parent):glitch_view(fileName, name, projectType, parent)
{
  Q_UNUSED(fromFile);
  m_arduinoStructures = new glitch_structures_arduino(this);
  m_arduinoStructures->prepareCategories();
  m_canvasSettings->setOutputFileExtension
    (glitch_view_arduino::projectOutputFileExtension());
  m_canvasSettings->setProjectKeywords(glitch_structures_arduino::keywords());
  m_loopObject = new glitch_object_loop_arduino(this);
  m_loopObject->setCanvasSettings(m_canvasSettings);
  m_loopObject->setUndoStack(m_undoStack);
  m_setupObject = new glitch_object_setup_arduino(this);
  m_setupObject->setCanvasSettings(m_canvasSettings);
  m_setupObject->setUndoStack(m_undoStack);

  glitch_proxy_widget *proxy = nullptr;

  proxy = new glitch_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsMovable, false);
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_loopObject);
  proxy->resize(m_loopObject->size());
  m_loopObject->setProxy(proxy);
  m_scene->addItem(proxy);
  proxy->setPos(QPointF(15.0, 15.0));
  proxy = new glitch_proxy_widget();
  proxy->setFlag(QGraphicsItem::ItemIsMovable, false);
  proxy->setFlag(QGraphicsItem::ItemIsSelectable, true);
  proxy->setWidget(m_setupObject);
  proxy->resize(m_setupObject->size());
  m_scene->addItem(proxy);
  m_setupObject->setProxy(proxy);
  proxy->setPos(QPointF(m_loopObject->width() + 45.0, 15.0));
  connect(&m_ideProcess,
	  &QProcess::readyReadStandardError,
	  this,
	  &glitch_view_arduino::slotIDEProcessOutput);
  connect(&m_ideProcess,
	  &QProcess::readyReadStandardOutput,
	  this,
	  &glitch_view_arduino::slotIDEProcessOutput);
  connect(m_bottomSplitter,
	  SIGNAL(splitterMoved(int, int)),
	  this,
	  SLOT(slotSilentSave(void)));
  connect(m_loopObject,
	  &glitch_object_loop_arduino::changed,
	  this,
	  &glitch_view_arduino::slotChanged);
  connect(m_loopObject,
	  SIGNAL(dockPropertyEditor(QWidget *)),
	  this,
	  SLOT(slotDockPropertyEditor(QWidget *)));
  connect(m_loopObject,
	  SIGNAL(processCommand(const QString &, const QStringList &)),
	  this,
	  SLOT(slotProcessCommand(const QString &, const QStringList &)));
  connect(m_loopObject,
	  SIGNAL(showEditWindow(QMainWindow *)),
	  this,
	  SLOT(slotShowEditWindow(QMainWindow *)));
  connect(m_rightSplitter,
	  SIGNAL(splitterMoved(int, int)),
	  this,
	  SLOT(slotSilentSave(void)));
  connect(m_scene,
	  SIGNAL(functionAdded(const QString &, const bool)),
	  this,
	  SLOT(slotFunctionAdded(const QString &, const bool)));
  connect(m_scene,
	  SIGNAL(functionDeleted(const QString &)),
	  this,
	  SLOT(slotFunctionDeleted(const QString &)));
  connect(m_setupObject,
	  &glitch_object_setup_arduino::changed,
	  this,
	  &glitch_view_arduino::slotChanged);
  connect(m_setupObject,
	  SIGNAL(dockPropertyEditor(QWidget *)),
	  this,
	  SLOT(slotDockPropertyEditor(QWidget *)));
  connect(m_setupObject,
	  SIGNAL(processCommand(const QString &, const QStringList &)),
	  this,
	  SLOT(slotProcessCommand(const QString &, const QStringList &)));
  connect(m_setupObject,
	  SIGNAL(showEditWindow(QMainWindow *)),
	  this,
	  SLOT(slotShowEditWindow(QMainWindow *)));
  connect(m_splitter,
	  SIGNAL(splitterMoved(int, int)),
	  this,
	  SLOT(slotSilentSave(void)));
  connect(m_ui.splitter,
	  SIGNAL(splitterMoved(int, int)),
	  this,
	  SLOT(slotSilentSave(void)));
  m_rightSplitter->addWidget(m_dockedWidgetPropertyEditors);
  m_rightSplitter->addWidget(m_canvasPreview);
  m_rightSplitter->setStretchFactor(0, 1);
  m_rightSplitter->setStretchFactor(1, 0);
  m_splitter->addWidget(m_arduinoStructures->frame());
  m_splitter->addWidget(m_view);
  m_splitter->addWidget(m_rightSplitter);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  m_splitter->setStretchFactor(2, 0);
  m_ui.splitter->setSizes(QList<int> () << 2 << 1);
  m_ui.splitter->setStretchFactor(0, 1);
  m_ui.splitter->setStretchFactor(1, 0);
  slotPreferencesAccepted();
}

glitch_view_arduino::~glitch_view_arduino()
{
  m_ideProcess.kill();
  m_ideProcess.waitForFinished();
}

QString glitch_view_arduino::nextUniqueFunctionName(void) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString name("function_0()");
  qint64 i = 0;

  while(m_functionNames.contains(name))
    {
      i += 1;
      name = QString("function_%1()").arg(i);
    }

  QApplication::restoreOverrideCursor();
  return name;
}

QString glitch_view_arduino::projectOutputFileExtension(void) const
{
  return ".ino";
}

QString glitch_view_arduino::source(void) const
{
  QString string("");
  QTextStream stream(&string);

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  generateSource(stream);
  QApplication::restoreOverrideCursor();
  return string;
}

QStringList glitch_view_arduino::keywords(void) const
{
  return glitch_structures_arduino::keywords();
}

bool glitch_view_arduino::containsFunctionName(const QString &name) const
{
  return m_functionNames.contains(name);
}

bool glitch_view_arduino::open(const QString &fileName, QString &error)
{
  auto ok = glitch_view::open(fileName, error);

  if(!ok)
    return ok;

  QString connectionName("");

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if((ok = db.open()))
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec(QString("SELECT "
			      "SUBSTR(properties, 1, %1), "
			      "SUBSTR(stylesheet, 1, %2), "
			      "SUBSTR(type, 1, 100) "
			      "FROM objects WHERE "
			      "type IN ('arduino-loop', 'arduino-setup')").
		      arg(static_cast<int> (glitch_view::Limits::
					    PROPERTIES_MAXIMUM_LENGTH)).
		      arg(static_cast<int> (glitch_view::Limits::
					    STYLESHEET_MAXIMUM_LENGTH))))
	  while(query.next())
	    {
	      auto const properties(query.value(0).toString().trimmed());
	      auto const styleSheet(query.value(1).toString().trimmed());
	      auto const type(query.value(2).toString().toLower().trimmed());

	      if(type == "arduino-loop")
		{
		  m_loopObject->setProperties
		    (glitch_object::splitPropertiesAmpersand(properties));
		  m_loopObject->setStyleSheet(styleSheet);
		}
	      else
		{
		  m_setupObject->setProperties
		    (glitch_object::splitPropertiesAmpersand(properties));
		  m_setupObject->setStyleSheet(styleSheet);
		}
	    }
	else
	  {
	    error = tr("An error occurred while accessing the objects table.");
	    ok = false;
	  }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);
  return ok;
}

bool glitch_view_arduino::selectedCanCopyOrDelete(void) const
{
  auto const size = selectedObjects().size();

  if(m_loopObject->proxy() && m_setupObject->proxy())
    {
      auto const b1 = m_loopObject->proxy()->isSelected();
      auto const b2 = m_setupObject->proxy()->isSelected();

      if(b1 && b2)
	return size > 2;
      else if(b1 || b2)
	return size > 1;
      else
	return size > 0;
    }
  else
    return size > 0;
}

glitch_object_loop_arduino *glitch_view_arduino::loopObject(void) const
{
  return m_loopObject;
}

glitch_object_setup_arduino *glitch_view_arduino::setupObject(void) const
{
  return m_setupObject;
}

void glitch_view_arduino::consumeFunctionName(const QString &name)
{
  if(!name.trimmed().isEmpty())
    m_functionNames[name] = '0';
}

void glitch_view_arduino::generateSource(QTextStream &stream) const
{
  glitch_view::generateSource(stream);

  foreach(auto w, scene()->orderedObjects())
    {
      if(!w || !w->shouldPrint())
	continue;
      else if(m_loopObject == w || m_setupObject == w)
	/*
	** The loop() and setup() methods are processed later.
	*/

	continue;

      auto const code(w->code());

      if(!code.trimmed().isEmpty())
	stream << code
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	       << endl
	       << endl;
#else
               << Qt::endl
	       << Qt::endl;
#endif
    }

  stream << m_loopObject->code()
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	 << endl
#else
	 << Qt::endl
#endif
	 << m_setupObject->code();
}

void glitch_view_arduino::generateSourceClipboard(void) const
{
  auto clipboard = QApplication::clipboard();

  if(clipboard)
    clipboard->setText(source());
}

void glitch_view_arduino::generateSourceFile(void) const
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  glitch_view::generateSourceFile();

  QFile file(m_canvasSettings->outputFile());

  if(file.open(QIODevice::WriteOnly))
    {
      QTextStream stream(&file);

      generateSource(stream);
    }

  QApplication::restoreOverrideCursor();
}

void glitch_view_arduino::openFunction(const QString &name) const
{
  foreach(auto w, scene()->objects())
    {
      auto f = qobject_cast<glitch_object_function_arduino *> (w);

      if(f && f->name() == name)
	{
	  f->showEditWindow();
	  break;
	}
    }
}

void glitch_view_arduino::removeFunctionName(const QString &name)
{
  m_functionNames.remove(name);
}

void glitch_view_arduino::separate(void)
{
  defaultContextMenu()->deleteLater();

  foreach(auto w, scene()->orderedObjects())
    if(w)
      w->separate();

  reparent();
}

void glitch_view_arduino::slotCanvasSettingsChanged(const bool undo)
{
  glitch_view::slotCanvasSettingsChanged(undo);
  m_arduinoStructures->setIconSize(m_canvasSettings->categoriesIconSize());
}

void glitch_view_arduino::slotFunctionAdded(const QString &name,
					    const bool isClone)
{
  if(!isClone)
    consumeFunctionName(name);
}

void glitch_view_arduino::slotFunctionDeleted(const QString &name)
{
  removeFunctionName(name);
}

void glitch_view_arduino::slotIDEProcessOutput(void)
{
  QByteArray bytes;

  do
    {
      bytes = m_ideProcess.readAllStandardError().trimmed();

      if(bytes.length() > 0)
	{
	  if(!QChar(bytes.at(bytes.length() - 1)).isPunct())
	    bytes.append(".");

	  bytes[0] = std::toupper(bytes[0]);
	  m_ideOutput->append(bytes);
	}
    }
  while(bytes.length() > 0);

  do
    {
      bytes = m_ideProcess.readAllStandardOutput().trimmed();

      if(bytes.length() > 0)
	{
	  if(!QChar(bytes.at(bytes.length() - 1)).isPunct())
	    bytes.append(".");

	  bytes[0] = std::toupper(bytes[0]);
	  m_ideOutput->append(bytes);
	}
    }
  while(bytes.length() > 0);
}

void glitch_view_arduino::slotProcessCommand
(const QString &command, const QStringList &arguments)
{
  if(command == "upload")
    upload(arguments);
  else if(command == "verify")
    verify(arguments);
}

void glitch_view_arduino::slotSilentSave(void)
{
  m_properties["bottom_splitter_state"] = m_bottomSplitter->saveState();
  m_properties["main_splitter_state"] = m_ui.splitter->saveState();
  m_properties["right_splitter_state"] = m_rightSplitter->saveState();
  m_properties["splitter_state"] = m_splitter->saveState();
  saveProperties();
}

void glitch_view_arduino::unite(void)
{
  defaultContextMenu()->deleteLater();
  reparent();
}

void glitch_view_arduino::upload(const QStringList &arguments)
{
  auto const fileName(m_canvasSettings->outputFile().trimmed());

  if(fileName.isEmpty())
    {
      m_ideOutput->append(tr("Empty program output file name."));
      return;
    }

  auto const processFileName(m_canvasSettings->projectIDE().trimmed());

  if(QFileInfo(processFileName).isExecutable() == false)
    {
      m_ideOutput->append(tr("The IDE is not an executable."));
      return;
    }
  else if(processFileName.isEmpty())
    {
      m_ideOutput->append(tr("Empty IDE process file name."));
      return;
    }

  if(m_ideProcess.state() != QProcess::NotRunning)
    {
      m_ideOutput->append(tr("Process running. Please wait."));
      return;
    }

  m_ideOutput->append(tr("Creating %1.").arg(fileName));
  generateSourceFile();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ideOutput->append(tr("Uploading %1.").arg(fileName));
  m_ideProcess.start
    (processFileName, QStringList() << "--upload" << arguments << fileName);
  QApplication::restoreOverrideCursor();
}

void glitch_view_arduino::verify(const QStringList &arguments)
{
  auto const fileName(m_canvasSettings->outputFile().trimmed());

  if(fileName.isEmpty())
    {
      m_ideOutput->append(tr("Empty program output file name."));
      return;
    }

  auto const processFileName(m_canvasSettings->projectIDE().trimmed());

  if(QFileInfo(processFileName).isExecutable() == false)
    {
      m_ideOutput->append(tr("The IDE is not an executable."));
      return;
    }
  else if(processFileName.isEmpty())
    {
      m_ideOutput->append(tr("Empty IDE process file name."));
      return;
    }

  if(m_ideProcess.state() != QProcess::NotRunning)
    {
      m_ideOutput->append(tr("Process running. Please wait."));
      return;
    }

  m_ideOutput->append(tr("Creating %1.").arg(fileName));
  generateSourceFile();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ideOutput->append(tr("Verifying %1.").arg(fileName));
  m_ideProcess.start
    (processFileName, QStringList() << "--verify" << arguments << fileName);
  QApplication::restoreOverrideCursor();
}
