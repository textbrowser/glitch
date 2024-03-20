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

#include <QShortcut>

#include "glitch-misc.h"
#include "glitch-object.h"
#include "glitch-source-preview.h"
#include "glitch-syntax-highlighter.h"

glitch_source_preview::glitch_source_preview(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.close->setShortcut(tr("Ctrl+W"));
  m_ui.next->setIcon(QIcon(":/next.png"));
  m_ui.previous->setIcon(QIcon(":/previous.png"));
  m_originalFindPalette = m_ui.find->palette();
  m_syntaxHighlighter = new glitch_syntax_highlighter(m_ui.text->document());
  new QShortcut(tr("Ctrl+F"), this, SLOT(slotFind(void)));
  connect(m_ui.find,
	  &QLineEdit::returnPressed,
	  this,
	  &glitch_source_preview::slotFindText);
  connect(m_ui.next,
	  &QPushButton::clicked,
	  this,
	  &glitch_source_preview::slotFindText);
  connect(m_ui.previous,
	  &QPushButton::clicked,
	  this,
	  &glitch_source_preview::slotFindText);
  connect(m_ui.find,
	  SIGNAL(textEdited(const QString &)),
	  this,
	  SLOT(slotFindText(void)));
  setWindowModality(Qt::NonModal);
}

glitch_source_preview::~glitch_source_preview()
{
}

void glitch_source_preview::setKeywordsColors(const QMap<QString, QColor> &map)
{
  m_syntaxHighlighter->setKeywordsColors(map);
}

void glitch_source_preview::setObject(glitch_object *object)
{
  if(m_object)
    {
      disconnect(m_object,
		 &glitch_object::changed,
		 this,
		 &glitch_source_preview::slotObjectChanged);
      disconnect(m_object,
		 &glitch_object::simulateDeleteSignal,
		 this,
		 &glitch_source_preview::slotObjectChanged);
    }

  m_object = object;

  if(m_object)
    {
      connect(m_object,
	      &glitch_object::changed,
	      this,
	      &glitch_source_preview::slotObjectChanged);
      connect(m_object,
	      &glitch_object::simulateDeleteSignal,
	      this,
	      &glitch_source_preview::slotObjectChanged);
      setSource(m_object->code());
    }
  else
    setSource("");
}

void glitch_source_preview::setSource(const QString &text)
{
  m_ui.text->setPlainText(text.trimmed());
}

void glitch_source_preview::slotFind(void)
{
  m_ui.find->setFocus();
  m_ui.find->selectAll();
}

void glitch_source_preview::slotFindText(void)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  auto options = QTextDocument::FindFlags();
#else
  QTextDocument::FindFlags options = 0;
#endif

  if(m_ui.previous == qobject_cast<QPushButton *> (sender()))
    options = QTextDocument::FindBackward;

  glitch_misc::searchText(m_ui.find, m_ui.text, m_originalFindPalette, options);
}

void glitch_source_preview::slotObjectChanged(void)
{
  if(m_object && m_object->proxy() && m_object->proxy()->isSelected())
    setSource(m_object->code());
  else
    setSource("");
}
