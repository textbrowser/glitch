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

#include <QDateTime>
#include <QDir>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStandardPaths>

#include "glitch-ash.h"
#include "glitch-variety.h"

QString glitch_ash_textedit::currentCommand(void) const
{
  auto cursor(textCursor());

  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.movePosition
    (QTextCursor::Right, QTextCursor::MoveAnchor, m_promptLength);
  cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

  auto const command(cursor.selectedText().simplified().trimmed());

  cursor.clearSelection();
  return command;
}

QString glitch_ash_textedit::history(const int index) const
{
  if(index >= 0 && index < m_history.count())
    return m_history.at(index);
  else
    return "";
}

bool glitch_ash_textedit::handleBackspaceKey(void) const
{
  auto const cursor(textCursor());

  if(cursor.blockNumber() == m_promptBlockNumber &&
     cursor.columnNumber() == m_promptLength)
    return true;

  return false;
}

void glitch_ash_textedit::clearHistory(void)
{
  m_history.clear();
  m_historyIndex = 0;
}

void glitch_ash_textedit::displayPrompt(void)
{
  setTextColor(QColor(0, 0, 139));

  QTextImageFormat image;
  auto cursor(textCursor());

  image.setHeight(24.0);
  image.setName(":/language.png");
  image.setWidth(24.0);
  cursor.insertImage(image);
  cursor.insertText(" > ");
  cursor.movePosition(QTextCursor::EndOfLine);
  setTextCursor(cursor);
  m_promptBlockNumber = cursor.blockNumber();
}

void glitch_ash_textedit::handleDownKey(void)
{
  if(!m_history.isEmpty())
    {
      auto const command(currentCommand());

      do
	{
	  if(++m_historyIndex >= m_history.size())
	    {
	      m_historyIndex = m_history.size() - 1;
	      break;
	    }
	}
      while(command == m_history.value(m_historyIndex));

      if(m_history.size() < m_historyIndex)
	replaceCurrentCommand("");
      else
	replaceCurrentCommand(m_history.value(m_historyIndex));
    }
}

void glitch_ash_textedit::handleHomeKey(void)
{
  auto cursor(textCursor());

  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.movePosition
    (QTextCursor::Right, QTextCursor::MoveAnchor, m_promptLength);
  setTextCursor(cursor);
}

void glitch_ash_textedit::handleInterrupt(void)
{
  replaceCurrentCommand(currentCommand() + "^C");
  append("");
  moveCursor(QTextCursor::End);
  displayPrompt();
}

void glitch_ash_textedit::handleReturnKey(void)
{
  auto const command(currentCommand());

  if(!command.isEmpty())
    {
      emit processCommand(command);
      m_history << (m_commands.contains(command) ? command + " " : command);
      m_historyIndex = m_history.size();
    }

  append("");
  moveCursor(QTextCursor::End);
  displayPrompt();
}

void glitch_ash_textedit::handleTabKey(void)
{
  auto const command(currentCommand());

  if(command.isEmpty())
    return;

  QMap<QString, char> map;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QMapIterator<QString, QString> it(m_commands);
#else
  QMultiMapIterator<QString, QString> it(m_commands);
#endif

  while(it.hasNext())
    {
      it.next();

      if(it.key().startsWith(command))
	map[it.key()] = 0;
      else
	{
	  auto const string(it.key() + " " + it.value());

	  if(string.startsWith(command))
	    map[string] = 0;
	}
    }

  if(map.size() == 1)
    replaceCurrentCommand(map.firstKey() + " ");
  else if(map.size() > 1)
    {
      moveCursor(QTextCursor::End);

      QMapIterator<QString, char> it(map);
      QString string("");

      while(it.hasNext())
	{
	  it.next();
	  string.append(it.key()).append(" ");
	}

      append(string.trimmed());
      append("");
      displayPrompt();
      replaceCurrentCommand(command);
    }
}

void glitch_ash_textedit::handleUpKey(void)
{
  if(!m_history.isEmpty())
    {
      auto const command(currentCommand());

      do
	{
	  if(m_historyIndex > 0)
	    m_historyIndex -= 1;
	  else
	    break;
	}
      while(command == m_history.value(m_historyIndex));

      replaceCurrentCommand(m_history.value(m_historyIndex));
    }
}

void glitch_ash_textedit::keyPressEvent(QKeyEvent *event)
{
  if(!event)
    {
      QTextEdit::keyPressEvent(event);
      return;
    }

  switch(event->key())
    {
    case Qt::Key_Backspace:
      {
	if(handleBackspaceKey())
	  return;

	break;
      }
    case Qt::Key_C:
      {
	auto const modifiers = glitch_variety::keyboardModifiers();

	if(modifiers & Qt::ControlModifier)
	  {
	    if(modifiers & Qt::ShiftModifier)
	      handleInterrupt();
	    else
	      copy();

	    return;
	  }

	break;
      }
    case Qt::Key_Down:
      {
	handleDownKey();
	return;
      }
    case Qt::Key_End:
      {
	break;
      }
    case Qt::Key_Enter:
    case Qt::Key_Return:
      {
	handleReturnKey();
	return;
      }
    case Qt::Key_Left:
      {
	if(handleBackspaceKey())
	  return;

	break;
      }
    case Qt::Key_Home:
      {
	if(verticalScrollBar())
	  {
	    if(glitch_variety::keyboardModifiers() & Qt::ControlModifier)
	      {
		verticalScrollBar()->setValue(0);
		return;
	      }
	  }

	handleHomeKey();
	return;
      }
    case Qt::Key_PageDown:
      {
	if(verticalScrollBar())
	  {
	    verticalScrollBar()->triggerAction
	      (QAbstractSlider::SliderPageStepAdd);
	    return;
	  }

	break;
      }
    case Qt::Key_PageUp:
      {
	if(verticalScrollBar())
	  {
	    verticalScrollBar()->triggerAction
	      (QAbstractSlider::SliderPageStepSub);
	    return;
	  }

	break;
      }
    case Qt::Key_Tab:
      {
	handleTabKey();
	return;
      }
    case Qt::Key_Up:
      {
	handleUpKey();
	return;
      }
    }

  QTextEdit::keyPressEvent(event);
}

void glitch_ash_textedit::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
}

void glitch_ash_textedit::mousePressEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
}

void glitch_ash_textedit::printHistory(void)
{
  if(m_history.isEmpty())
    {
      m_history << tr("history ");
      m_historyIndex = 1;
    }

  for(int i = 0; i < m_history.size(); i++)
    append(QString("%1: %2").arg(i + 1).arg(m_history.at(i)));
}

void glitch_ash_textedit::replaceCurrentCommand(const QString &command)
{
  auto cursor(textCursor());

  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.movePosition
    (QTextCursor::Right, QTextCursor::MoveAnchor, m_promptLength);
  cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
  cursor.insertText(command);
}

void glitch_ash_textedit::saveHistoryDesktop(void) const
{
  auto fileName
    (QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).
     value(0));

  fileName.append(QDir::separator());
  fileName.append
    (QString("glitch-ash-history-%1.txt").
     arg(QDateTime::currentDateTime().toString(Qt::ISODate).
	 remove('-').remove(':')));

  QFile file(fileName);

  if(file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
    {
      QTextStream stream(&file);

      for(int i = 0; i < m_history.size(); i++)
	stream << m_history.at(i)
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	       << endl
#else
	       << Qt::endl
#endif
	  ;
    }
}

void glitch_ash_textedit::showEvent(QShowEvent *event)
{
  QTextEdit::showEvent(event);
  setFocus();
}

glitch_ash::glitch_ash(const bool topLevel, QWidget *parent):QDialog(parent)
{
  if(topLevel)
    {
      m_commands.insert(tr("about"), "");
      m_commands.insert(tr("generate"), tr("clipboard"));
      m_commands.insert(tr("generate"), tr("file"));
      m_commands.insert(tr("generate"), tr("view"));
      m_commands.insert(tr("show"), tr("canvas-settings"));
      m_commands.insert(tr("show"), tr("settings"));
    }

  m_commands.insert(tr("clear"), "");
  m_commands.insert(tr("clear-history"), "");
  m_commands.insert(tr("close"), "");
  m_commands.insert(tr("cls"), "");
  m_commands.insert(tr("copy"), "");
  m_commands.insert(tr("delete"), tr("identifier-1 identifier-2 ..."));
  m_commands.insert(tr("deselect"), tr("all"));
  m_commands.insert(tr("deselect"), tr("identifier-1 identifier-2 ..."));
  m_commands.insert(tr("full-screen"), "");
  m_commands.insert(tr("help"), "");
  m_commands.insert(tr("history"), "");
  m_commands.insert(tr("list"), tr("all"));
  m_commands.insert(tr("list"), tr("local"));
  m_commands.insert(tr("list-statistics"), "");
  m_commands.insert(tr("normal-screen"), "");
  m_commands.insert(tr("paste"), "");
  m_commands.insert(tr("redo"), "");
  m_commands.insert(tr("save"), "");
  m_commands.insert(tr("save-history-desktop"), "");
  m_commands.insert(tr("select"), tr("all"));
  m_commands.insert(tr("select"), tr("identifier-1 identifier-2 ..."));
  m_commands.insert(tr("set"), tr("widget-position identifier-1 x,y ..."));
  m_commands.insert(tr("set"), tr("widget-size identifier-1 width,height ..."));
  m_commands.insert(tr("undo"), "");
  m_commands.insert(tr("upload"), tr("--board --port --verbose"));
  m_commands.insert(tr("verify"), tr("--board --port --verbose"));
  m_isTopLevel = topLevel;
  m_ui.setupUi(this);
  m_ui.text->setCommands(m_commands);
  m_ui.text->setCursorWidth(10);
  m_ui.text->setUndoRedoEnabled(false);
  connect(m_ui.text,
	  SIGNAL(processCommand(const QString &)),
	  this,
	  SLOT(slotProcessCommand(const QString &)));
}

glitch_ash::~glitch_ash()
{
}

bool glitch_ash::optionsOptional(const QString &command)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  auto const list(command.split(' ', Qt::SkipEmptyParts));
#else
  auto const list(command.split(' ', QString::SkipEmptyParts));
#endif
  auto state = true;

  foreach(auto const &string, m_commands.value(list.value(0)).split(' '))
    if(!string.startsWith("--"))
      {
	state = false;
	break;
      }

  return state;
}

void glitch_ash::slotCanvasNameChanged(const QString &name)
{
  if(name.trimmed().isEmpty())
    return;

  setWindowTitle(tr("Glitch: ASH (%1)").arg(name.trimmed()));
}

void glitch_ash::slotCommandProcessed(const QString &results)
{
  if(results.trimmed().isEmpty())
    return;

  m_ui.text->append(results.trimmed());
}

void glitch_ash::slotInformationReceived(const QString &text)
{
  if(text.trimmed().isEmpty())
    return;

  m_ui.text->append(text.trimmed());
}

void glitch_ash::slotProcessCommand(const QString &command)
{
  if(command.trimmed().isEmpty())
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString history("");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  foreach(auto const &c, command.split(';', Qt::SkipEmptyParts))
#else
  foreach(auto const &c, command.split(';', QString::SkipEmptyParts))
#endif
  {
    auto const command(c.trimmed());

    if(command == tr("?") || command == tr("help"))
      {
	QString string("");
	auto even = false;

	foreach(auto const &i, m_commands.uniqueKeys())
	  {
	    even = !even;

	    if(even)
	      string.append("<b>" + i + "</b> ");
	    else
	      string.append(i + " ");
	  }

	m_ui.text->append(string.trimmed());
      }
    else if(command == tr("clear") || command == tr("cls"))
      m_ui.text->clear();
    else if(command == tr("clear-history"))
      m_ui.text->clearHistory();
    else if(command.indexOf(' ') == -1 && m_commands.value(command).size() > 0)
      {
	if(command == tr("list") || optionsOptional(command))
	  {
	    emit processCommand(command);
	    continue;
	  }

	m_ui.text->append("<b>" + command + ":</b>");

	auto list(m_commands.values(command));

	std::sort(list.begin(), list.end());

	foreach(auto const &i, list)
	  m_ui.text->append(i);
      }
    else if(command.startsWith("!"))
      history = m_ui.text->history
	(QString(command).remove('!').toInt() - 1).trimmed();
    else if(command.startsWith(tr("?")) || command.startsWith(tr("help")))
      {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	auto const list1(command.split(' ', Qt::SkipEmptyParts));
#else
	auto const list1(command.split(' ', QString::SkipEmptyParts));
#endif

	for(int i = 1; i < list1.size(); i++)
	  {
	    auto list2(m_commands.values(list1.at(i)));

	    if(list2.isEmpty())
	      continue;

	    m_ui.text->append("<b>" + list1.at(i) + ":</b>");
	    std::sort(list2.begin(), list2.end());

	    foreach(auto const &j, list2)
	      if(!j.isEmpty())
		m_ui.text->append(j);
	  }
      }
    else if(command.startsWith(tr("history")))
      m_ui.text->printHistory();
    else if(command.startsWith(tr("save-history-desktop")))
      m_ui.text->saveHistoryDesktop();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    else if(m_commands.
	    contains(command.split(' ', Qt::SkipEmptyParts).value(0)))
#else
    else if(m_commands.
	    contains(command.split(' ', QString::SkipEmptyParts).value(0)))
#endif
      emit processCommand(command);
    else
      m_ui.text->append(tr("%1: command not recognized.").arg(command));
  }

  QApplication::restoreOverrideCursor();

  if(!history.isEmpty())
    emit m_ui.text->processCommand(history);
}
