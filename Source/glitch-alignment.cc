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

#include <QtDebug>

#include "glitch-alignment.h"

glitch_alignment::glitch_alignment(QWidget *parent):QWidget(parent)
{
  for(int i = 0; i < 8; i++)
    {
      auto action = new QAction(this);

      switch(i)
	{
	case 0:
	  {
	    action->setIcon(QIcon(":bottom-align.png"));
	    action->setToolTip(tr("Bottom Align"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotAlign);
	    break;
	  }
	case 1:
	  {
	    action->setIcon(QIcon(":top-align.png"));
	    action->setToolTip(tr("Top Align"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotAlign);
	    break;
	  }
	case 2:
	  {
	    action->setIcon(QIcon(":horizontal-center-align.png"));
	    action->setToolTip(tr("Horizontal Center Align"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotAlign);
	    break;
	  }
	case 3:
	  {
	    action->setIcon(QIcon(":vertical-center-align.png"));
	    action->setToolTip(tr("Vertical Center Align"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotAlign);
	    break;
	  }
	case 4:
	  {
	    action->setIcon(QIcon(":left-align.png"));
	    action->setToolTip(tr("Left Align"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotAlign);
	    break;
	  }
	case 5:
	  {
	    action->setIcon(QIcon(":right-align.png"));
	    action->setToolTip(tr("Right Align"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotAlign);
	    break;
	  }
	case 6:
	  {
	    action->setIcon(QIcon(":horizontal-stack.png"));
	    action->setToolTip(tr("Horizontal Stack"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotStack);
	    break;
	  }
	case 7:
	  {
	    action->setIcon(QIcon(":vertical-stack.png"));
	    action->setToolTip(tr("Vertical Stack"));
	    connect(action,
		    &QAction::triggered,
		    this,
		    &glitch_alignment::slotStack);
	    break;
	  }
	default:
	  {
	    break;
	  }
	}

      m_actions << action;
    }
}

glitch_alignment::~glitch_alignment()
{
}

QList<QAction *> glitch_alignment::actions(void) const
{
  return m_actions;
}

void glitch_alignment::slotAlign(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action == m_actions.at(0))
    align(AlignmentTypes::ALIGN_BOTTOM);
  else if(action == m_actions.at(1))
    align(AlignmentTypes::ALIGN_TOP);
  else if(action == m_actions.at(2))
    align(AlignmentTypes::ALIGN_CENTER_HORIZONTAL);
  else if(action == m_actions.at(3))
    align(AlignmentTypes::ALIGN_CENTER_VERTICAL);
  else if(action == m_actions.at(4))
    align(AlignmentTypes::ALIGN_LEFT);
  else if(action == m_actions.at(5))
    align(AlignmentTypes::ALIGN_RIGHT);
}

void glitch_alignment::slotStack(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action == m_actions.at(6))
    stack(StackTypes::HORIZONTAL_STACK);
  else if(action == m_actions.at(7))
    stack(StackTypes::VERTICAL_STACK);
}
