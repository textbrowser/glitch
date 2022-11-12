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

#include <QPushButton>
#include <QSettings>

#include "glitch-preferences.h"

glitch_preferences::glitch_preferences(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.buttonBox->button(QDialogButtonBox::Apply),
	  &QPushButton::clicked,
	  this,
	  &glitch_preferences::slotApply);
  prepare();
  setWindowModality(Qt::ApplicationModal);
}

glitch_preferences::~glitch_preferences()
{
}

void glitch_preferences::prepare(void)
{
  QSettings settings;

  m_ui.tear_off_menus->setChecked
    (settings.value("preferences/tear_off_menus", true).toBool());
}

void glitch_preferences::slotApply(void)
{
  QSettings settings;

  settings.setValue
    ("preferences/tear_off_menus", m_ui.tear_off_menus->isChecked());
  emit accept();
}
