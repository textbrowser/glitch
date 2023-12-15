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

  auto cursor(textCursor());

  cursor.insertText("> ");
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
  QMapIterator<QString, QString> it(m_commands);
  QStringList list;
  auto command(currentCommand());

  while(it.hasNext())
    {
      it.next();

      if(command.isEmpty() || it.key().startsWith(command))
	list << it.key();
    }

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

glitch_ash::glitch_ash(QWidget *parent):QDialog(parent)
{
  m_commands[tr("about")] = "";
  m_commands[tr("clear")] = "";
  m_commands[tr("close")] = "";
  m_commands[tr("cls")] = "";
  m_commands[tr("display")] = tr("canvas-settings") +
    " " +
    tr("settings");
  m_commands[tr("help")] = "";
  m_commands[tr("show")] = m_commands.value(tr("display"));
  m_ui.setupUi(this);
  m_ui.close->setIcon(QIcon(":/close.png"));
  m_ui.text->setCommands(m_commands);
  m_ui.text->setCursorWidth(10);
  m_ui.text->setUndoRedoEnabled(false);
#ifndef Q_OS_ANDROID
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_ash::close);
#else
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_ash::hide);
#endif
  connect(m_ui.text,
	  SIGNAL(processCommand(const QString &)),
	  this,
	  SLOT(slotProcessCommand(const QString &)));
}

glitch_ash::~glitch_ash()
{
}

void glitch_ash::show(void)
{
  glitch_misc::centerWindow(parentWidget(), this);
  QDialog::showNormal();
  QDialog::activateWindow();
  QDialog::raise();
  m_ui.text->setFocus();
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

void glitch_ash::slotProcessCommand(const QString &command)
{
  if(command.trimmed().isEmpty())
    return;

  if(command == tr("clear") || command == tr("cls"))
    m_ui.text->clear();
  else if(command == tr("close"))
    close();
  else if(command == tr("help"))
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      QMapIterator<QString, QString> it(m_commands);

      while(it.hasNext())
	{
	  it.next();
	  m_ui.text->append(it.key());
	}

      QApplication::restoreOverrideCursor();
    }
  else if(command.indexOf(' ') == -1 && m_commands.value(command).size() > 0)
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      m_ui.text->append(command);

      foreach(const auto &i, m_commands.value(command).split(' '))
	m_ui.text->append(" " + i);

      QApplication::restoreOverrideCursor();
    }
  else
    emit processCommand(command);
}
