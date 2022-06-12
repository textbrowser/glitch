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

#include <QShortcut>
#include <QtDebug>

#include "glitch-alignment.h"

glitch_alignment::glitch_alignment(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.bottom_align->setFocus();
  connect(m_ui.bottom_align,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotAlign);
  connect(m_ui.horizontal_center_align,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotAlign);
  connect(m_ui.horizontal_stack,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotStack);
  connect(m_ui.left_align,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotAlign);
  connect(m_ui.right_align,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotAlign);
  connect(m_ui.top_align,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotAlign);
  connect(m_ui.vertical_center_align,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotAlign);
  connect(m_ui.vertical_stack,
	  &QPushButton::clicked,
	  this,
	  &glitch_alignment::slotStack);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  setWindowModality(Qt::NonModal);
}

glitch_alignment::~glitch_alignment()
{
}

void glitch_alignment::slotAlign(void)
{
  auto toolButton = qobject_cast<QToolButton *> (sender());

  if(m_ui.bottom_align == toolButton)
    align(AlignmentTypes::ALIGN_BOTTOM);
  else if(m_ui.horizontal_center_align == toolButton)
    align(AlignmentTypes::ALIGN_CENTER_HORIZONTAL);
  else if(m_ui.left_align == toolButton)
    align(AlignmentTypes::ALIGN_LEFT);
  else if(m_ui.right_align == toolButton)
    align(AlignmentTypes::ALIGN_RIGHT);
  else if(m_ui.top_align == toolButton)
    align(AlignmentTypes::ALIGN_TOP);
  else if(m_ui.vertical_center_align == toolButton)
    align(AlignmentTypes::ALIGN_CENTER_VERTICAL);
}

void glitch_alignment::slotStack(void)
{
  auto toolButton = qobject_cast<QToolButton *> (sender());

  if(m_ui.horizontal_stack == toolButton)
    stack(StackTypes::HORIZONTAL_STACK);
  else if(m_ui.vertical_stack == toolButton)
    stack(StackTypes::VERTICAL_STACK);
}
