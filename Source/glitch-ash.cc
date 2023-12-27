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

#include <QKeyEvent>

#include "glitch-ash.h"
#include "glitch-misc.h"

QString glitch_ash_textedit::currentCommand(void) const
{
  auto cursor(textCursor());

  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.movePosition
    (QTextCursor::Right, QTextCursor::MoveAnchor, m_promptLength);
  cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

  auto command(cursor.selectedText().simplified().trimmed());

  cursor.clearSelection();
  return command;
}

bool glitch_ash_textedit::handleBackspaceKey(void) const
{
  auto cursor(textCursor());

  if(cursor.blockNumber() == m_promptBlockNumber &&
     cursor.columnNumber() == m_promptLength)
    return true;

  return false;
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
      auto command(currentCommand());

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

void glitch_ash_textedit::handleReturnKey(void)
{
  auto command(currentCommand());

  if(!command.isEmpty())
    {
      emit processCommand(command);
      m_history << command;
      m_historyIndex = m_history.size();
    }

  append("");
  moveCursor(QTextCursor::End);
  displayPrompt();
}

void glitch_ash_textedit::handleTabKey(void)
{
  QStringList list;
  auto command(currentCommand());

  foreach(const auto &i, m_commands.uniqueKeys())
    if(command.isEmpty() || i.startsWith(command))
      list << i;

  if(list.size() == 1)
    replaceCurrentCommand(list.at(0) + " ");
  else
    {
      moveCursor(QTextCursor::End);

      for(int i = 0; i < list.size(); i++)
	append(list.at(i));

      append("");
      displayPrompt();
      replaceCurrentCommand(command);
    }
}

void glitch_ash_textedit::handleUpKey(void)
{
  if(!m_history.isEmpty())
    {
      auto command(currentCommand());

      do
	{
	  if(m_historyIndex)
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
    case Qt::Key_Down:
      {
	handleDownKey();
	return;
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

void glitch_ash_textedit::replaceCurrentCommand(const QString &command)
{
  auto cursor(textCursor());

  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.movePosition
    (QTextCursor::Right, QTextCursor::MoveAnchor, m_promptLength);
  cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
  cursor.insertText(command);
}

void glitch_ash_textedit::showEvent(QShowEvent *event)
{
  QTextEdit::showEvent(event);
  setFocus();
}

glitch_ash::glitch_ash(QWidget *parent):QDialog(parent)
{
  m_commands.insert(tr("about"), "");
  m_commands.insert(tr("clear"), "");
  m_commands.insert(tr("cls"), "");
  m_commands.insert(tr("display"),
		    tr("canvas-settings") +
		    " " +
		    tr("settings"));
  m_commands.insert(tr("help"), "");
  m_commands.insert(tr("redo"), "");
  m_commands.insert(tr("select"), tr("all identifier-1 identifier-2 ..."));
  m_commands.insert(tr("set"), tr("widget-position identifier-1 x,y ..."));
  m_commands.insert(tr("set"), tr("widget-size identifier width,height ..."));
  m_commands.insert(tr("show"), m_commands.value(tr("display")));
  m_commands.insert(tr("undo"), "");
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

  m_ui.text->append(results);
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

  if(command == tr("clear") || command == tr("cls"))
    m_ui.text->clear();
  else if(command == tr("help"))
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      QString string("");

      foreach(const auto &i, m_commands.uniqueKeys())
	string.append(i + " ");

      m_ui.text->append(string.trimmed());
      QApplication::restoreOverrideCursor();
    }
  else if(command.indexOf(' ') == -1 && m_commands.value(command).size() > 0)
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      m_ui.text->append(command + ":");

      auto list(m_commands.values(command));

      std::sort(list.begin(), list.end());

      foreach(const auto &i, list)
	m_ui.text->append(i);

      QApplication::restoreOverrideCursor();
    }
  else if(m_commands.contains(command))
    emit processCommand(command);
  else
    m_ui.text->append(tr("%1: command not recognized.").arg(command));
}
