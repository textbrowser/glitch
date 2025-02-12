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

#include <QFileInfo>
#include <QKeyEvent>
#include <QScrollBar>

#include "glitch-recent-diagrams-view.h"

glitch_recent_diagrams_view::glitch_recent_diagrams_view(QWidget *parent):
  QGraphicsView(parent)
{
  m_menuAction = new QAction
    (QIcon(":/recent.png"), tr("Recent Diagrams"), this);
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

QAction *glitch_recent_diagrams_view::menuAction(void) const
{
  return m_menuAction;
}

QStringList glitch_recent_diagrams_view::selectedFileNames(void) const
{
  QMap<QString, char> map;

  foreach(auto item, scene()->selectedItems())
    if(item)
      map[item->data(Qt::UserRole).toString()] = 0;

  return map.keys();
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

void glitch_recent_diagrams_view::keyPressEvent(QKeyEvent *event)
{
  QGraphicsView::keyPressEvent(event);

  if(event)
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
      slotOpen();
}

void glitch_recent_diagrams_view::populate
(const QVectorQPairQImageQString &vector)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const fileNames(selectedFileNames());
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

  if(missing.size() != QSize(372, 240))
    missing = missing.scaled
      (372, 240, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  for(int i = 0; i < vector.size(); i++)
    {
      auto effect = new QGraphicsDropShadowEffect();
      auto item = new glitch_recent_diagrams_view_item
	(QPixmap::fromImage(vector.at(i).first));

      connect(item,
	      SIGNAL(remove(QGraphicsItem *)),
	      this,
	      SLOT(slotRemove(QGraphicsItem *)));
      connect(item,
	      SIGNAL(remove(const QString &)),
	      this,
	      SIGNAL(remove(const QString &)));
      effect->setBlurRadius(10.0);
      effect->setColor(QColor(Qt::gray));
      effect->setOffset(2.5, 2.5);

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
      item->setFlag(QGraphicsItem::ItemIsSelectable, true);
      item->setGraphicsEffect(effect);
      item->setPixmap
	(QFileInfo(vector.at(i).second).isReadable() ?
	 item->pixmap() : missing);
      item->setSelected(fileNames.contains(vector.at(i).second));
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

void glitch_recent_diagrams_view::slotOpen(void)
{
  auto const list(selectedFileNames());

  if(list.isEmpty())
    emit openDiagram();
  else
    foreach(auto const &fileName, list)
      emit openDiagram(fileName);
}

void glitch_recent_diagrams_view::slotRemove(QGraphicsItem *item)
{
  if(!item)
    return;

  scene()->removeItem(item);
  delete item;
}
