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

#ifndef _glitch_canvas_settings_h_
#define _glitch_canvas_settings_h_

#include <QColorDialog>
#include <QGraphicsView>
#include <QPointer>
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
    m_parent = qobject_cast<QWidget *> (parent);
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
	  pushButton->setText(index.data().toString().remove('&').trimmed());
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

  QPointer<QWidget> m_parent;
  mutable QModelIndex m_index;

 private slots:
  void slotSelectColor(void)
  {
    QColorDialog dialog(m_parent);

    dialog.setCurrentColor
      (QColor(m_index.data().toString().remove('&').trimmed()));
    dialog.setOption(QColorDialog::ShowAlphaChannel, true);
#ifdef Q_OS_ANDROID
    dialog.showMaximized();
#endif
    QApplication::processEvents();

    if(dialog.exec() == QDialog::Accepted &&
       m_index.isValid() &&
       m_index.model())
      {
	auto model = const_cast<QAbstractItemModel *> (m_index.model());
	auto sortingEnabled = false;
	auto table = qobject_cast<QTableWidget *>
	  (m_index.model()->parent());

	if(table)
	  {
	    sortingEnabled = table->isSortingEnabled();
	    table->setSortingEnabled(false);
	  }

	if(model)
	  {
	    model->setData
	      (m_index, dialog.selectedColor(), Qt::DecorationRole);
	    model->setData
	      (m_index, dialog.selectedColor().name(QColor::HexArgb));
	  }

	if(table)
	  table->setSortingEnabled(sortingEnabled);
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
    MAXIMIZE_EDIT_WINDOWS,
    PROJECT_BOARD,
    PROJECT_COMMUNICATIONS_PORT,
    PROJECT_IDE,
    REDO_UNDO_STACK_SIZE,
    SAVE_PERIODICALLY,
    SELECTION_COLOR,
    SHOW_CANVAS_DOTS,
    SHOW_CANVAS_GRIDS,
    SHOW_ORDER_INDICATORS,
    SHOW_PREVIEW,
    TABBED_EDIT_WINDOWS,
    TAB_POSITION_INDEX,
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
  QString projectBoard(void) const;
  QString projectCommunicationsPort(void) const;
  QString projectIDE(void) const;
  QString wireType(void) const;
  QTabWidget::TabPosition tabPosition(void) const;
  bool generatePeriodically(void) const;
  bool generateSourceViewPeriodically(void) const;
  bool maximizeEditWindows(void) const;
  bool save(QString &error) const;
  bool savePeriodically(void) const;
  bool showCanvasDots(void) const;
  bool showCanvasGrids(void) const;
  bool showOrderIndicators(void) const;
  bool showPreview(void) const;
  bool tabbedEditWindows(void) const;
  double wireWidth(void) const;
  int adjustedTabPositionIndexFromIndex(const int index) const;
  int redoUndoStackSize(void) const;
  void prepare(const QString &fileName);
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
  void setShowPreview(const bool state);
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
  void alterDatabase(void) const;
  void closeEvent(QCloseEvent *event);
  void prepareKeywordColors(const QString &text);
  void prepareWidgets(void);
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

  void slotCommunicationsPortRefresh(void);
  void slotResetSourceViewKeywords(void);
  void slotSelectColor(void);
  void slotSelectProjectIDE(void);
  void slotSpecialCopy(void);
  void slotSpecialPaste(void);
  void slotTimerTimeout(void);

 signals:
  void accepted(const bool undo);
};

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
inline size_t
#else
inline uint
#endif
qHash(const glitch_canvas_settings::Settings &key,
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
      size_t seed)
#else
      uint seed)
#endif
{
  return ::qHash(static_cast<uint> (key), seed);
}

#endif
