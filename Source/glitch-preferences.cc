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

#include <QFontDialog>
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
  connect(m_ui.display_application_font,
	  &QPushButton::clicked,
	  this,
	  &glitch_preferences::slotSelectFont);
  m_ui.display_application_font->setText
    (QApplication::font().toString().trimmed());
  prepareLanguages();
  processSettings();
  setWindowModality(Qt::ApplicationModal);
}

glitch_preferences::~glitch_preferences()
{
}

void glitch_preferences::prepareLanguages(void)
{
  QMap<QString, QString> map;

  map[tr("English")] = "English";

  QMapIterator<QString, QString> it(map);

  while(it.hasNext())
    {
      it.next();
      m_ui.display_language->addItem(it.key(), it.value());
    }
}

void glitch_preferences::processSettings(void)
{
  QSettings settings;

  m_ui.display_application_font->setText
    (settings.value("preferences/application_font").toString().trimmed());

  if(m_ui.display_application_font->text().isEmpty())
    m_ui.display_application_font->setText
      (QApplication::font().toString().trimmed());

  m_ui.docked_widget_property_editors->setChecked
    (settings.value("preferences/docked_widget_property_editors", false).
     toBool());
  m_ui.font_hinting->setCurrentIndex
    (m_ui.font_hinting->
     findText(settings.value("preferences/font_hinting").toString().trimmed()));

  if(m_ui.font_hinting->currentIndex() < 0)
    m_ui.font_hinting->setCurrentIndex(0); // Default

  auto language(settings.value("preferences/language").toString().trimmed());

  for(int i = 0; i < m_ui.display_language->count(); i++)
    if(QString::compare(language,
			m_ui.display_language->itemData(i).toString(),
			Qt::CaseInsensitive) == 0)
      {
	m_ui.display_language->setCurrentIndex(i);
	break;
      }

  if(m_ui.display_language->currentIndex() < 0)
    m_ui.display_language->setCurrentIndex
      (m_ui.display_language->findText(tr("English")));

  m_ui.display_tear_off_menus->setChecked
    (settings.value("preferences/tear_off_menus", true).toBool());
}

void glitch_preferences::slotApply(void)
{
  QSettings settings;

  settings.setValue
    ("preferences/application_font",
     m_ui.display_application_font->text().remove('&'));
  settings.setValue
    ("preferences/docked_widget_property_editors",
     m_ui.docked_widget_property_editors->isChecked());
  settings.setValue
    ("preferences/font_hinting", m_ui.font_hinting->currentText());
  settings.setValue
    ("preferences/language", m_ui.display_language->currentData().toString());
  settings.setValue
    ("preferences/tear_off_menus", m_ui.display_tear_off_menus->isChecked());
  emit accept();
}

void glitch_preferences::slotSelectFont(void)
{
  QFont font;
  QFontDialog dialog(this);
  auto string(m_ui.display_application_font->text());

  if(!string.isEmpty() && font.fromString(string.remove('&')))
    dialog.setCurrentFont(font);
  else
    dialog.setCurrentFont(QApplication::font());

  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_ui.display_application_font->setText
	(dialog.selectedFont().toString().trimmed());
    }
  else
    QApplication::processEvents();
}
