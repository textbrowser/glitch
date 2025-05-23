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

#include <QColorDialog>

#include "glitch-object.h"
#include "glitch-port-colors.h"
#include "glitch-variety.h"

glitch_port_colors::glitch_port_colors(QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.buttonBox->setEnabled(false);
  connect(m_ui.buttonBox->button(QDialogButtonBox::Apply),
	  &QPushButton::clicked,
	  this,
	  &glitch_port_colors::applied);
  connect(m_ui.input_connected,
	  &QPushButton::clicked,
	  this,
	  &glitch_port_colors::slotSelectColor);
  connect(m_ui.input_full,
	  &QPushButton::clicked,
	  this,
	  &glitch_port_colors::slotSelectColor);
  connect(m_ui.input_disconnected,
	  &QPushButton::clicked,
	  this,
	  &glitch_port_colors::slotSelectColor);
  connect(m_ui.output_connected,
	  &QPushButton::clicked,
	  this,
	  &glitch_port_colors::slotSelectColor);
  connect(m_ui.output_disconnected,
	  &QPushButton::clicked,
	  this,
	  &glitch_port_colors::slotSelectColor);
}

glitch_port_colors::~glitch_port_colors()
{
}

QString glitch_port_colors::colors(void) const
{
  return m_ui.input_connected->text().remove('&') +
    "-" +
    m_ui.input_disconnected->text().remove('&') +
    "-" +
    m_ui.input_full->text().remove('&') +
    "-" +
    m_ui.output_connected->text().remove('&') +
    "-" +
    m_ui.output_disconnected->text().remove('&');
}

void glitch_port_colors::setColors(const QVariant &value)
{
  QList<QPushButton *> list;

  if(value.toString().count('-') == 3)
    {
      glitch_variety::assignImage(m_ui.input_full, QColor(Qt::red));
      list << m_ui.input_connected
	   << m_ui.input_disconnected
	   << m_ui.output_connected
	   << m_ui.output_disconnected;
      m_ui.input_full->setText(QColor(Qt::red).name(QColor::HexArgb));
    }
  else
    list << m_ui.input_connected
	 << m_ui.input_disconnected
	 << m_ui.input_full
	 << m_ui.output_connected
	 << m_ui.output_disconnected;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  auto const strings(value.toString().split('-', Qt::SkipEmptyParts));
#else
  auto const strings(value.toString().split('-', QString::SkipEmptyParts));
#endif

  for(int i = 0; i < list.size(); i++)
    {
      auto const string(strings.value(i).remove('&').trimmed());

      glitch_variety::assignImage(list.at(i), QColor(string));
      list.at(i)->setText(string);
    }
}

void glitch_port_colors::setObject(glitch_object *object)
{
  m_object = object;
  m_ui.buttonBox->setEnabled(object);

  if(m_object)
    setColors(m_object->property(glitch_object::Properties::PORT_COLORS));
}

void glitch_port_colors::slotSelectColor(void)
{
  auto button = qobject_cast<QPushButton *> (sender());

  if(!button)
    return;

  QColorDialog dialog(this);
  QString title("");

  if(button == m_ui.input_connected)
    title = tr("Glitch: Select Widget Connected Input Color");
  else if(button == m_ui.input_disconnected)
    title = tr("Glitch: Select Widget Disconnected Input Color");
  else if(button == m_ui.input_full)
    title = tr("Glitch: Select Widget Full Input Color");
  else if(button == m_ui.output_connected)
    title = tr("Glitch: Select Widget Connected Output Color");
  else
    title = tr("Glitch: Select Widget Disconnected Output Color");

  dialog.setCurrentColor(QColor(button->text().remove('&')));
  dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(title);
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      button->setText(dialog.selectedColor().name(QColor::HexArgb));
      glitch_variety::assignImage(button, dialog.selectedColor());
    }
  else
    QApplication::processEvents();
}
