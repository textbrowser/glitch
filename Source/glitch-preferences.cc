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

#include <QFileDialog>
#include <QFontDialog>
#include <QPushButton>
#include <QSettings>

#include "glitch-preferences.h"
#include "glitch-scroll-filter.h"
#include "glitch-variety.h"

glitch_preferences::glitch_preferences(QWidget *parent):QDialog(parent)
{
  m_defaultOutputDirectory = QDir::homePath() + QDir::separator() + "Glitch";
  m_ui.setupUi(this);
  glitch_variety::sortCombinationBox(m_ui.font_hinting);
  m_ui.button_box->button(QDialogButtonBox::Close)->setShortcut(tr("Ctrl+W"));
  connect(m_ui.button_box->button(QDialogButtonBox::Apply),
	  &QPushButton::clicked,
	  this,
	  &glitch_preferences::slotApply);
  connect(m_ui.button_box->button(QDialogButtonBox::Close),
	  &QPushButton::clicked,
	  this,
	  &glitch_preferences::reject);
  connect(m_ui.display_application_font,
	  &QPushButton::clicked,
	  this,
	  &glitch_preferences::slotSelectFont);
  connect(m_ui.output_directory,
	  SIGNAL(textChanged(const QString &)),
	  this,
	  SLOT(slotOutputDirectoryTextChanged(const QString &)));
  connect(m_ui.select_output_directory,
	  &QPushButton::clicked,
	  this,
	  &glitch_preferences::slotSelectOutputDirectory);
  m_ui.display_application_font->setText
    (QApplication::font().toString().trimmed());
  m_ui.display_language->installEventFilter(new glitch_scroll_filter(this));
  m_ui.font_hinting->installEventFilter(new glitch_scroll_filter(this));
  m_ui.output_directory->setText(m_defaultOutputDirectory);
  m_ui.output_directory->setCursorPosition(0);
  m_ui.output_directory->selectAll();
  m_ui.select_output_directory->setIcon(QIcon(":/open.png"));
  m_ui.zoom_factor->installEventFilter(new glitch_scroll_filter(this));
  m_ui.zoom_factor->setToolTip
    (tr("[%1, %2]").
     arg(m_ui.zoom_factor->minimum()).arg(m_ui.zoom_factor->maximum()));
  prepareLanguages();
  processSettings();
  setWindowModality(Qt::ApplicationModal);
}

glitch_preferences::~glitch_preferences()
{
}

void glitch_preferences::prepareEnvironmentVariables(void)
{
  auto const variable
    (QSettings().value("preferences/QT_STYLE_OVERRIDE").
     toByteArray().trimmed());

  qputenv("QT_STYLE_OVERRIDE", variable);
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

  glitch_variety::sortCombinationBox(m_ui.display_language);
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
    (settings.value("preferences/docked_widget_property_editors", true).
     toBool());
  m_ui.download_version_information->setChecked
    (settings.value("preferences/download_version_information", false).
     toBool());
  m_ui.font_hinting->setCurrentIndex
    (m_ui.font_hinting->
     findText(settings.value("preferences/font_hinting").toString().trimmed()));

  if(m_ui.font_hinting->currentIndex() < 0)
    m_ui.font_hinting->setCurrentIndex(0); // Default

  auto const language
    (settings.value("preferences/language").toString().trimmed());

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

  m_ui.display_tab_bar->setChecked
    (settings.value("preferences/tab_bar", true).toBool());
  m_ui.display_tear_off_menus->setChecked
    (settings.value("preferences/tear_off_menus", true).toBool());
  m_ui.output_directory->setText
    (QFileInfo(settings.
	       value("preferences/output_directory", m_defaultOutputDirectory).
	       toString()).absoluteFilePath());
  m_ui.output_directory->setCursorPosition(0);
  m_ui.output_directory->selectAll();
  m_ui.override_widget_fonts->setChecked
    (settings.value("preferences/override_widget_fonts", true).toBool());
  m_ui.style_override->setText
    (settings.value("preferences/QT_STYLE_OVERRIDE").toString().trimmed());
  m_ui.zoom_factor->setValue
    (settings.value("preferences/zoom_factor", 1.25).toDouble());
}

void glitch_preferences::slotApply(void)
{
  QSettings settings;

  settings.setValue
    ("preferences/QT_STYLE_OVERRIDE", m_ui.style_override->text().trimmed());
  settings.setValue
    ("preferences/application_font",
     m_ui.display_application_font->text().remove('&'));
  settings.setValue
    ("preferences/docked_widget_property_editors",
     m_ui.docked_widget_property_editors->isChecked());
  settings.setValue
    ("preferences/download_version_information",
     m_ui.download_version_information->isChecked());
  settings.setValue
    ("preferences/font_hinting", m_ui.font_hinting->currentText());
  settings.setValue
    ("preferences/language", m_ui.display_language->currentData().toString());
  settings.setValue
    ("preferences/output_directory", m_ui.output_directory->text());
  settings.setValue
    ("preferences/override_widget_fonts",
     m_ui.override_widget_fonts->isChecked());
  settings.setValue("preferences/tab_bar", m_ui.display_tab_bar->isChecked());
  settings.setValue
    ("preferences/tear_off_menus", m_ui.display_tear_off_menus->isChecked());
  settings.setValue("preferences/zoom_factor", m_ui.zoom_factor->value());
  m_ui.output_directory->setText
    (QFileInfo(settings.value("preferences/output_directory").toString()).
     absoluteFilePath());
  m_ui.output_directory->setCursorPosition(0);
  m_ui.output_directory->selectAll();
  emit accept();
}

void glitch_preferences::slotOutputDirectoryTextChanged(const QString &text)
{
  if(text.trimmed().isEmpty())
    m_ui.output_directory->setToolTip
      (tr("Directory which will house generated files."));
  else
    m_ui.output_directory->setToolTip
      (tr("Directory which will house generated files. (%1)").arg(text));
}

void glitch_preferences::slotSelectFont(void)
{
  QFont font;
  QFontDialog dialog(this);
  auto string(m_ui.display_application_font->text());

  if(string.isEmpty() && !font.fromString(string.remove('&')))
    dialog.setCurrentFont(QApplication::font());
  else
    dialog.setCurrentFont(font);

#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
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

void glitch_preferences::slotSelectOutputDirectory(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(m_ui.output_directory->text());
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Glitch: Select Output Directory"));
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_ui.output_directory->setText(dialog.selectedFiles().value(0));
      m_ui.output_directory->setCursorPosition(0);
      m_ui.output_directory->selectAll();
    }
}
