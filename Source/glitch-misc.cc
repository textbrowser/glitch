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

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QIcon>
#include <QLineEdit>
#include <QMessageBox>
#include <QScopedArrayPointer>
#include <QTextLayout>

#include "glitch-misc.h"
#include "glitch-view.h"

QPointF glitch_misc::dbPointToPointF(const QString &text)
{
  auto list(QString(text).remove("(").remove(")").split(","));

  return {qAbs(list.value(0).toDouble()), qAbs(list.value(1).toDouble())};
}

QString glitch_misc::homePath(void)
{
  QString homePath(qgetenv("GLITCH_HOME").trimmed());

  if(homePath.isEmpty())
#ifdef Q_OS_WIN32
    return QDir::currentPath() + QDir::separator() + ".glitch";
#else
    return QDir::homePath() + QDir::separator() + ".glitch";
#endif
  else
    {
      static auto r
	(QRegularExpression(QString("[%1%1]+").arg(QDir::separator())));

      homePath.replace(r, QDir::separator());

      if(homePath.endsWith(QDir::separator()))
	homePath = homePath.mid(0, homePath.length() - 1);

      return homePath;
    }
}

bool glitch_misc::sameAncestors(const QObject *object1, const QObject *object2)
{
  if(!object1 || !object2)
    return false;

  auto parent1 = const_cast<QObject *> (object1);

  do
    {
      if(qobject_cast<glitch_view *> (parent1))
	break;
      else if(parent1)
	parent1 = parent1->parent();
      else
	break;
    }
  while(parent1);

  auto parent2 = const_cast<QObject *> (object2);

  do
    {
      if(qobject_cast<glitch_view *> (parent2))
	break;
      else if(parent2)
	parent2 = parent2->parent();
      else
	break;
    }
  while(parent2);

  return parent1 == parent2;
}

void glitch_misc::highlight(QLineEdit *lineEdit)
{
  if(!lineEdit)
    return;

  QHash<QString, QColor> colors;

  colors["#define"] = QColor(128, 0, 128);
  colors["#include"] = QColor(128, 0, 128);
  colors["bool"] = QColor(0, 100, 0);
  colors["boolean"] = QColor(0, 100, 0);
  colors["byte"] = QColor(0, 100, 0);
  colors["case"] = QColor(255, 0, 255);
  colors["char"] = QColor(0, 100, 0);
  colors["do"] = QColor(255, 0, 255);
  colors["double"] = QColor(0, 100, 0);
  colors["else"] = QColor(255, 0, 255);
  colors["float"] = QColor(0, 100, 0);
  colors["for"] = QColor(255, 0, 255);
  colors["if"] = QColor(255, 0, 255);
  colors["int"] = QColor(0, 100, 0);
  colors["long"] = QColor(0, 100, 0);
  colors["short"] = QColor(0, 100, 0);
  colors["size_t"] = QColor(0, 100, 0);
  colors["unsigned char"] = QColor(0, 100, 0);
  colors["unsigned int"] = QColor(0, 100, 0);
  colors["unsigned long"] = QColor(0, 100, 0);
  colors["while"] = QColor(255, 0, 255);
  colors["word"] = QColor(0, 100, 0);

  QScopedArrayPointer<QTextLayout::FormatRange> ranges;
  QTextCharFormat format;
  auto list(lineEdit->text().split(QRegularExpression("\\W+")));
  int index = 0;

  ranges.reset(new QTextLayout::FormatRange[list.size()]);

  for(int i = 0; i < list.size(); i++)
    {
      format.setFontHintingPreference(QFont::PreferFullHinting);
      format.setFontStyleStrategy(QFont::PreferAntialias);
      format.setFontWeight(QFont::Normal);

      auto string(list.at(i));

      if(colors.contains('#' + list.at(i)))
	{
	  format.setForeground(colors.value('#' + list.at(i)));
	  string.prepend("#");
	}
      else if(colors.contains(string))
	format.setForeground(colors.value(string));
      else
	format.setForeground(QColor(Qt::blue));

      ranges[i].format = format;
      ranges[i].length = string.length();
      ranges[i].start = index = lineEdit->text().indexOf(string, index);
      index += string.length();
    }

  QList<QInputMethodEvent::Attribute> attributes;

  for(int i = 0; i < list.size(); i++)
    attributes << QInputMethodEvent::Attribute
      (QInputMethodEvent::TextFormat,
       ranges[i].start - lineEdit->cursorPosition(),
       ranges[i].length,
       ranges[i].format);

  QInputMethodEvent event(QString(), attributes);
  auto state = lineEdit->isReadOnly();

  lineEdit->setReadOnly(false);
  QCoreApplication::sendEvent(lineEdit, &event);
  lineEdit->setReadOnly(state);
}

void glitch_misc::showErrorDialog(const QString &text, QWidget *parent)
{
  QMessageBox mb(parent);

  mb.setIcon(QMessageBox::Critical);
  mb.setText(text);
  mb.setWindowIcon(QIcon(":Logo/glitch-logo.png"));
  mb.setWindowTitle(QObject::tr("Glitch: Error"));
  mb.exec();
  QApplication::processEvents();
}

void glitch_misc::sortCombinationBox(QComboBox *comboBox)
{
  if(!comboBox)
    return;

  QStringList list;

  for(int i = 0; i < comboBox->count(); i++)
    list << comboBox->itemText(i);

  comboBox->clear();
  std::sort(list.begin(), list.end());
  comboBox->addItems(list);
}
