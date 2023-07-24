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

#ifndef _glitch_canvas_settings_h_
#define _glitch_canvas_settings_h_

#include <QColorDialog>
#include <QGraphicsView>
#include <QStyledItemDelegate>
#include <QTimer>

#include "ui_glitch-canvas-settings.h"

class glitch_canvas_settings_item_delegate: public QStyledItemDelegate
{
  Q_OBJECT

 public:
  glitch_canvas_settings_item_delegate(QObject *parent):
    QStyledItemDelegate(parent)
  {
  }

  QWidget *createEditor(QWidget *parent,
			const QStyleOptionViewItem &option,
			const QModelIndex &index) const
  {
    switch(index.column())
      {
      case 1:
	{
	  auto pushButton = new QPushButton(parent);

	  connect(pushButton,
		  SIGNAL(clicked(void)),
		  this,
		  SLOT(slotSelectColor(void))
#ifdef Q_OS_MACOS
		  , Qt::QueuedConnection
#endif
		  );
	  m_index = index;
	  pushButton->setText(index.data().toString().trimmed());
	  return pushButton;
	}
      default:
	{
	  break;
	}
      }

    return QStyledItemDelegate::createEditor(parent, option, index);
  }

 private:
  void setModelData(QWidget *editor,
		    QAbstractItemModel *model,
		    const QModelIndex &index) const
  {
    auto pushButton = qobject_cast<QPushButton *> (editor);

    if(model && pushButton)
      {
	pushButton->setText(model->data(index).toString().trimmed());
	return;
      }

    QStyledItemDelegate::setModelData(editor, model, index);
  }

  mutable QModelIndex m_index;

 private slots:
  void slotSelectColor(void)
  {
    auto pushButton = qobject_cast<QPushButton *> (sender());

    if(!pushButton)
      return;

    QColorDialog dialog(pushButton);

    dialog.setCurrentColor(QColor(pushButton->text().remove('&')));

    if(dialog.exec() == QDialog::Accepted)
      {
	if(m_index.isValid() && m_index.model())
	  {
	    auto sortingEnabled = false;
	    auto table = qobject_cast<QTableWidget *>
	      (m_index.model()->parent());

	    if(table)
	      {
		sortingEnabled = table->isSortingEnabled();
		table->setSortingEnabled(false);
	      }

	    const_cast<QAbstractItemModel *> (m_index.model())->setData
	      (m_index, dialog.selectedColor(), Qt::DecorationRole);
	    const_cast<QAbstractItemModel *> (m_index.model())->setData
	      (m_index, dialog.selectedColor().name());

	    if(table)
	      table->setSortingEnabled(sortingEnabled);
	  }

	pushButton->setText(dialog.selectedColor().name());
	emit commitData(pushButton);
	emit closeEditor(pushButton); // Order is crucial.
      }
  }
};

class glitch_canvas_settings: public QDialog
{
  Q_OBJECT

 public:
  enum class Pages
  {
    Canvas = 0,
    Project = 1,
    UndoStack
  };

  enum class Settings
  {
    CANVAS_BACKGROUND_COLOR = 0,
    CANVAS_NAME,
    CATEGORIES_ICON_SIZE,
    DOTS_GRIDS_COLOR,
    GENERATE_PERIODICALLY,
    GENERATE_SOURCE_VIEW_PERIODICALLY,
    KEYWORD_COLORS,
    LOCK_COLOR,
    PROJECT_IDE,
    REDO_UNDO_STACK_SIZE,
    SELECTION_COLOR,
    SHOW_CANVAS_DOTS,
    SHOW_CANVAS_GRIDS,
    SHOW_ORDER_INDICATORS,
    VIEW_UPDATE_MODE,
    WIRE_COLOR,
    WIRE_TYPE,
    WIRE_WIDTH
  };

  glitch_canvas_settings(QWidget *parent);
  ~glitch_canvas_settings();
  QColor canvasBackgroundColor(void) const;
  QColor dotsGridsColor(void) const;
  QColor lockColor(void) const;
  QColor selectionColor(void) const;
  QColor wireColor(void) const;
  QGraphicsView::ViewportUpdateMode viewportUpdateMode(void) const;
  QHash<glitch_canvas_settings::Settings, QVariant> settings(void) const;
  QMap<QString, QColor> keywordColorsAsMap(void) const;
  QString categoriesIconSize(void) const;
  QString keywordColorsAsString(void) const;
  QString name(void) const;
  QString outputFile(void) const;
  QString projectIDE(void) const;
  QString wireType(void) const;
  bool generatePeriodically(void) const;
  bool generateSourceViewPeriodically(void) const;
  bool save(QString &error) const;
  bool showCanvasDots(void) const;
  bool showCanvasGrids(void) const;
  bool showOrderIndicators(void) const;
  double wireWidth(void) const;
  int redoUndoStackSize(void) const;
  void prepare(void);
  void setFileName(const QString &fileName);
  void setCategoriesIconSize(const QString &text);
  void setName(const QString &name);
  void setOutputFileExtension(const QString &extension);
  void setProjectIDE(const QString &filename);
  void setProjectKeywords(const QStringList &list);
  void setRedoUndoStackSize(const int value);
  void setSettings
    (const QHash<glitch_canvas_settings::Settings, QVariant> &hash);
  void setShowCanvasDots(const bool state);
  void setShowCanvasGrids(const bool state);
  void setShowOrderIndicators(const bool state);
  void setViewportUpdateMode(const QGraphicsView::ViewportUpdateMode mode);
  void setWireType(const QString &string);
  void setWireWidth(const double value);
  void showPage(const Pages page);

 private:
  enum class Limits
  {
    NAME_MAXIMUM_LENGTH = 100
  };

  QHash<glitch_canvas_settings::Settings, QVariant> m_settings;
  QString m_fileName;
  QString m_outputFileExtension;
  QTimer m_timer;
  Ui_glitch_canvas_settings m_ui;
  glitch_canvas_settings_item_delegate *m_itemDelegate;
  QString defaultName(void) const;
  QString keywordColorsFromTableAsString(void) const;
  void closeEvent(QCloseEvent *event);
  void prepareKeywordColors(const QString &text);
  void showEvent(QShowEvent *event);

 private slots:
  void accept(void);

#ifdef Q_OS_ANDROID
  void reject(void)
  {
    hide();
    m_timer.stop();
    setResult(QDialog::Rejected);
  }
#endif

  void slotResetSourceViewKeywords(void);
  void slotSelectColor(void);
  void slotSelectProjectIDE(void);
  void slotSpecialCopy(void);
  void slotSpecialPaste(void);
  void slotTimerTimeout(void);

 signals:
  void accepted(const bool undo);
};

inline uint qHash(const glitch_canvas_settings::Settings &key, uint seed)
{
  return ::qHash(static_cast<uint> (key), seed);
}

#endif
