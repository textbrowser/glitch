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

#include <QDesktopServices>
#include <QDir>
#include <QFontDatabase>
#ifdef GLITCH_PDF_SUPPORTED
#include <QPdfDocument>
#include <QPdfView>
#endif
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QScrollBar>
#include <QtDebug>

#include "glitch-documentation.h"
#include "glitch-variety.h"

glitch_documentation::glitch_documentation(QWidget *parent):
  glitch_documentation("", parent)
{
}

glitch_documentation::glitch_documentation
(const QString &fileName, QWidget *parent):QMainWindow(parent)
{
  m_openExternalLinks = false;
#ifdef GLITCH_PDF_SUPPORTED
  if(fileName.toLower().trimmed().endsWith(".pdf"))
    {
      m_pdfDocument = new QPdfDocument(this);
      m_pdfDocument->load(fileName);
      m_pdfView = new QPdfView(this);
      m_pdfView->setDocument(m_pdfDocument);
      m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);
    }
  else
    {
      m_pdfDocument = nullptr;
      m_pdfView = nullptr;
    }
#endif
  m_ui.setupUi(this);
  m_ui.action_Close->setIcon(QIcon(":/close.png"));
  m_ui.action_Find->setIcon(QIcon(":/find.png"));
  m_ui.action_Print->setIcon(QIcon(":/print.png"));
  m_ui.close->setIcon(QIcon(":/close.png"));
  m_ui.find->setPlaceholderText(tr("Find"));
  m_ui.next->setIcon(QIcon(":/next.png"));
  m_ui.previous->setIcon(QIcon(":/previous.png"));

  if(fileName.toLower().trimmed().endsWith(".html"))
    m_ui.text->setSource(QUrl(fileName));

  m_originalFindPalette = m_ui.find->palette();
#ifdef Q_OS_ANDROID
  connect(m_ui.action_Close,
	  &QAction::triggered,
	  this,
	  &glitch_documentation::hide);
#else
  connect(m_ui.action_Close,
	  &QAction::triggered,
	  this,
	  &glitch_documentation::close);
#endif
  connect(m_ui.action_Find,
	  &QAction::triggered,
	  this,
	  &glitch_documentation::slotFind);
  connect(m_ui.action_Print,
          &QAction::triggered,
          this,
          &glitch_documentation::slotPrint);
#ifdef Q_OS_ANDROID
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_documentation::hide);
#else
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_documentation::close);
#endif
  connect(m_ui.find,
	  &QLineEdit::returnPressed,
	  this,
	  &glitch_documentation::slotFindText);
  connect(m_ui.next,
	  &QPushButton::clicked,
	  this,
	  &glitch_documentation::slotFindText);
  connect(m_ui.previous,
	  &QPushButton::clicked,
	  this,
	  &glitch_documentation::slotFindText);
  connect(m_ui.find,
	  SIGNAL(textEdited(const QString &)),
	  this,
	  SLOT(slotFindText(void)));
  connect(m_ui.text,
	  SIGNAL(anchorClicked(const QUrl &)),
	  this,
	  SLOT(slotAnchorClicked(const QUrl &)));
#ifdef GLITCH_PDF_SUPPORTED
  if(m_pdfView)
    {
      m_ui.action_Find->setEnabled(false);
      m_ui.action_Print->setEnabled(false);
      m_ui.frame->setVisible(false);
      m_ui.stack->addWidget(m_pdfView);
      m_ui.stack->setCurrentIndex(1);
    }
#endif
}

glitch_documentation::glitch_documentation
(const QUrl &url, QWidget *parent):glitch_documentation(url.toString(), parent)
{
}

glitch_documentation::~glitch_documentation()
{
}

void glitch_documentation::setAllowOpeningOfExternalLinks(const bool state)
{
  m_openExternalLinks = state;
}

void glitch_documentation::setHtml(const QString &html)
{
  m_ui.text->setHtml(html);
}

void glitch_documentation::setPlainText(const QString &text)
{
  auto font(m_ui.text->font());
  auto h = m_ui.text->horizontalScrollBar() ?
    m_ui.text->horizontalScrollBar()->value() : 0;
  auto v = m_ui.text->verticalScrollBar() ?
    m_ui.text->verticalScrollBar()->value() : 0;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  foreach(auto const &str, QFontDatabase::families())
#else
  foreach(auto const &str, QFontDatabase().families())
#endif
    if(str.toLower().contains("courier"))
      {
	font.setFamily(str);
	break;
      }

  m_ui.text->setFont(font);
  m_ui.text->setPlainText(text);

  if(m_ui.text->horizontalScrollBar())
    m_ui.text->horizontalScrollBar()->setValue(h);

  if(m_ui.text->verticalScrollBar())
    m_ui.text->verticalScrollBar()->setValue(v);
}

void glitch_documentation::show(void)
{
  QMainWindow::showNormal();
  QMainWindow::activateWindow();
  QMainWindow::raise();
}

void glitch_documentation::slotAnchorClicked(const QUrl &url)
{
  if(url.scheme().toLower().trimmed() == "qrc")
    m_ui.text->setSource(url);
  else if(m_openExternalLinks)
    QDesktopServices::openUrl(url);
}

void glitch_documentation::slotFind(void)
{
  m_ui.find->setFocus();
  m_ui.find->selectAll();
}

void glitch_documentation::slotFindText(void)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  auto options = QTextDocument::FindFlags();
#else
  QTextDocument::FindFlags options = 0;
#endif

  if(m_ui.previous == qobject_cast<QPushButton *> (sender()))
    options = QTextDocument::FindBackward;

  glitch_variety::searchText
    (m_ui.find, m_ui.text, m_originalFindPalette, options);
}

void glitch_documentation::slotPrint(void)
{
  QApplication::processEvents();

  QPrinter printer(QPrinter::HighResolution);
  QPrintDialog dialog(&printer, this);

  dialog.setWindowIcon(windowIcon());

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_ui.text->print(&printer);
    }
  else
    QApplication::processEvents();
}
