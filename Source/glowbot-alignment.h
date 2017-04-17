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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _glowbot_alignment_h_
#define _glowbot_alignment_h_

#include <QDialog>

#include "ui_glowbot-alignment.h"

class glowbot_alignment: public QDialog
{
  Q_OBJECT

 public:
  glowbot_alignment(QWidget *parent);
  ~glowbot_alignment();

 private:
  enum AlignmentType
  {
    ALIGN_BOTTOM = 0,
    ALIGN_CENTER_HORIZONTAL,
    ALIGN_CENTER_VERTICAL,
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_TOP
  };

  enum StackType
  {
    HORIZONTAL_STACK = 0,
    VERTICAL_STACK
  };

  Ui_glowbot_alignment m_ui;
  void align(const AlignmentType alignmentType);
  void stack(const StackType stackType);

 private slots:
  void slotAlign(void);
  void slotStack(void);
};

#endif
