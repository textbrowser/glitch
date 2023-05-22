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

#include <QFile>
#include <QSettings>
#include <QSplitter>
#include <QSqlQuery>
#include <QTextStream>

#include "Arduino/glitch-structures-arduino.h"
#include "glitch-alignment.h"
#include "glitch-docked-container.h"
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
  m_canvasSettings->setOutputFileExtension
    (glitch_view_arduino::projectOutputFileExtension());
  m_canvasSettings->setProjectKeywords(glitch_structures_arduino::keywords());
  m_loopObject = new glitch_object_loop_arduino(this);
  m_loopObject->setCanvasSettings(m_canvasSettings);
  m_setupObject = new glitch_object_setup_arduino(this);
  m_setupObject->setCanvasSettings(m_canvasSettings);

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
  connect(m_loopObject,
	  &glitch_object_loop_arduino::changed,
	  this,
	  &glitch_view_arduino::slotChanged);
  connect(m_loopObject,
	  SIGNAL(dockPropertyEditor(QWidget *)),
	  this,
	  SLOT(slotDockPropertyEditor(QWidget *)));
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
  connect(m_splitter,
	  SIGNAL(splitterMoved(int, int)),
	  this,
	  SLOT(slotSilentSave(void)));
  m_splitter->addWidget(m_arduinoStructures->frame());
  m_splitter->addWidget(m_view);
  m_splitter->addWidget(m_dockedWidgetPropertyEditors);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  m_splitter->setStretchFactor(2, 0);
  slotPreferencesAccepted();
}

glitch_view_arduino::~glitch_view_arduino()
{
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
	      auto properties(query.value(0).toString().trimmed());
	      auto styleSheet(query.value(1).toString().trimmed());
	      auto type(query.value(2).toString().toLower().trimmed());

	      if(type == "arduino-loop")
		{
		  m_loopObject->setProperties(properties.split('&'));
		  m_loopObject->setStyleSheet(styleSheet);
		}
	      else
		{
		  m_setupObject->setProperties(properties.split('&'));
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

      auto code(w->code());

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

void glitch_view_arduino::generateSourceFile(void) const
{
  if(m_canvasSettings->notify())
    {
      m_canvasSettings->showNormal();
      m_canvasSettings->activateWindow();
      m_canvasSettings->raise();
      return;
    }

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

void glitch_view_arduino::removeFunctionName(const QString &name)
{
  m_functionNames.remove(name);
}

void glitch_view_arduino::separate(void)
{
  defaultContextMenu()->deleteLater();

  if(m_tools && m_tools->isVisible())
    {
      setProperty("tools-operation", m_tools->operation());
      m_tools->deleteLater();
      QTimer::singleShot(500, this, &glitch_view_arduino::slotShowTools);
    }

  foreach(auto w, scene()->orderedObjects())
    if(w)
      w->separate();
}

void glitch_view_arduino::showStructures(void)
{
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

void glitch_view_arduino::slotPreferencesAccepted(void)
{
  glitch_view::slotPreferencesAccepted();

  QSettings settings;
  auto state = settings.value
    ("preferences/docked_widget_property_editors", true).toBool();

  m_dockedWidgetPropertyEditors->setVisible(state);
}

void glitch_view_arduino::slotShowStructures(void)
{
  showStructures();
}

void glitch_view_arduino::slotSilentSave(void)
{
  m_properties["splitter_state"] = m_splitter->saveState();
  saveProperties();
}

void glitch_view_arduino::unite(void)
{
  defaultContextMenu()->deleteLater();

  if(m_tools && m_tools->isVisible())
    {
      m_tools->deleteLater();
      QTimer::singleShot(500, this, &glitch_view_arduino::slotShowTools);
    }
}
