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

#ifndef _glitch_recent_diagram_h_
#define _glitch_recent_diagram_h_

#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidgetAction>

class glitch_recent_diagram: public QWidgetAction
{
  Q_OBJECT

 public:
  glitch_recent_diagram(const QString &fileName, QWidget *parent):
    QWidgetAction(parent)
  {
    m_fileName = fileName;
    m_label = new QLabel(m_fileName, m_widget = new QWidget(parent));

    QFileInfo fileInfo(m_fileName);

    if(!fileInfo.exists() || !fileInfo.isReadable())
      {
	m_label->setStyleSheet("QLabel {color: rgb(240, 128, 128);}");

	if(!fileInfo.exists())
	  m_label->setToolTip(tr("File does not exist."));
	else
	  m_label->setToolTip(tr("File is not readable."));
      }

    m_layout = new QHBoxLayout(m_widget);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_pushButton = new QPushButton(tr("Forget"), m_widget);
    m_pushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    m_layout->addWidget(m_pushButton);
    m_layout->addWidget(m_label);
    m_widget->setStyleSheet
      (QString("QLabel:hover {background: %1; color: %2;}").
       arg(m_widget->palette().color(QPalette::Highlight).name()).
       arg(m_widget->palette().color(QPalette::HighlightedText).name()));
    setDefaultWidget(m_widget);
    connect(m_pushButton,
	    &QPushButton::clicked,
	    this,
	    &glitch_recent_diagram::clicked);
  }

  ~glitch_recent_diagram()
  {
    m_label->deleteLater();
    m_layout->deleteLater();
    m_pushButton->deleteLater();
    m_widget->deleteLater();
  }

 private:
  QHBoxLayout *m_layout;
  QLabel *m_label;
  QPushButton *m_pushButton;
  QString m_fileName;
  QWidget *m_widget;

 signals:
  void clicked(void);
};

#endif
