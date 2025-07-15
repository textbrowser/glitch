/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMessageAuthenticationCode>
#include <QScrollBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtConcurrent>

#include "glitch-common.h"
#include "glitch-recent-diagrams-view.h"
#include "glitch-variety.h"
#include "glitch-version.h"

auto static s_snapSize = QSize(372, 240);

glitch_recent_diagrams_view::glitch_recent_diagrams_view(QWidget *parent):
  QGraphicsView(parent)
{
  connect(&m_timer,
	  &QTimer::timeout,
	  this,
	  &glitch_recent_diagrams_view::slotPopulateRecentDiagrams);
  connect(this,
	  SIGNAL(recentDiagramsGathered(const QByteArray &,
					const QVectorQPairQImageQString &)),
	  this,
	  SLOT(slotRecentDiagramsGathered(const QByteArray &,
					  const QVectorQPairQImageQString &)));
  m_lastModified = 0;
  m_menuAction = new QAction
    (QIcon(":/recent.png"), tr("Recent Diagrams"), this);
  m_recentFilesFileName = glitch_variety::homePath() +
    QDir::separator() +
    "Glitch" +
    QDir::separator() +
    "glitch_recent_files.db";
  m_timer.start(50);
  setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  setCacheMode(QGraphicsView::CacheNone);
  setDragMode(QGraphicsView::NoDrag);
  setRenderHints(QPainter::Antialiasing |
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
		 QPainter::LosslessImageRendering |
#endif
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
  setRubberBandSelectionMode(Qt::IntersectsItemShape);
  setScene(new QGraphicsScene(this));
  setStyleSheet("QGraphicsView {border: none;}");
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#ifdef Q_OS_MACOS
  // qt.pointer.dispatch: skipping QEventPoint()

  viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
#endif
}

glitch_recent_diagrams_view::~glitch_recent_diagrams_view()
{
  m_gatherRecentDiagramsFuture.cancel();
  m_gatherRecentDiagramsFuture.waitForFinished();
  m_timer.stop();
}

QAction *glitch_recent_diagrams_view::menuAction(void) const
{
  return m_menuAction;
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void glitch_recent_diagrams_view::enterEvent(QEnterEvent *event)
#else
void glitch_recent_diagrams_view::enterEvent(QEvent *event)
#endif
{
  QGraphicsView::enterEvent(event);
  setFocus();
}

void glitch_recent_diagrams_view::gatherRecentDiagrams
(const QByteArray &digest, const QString &fileName)
{
  auto const value = QFileInfo(fileName).lastModified().toMSecsSinceEpoch();

  if(m_lastModified.fetchAndAddOrdered(0) < value)
    m_lastModified.fetchAndStoreOrdered(value);
  else
    return;

  QMessageAuthenticationCode sha
    (QCryptographicHash::Sha3_512,
     QByteArray("Glitch") + QByteArray(GLITCH_VERSION_STRING));
  QString connectionName("");
  QVectorQPairQImageQString vector;

  {
    auto db(glitch_common::sqliteDatabase());

    connectionName = db.connectionName();
    db.setDatabaseName(fileName);

    if(db.open())
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec("SELECT file_name, image FROM glitch_recent_files "
		      "ORDER BY 1 LIMIT 100"))
	  while(m_gatherRecentDiagramsFuture.isCanceled() == false &&
		query.next())
	    {
	      QImage image;

	      if(image.loadFromData(QByteArray::
				    fromBase64(query.value(1).toByteArray()),
				    "PNG"))
		{
		  QFileInfo const fileInfo(query.value(0).toString());

		  sha.addData(query.value(0).toByteArray() +
			      query.value(1).toByteArray());
		  vector << QPair<QImage, QString>
		    (image, fileInfo.absoluteFilePath());
		}
	    }
      }

    db.close();
  }

  glitch_common::discardDatabase(connectionName);

  if(digest != sha.result())
    emit recentDiagramsGathered(sha.result(), vector);
}

void glitch_recent_diagrams_view::keyPressEvent(QKeyEvent *event)
{
  QGraphicsView::keyPressEvent(event);

  if(event)
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
      emit openDiagram();
}

void glitch_recent_diagrams_view::populate
(const QVectorQPairQImageQString &vector)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const hValue = horizontalScrollBar()->value();
  auto const vValue = verticalScrollBar()->value();

  resetTransform();
  scene()->clear();
  setSceneRect(0.0, 0.0, 1.0, 1.0);

  QPixmap static missing(":/missing-image.png", "PNG");
  const int columns = 3;
  const qreal offseth = 15.0;
  const qreal offsetw = 15.0;
  int columnIndex = 0;
  int rowIndex = 0;

  if(missing.size() != s_snapSize)
    missing = missing.scaled
      (s_snapSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  for(int i = 0; i < vector.size(); i++)
    {
      auto item = new glitch_recent_diagrams_view_item
	(QPixmap::fromImage(vector.at(i).first));

      connect(item,
	      SIGNAL(open(const QString &)),
	      this,
	      SIGNAL(openDiagram(const QString &)));
      connect(item,
	      SIGNAL(remove(QGraphicsItem *)),
	      this,
	      SLOT(slotRemove(QGraphicsItem *)));
      connect(item,
	      SIGNAL(remove(const QString &)),
	      this,
	      SIGNAL(remove(const QString &)));

      auto const height = 25.0 + item->boundingRect().size().height();
      auto const width = 25.0 + item->boundingRect().size().width();

      if(rowIndex == 0)
	item->setPos(columnIndex * width + offsetw, offseth);
      else
	item->setPos
	  (columnIndex * width + offsetw, height * rowIndex + offseth);

      columnIndex += 1;
      item->setData(Qt::UserRole, vector.at(i).second);
      item->setFileName(vector.at(i).second);
      item->setPixmap
	(QFileInfo(vector.at(i).second).isReadable() ?
	 item->pixmap() : missing);
      item->setToolTip(vector.at(i).second);
      scene()->addItem(item);

      if(columnIndex >= columns)
	{
	  columnIndex = 0;
	  rowIndex += 1;
	}
    }

  auto rect(scene()->itemsBoundingRect());

  rect.setHeight(offseth + rect.height());
  rect.setX(0.0);
  rect.setY(0.0);
  horizontalScrollBar()->setValue(hValue);
  setSceneRect(rect);
  verticalScrollBar()->setValue(vValue);
  QApplication::restoreOverrideCursor();
}

void glitch_recent_diagrams_view::slotPopulateRecentDiagrams(void)
{
  if(m_gatherRecentDiagramsFuture.isFinished())
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    m_gatherRecentDiagramsFuture = QtConcurrent::run
      (this,
       &glitch_recent_diagrams_view::gatherRecentDiagrams,
       m_digest,
       m_recentFilesFileName);
#else
    m_gatherRecentDiagramsFuture = QtConcurrent::run
      (&glitch_recent_diagrams_view::gatherRecentDiagrams,
       this,
       m_digest,
       m_recentFilesFileName);
#endif
}

void glitch_recent_diagrams_view::slotRecentDiagramsGathered
(const QByteArray &digest, const QVectorQPairQImageQString &vector)
{
  m_digest = digest;
  populate(vector);
}

void glitch_recent_diagrams_view::slotRemove(QGraphicsItem *item)
{
  if(!item)
    return;

  scene()->removeItem(item);
  delete item;
}
