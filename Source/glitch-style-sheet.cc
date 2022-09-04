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

#include "glitch-style-sheet.h"

glitch_style_sheet::glitch_style_sheet(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.buttonBox->setEnabled(false);
  m_ui.preview->setEnabled(false);
  m_ui.style_sheet->setPlainText(tr("Glitch error. m_widget is zero."));
  m_ui.style_sheet->setReadOnly(true);
  connect(m_ui.preview,
	  &QPushButton::clicked,
	  this,
	  &glitch_style_sheet::slotPreview);
}

glitch_style_sheet::~glitch_style_sheet()
{
}

QString glitch_style_sheet::styleSheet(void) const
{
  return m_ui.style_sheet->toPlainText();
}

void glitch_style_sheet::setWidget(QWidget *widget)
{
  m_ui.buttonBox->setEnabled(widget != nullptr);
  m_ui.preview->setEnabled(widget != nullptr);
  m_widget = widget;

  if(m_widget)
    {
      m_ui.style_sheet->setPlainText(m_widget->styleSheet());
      m_ui.style_sheet->setReadOnly(false);
    }
  else
    {
      m_ui.style_sheet->setPlainText(tr("Glitch error. m_widget is zero."));
      m_ui.style_sheet->setReadOnly(true);
    }
}

void glitch_style_sheet::slotPreview(void)
{
  if(!m_widget)
    return;

  m_widget->setStyleSheet(m_ui.style_sheet->toPlainText());
}
