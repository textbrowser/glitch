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

#include <QInputDialog>

#include "glitch-object-block-comment-arduino.h"
#include "glitch-undo-command.h"

glitch_object_block_comment_arduino::glitch_object_block_comment_arduino
(QWidget *parent):glitch_object_block_comment_arduino(1, parent)
{
  m_properties[Properties::COMMENT] = "A comment in the landscape of Glitch.";
}

glitch_object_block_comment_arduino::glitch_object_block_comment_arduino
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "arduino-blockcomment";
  prepareContextMenu();
  resize(100, 100);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setName(m_type);
  setStyleSheet("background-color: transparent;");
}

glitch_object_block_comment_arduino::~glitch_object_block_comment_arduino()
{
}

QString glitch_object_block_comment_arduino::code(void) const
{
  if(!property(Properties::GENERATE_SOURCE).toBool())
    return "";

  QString code("");

  foreach
    (auto const &str,
     m_properties.value(Properties::COMMENT).toString().trimmed().split('\n'))
    if(!str.trimmed().isEmpty())
      code.append(QString("// %1\n").arg(str.trimmed()));

  return code.trimmed();
}

bool glitch_object_block_comment_arduino::isFullyWired(void) const
{
  return true;
}

bool glitch_object_block_comment_arduino::shouldPrint(void) const
{
  return true;
}

glitch_object_block_comment_arduino *glitch_object_block_comment_arduino::
clone(QWidget *parent) const
{
  auto clone = new glitch_object_block_comment_arduino(parent);

  clone->cloneWires(m_copiedConnectionsPositions);
  clone->cloneWires(m_wires);
  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->resize(size());
  clone->setAttribute
    (Qt::WA_OpaquePaintEvent, testAttribute(Qt::WA_OpaquePaintEvent));
  clone->setCanvasSettings(m_canvasSettings);
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_block_comment_arduino *glitch_object_block_comment_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_block_comment_arduino
    (values.value("myoid").toLongLong(), parent);

  object->setProperties(splitPropertiesAmpersand(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_block_comment_arduino::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::SET_COMMENT_TEXT))
    {
      auto action = new QAction(tr("Comment &Text..."), this);

      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_block_comment_arduino::slotSetCommentText,
	      Qt::QueuedConnection);
      m_actions[DefaultMenuActions::SET_COMMENT_TEXT] = action;
      menu.addAction(action);
    }
  else if(m_actions.value(DefaultMenuActions::SET_COMMENT_TEXT, nullptr))
    menu.addAction(m_actions.value(DefaultMenuActions::SET_COMMENT_TEXT));

  addDefaultActions(menu);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setChecked(false);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setEnabled(false);
}

void glitch_object_block_comment_arduino::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QPainterPath path;
  QPen pen;
  auto const frameWidth = static_cast<qreal> (1.5);
  auto const frameWidth1 = frameWidth / 2.0;
  auto const height = static_cast<qreal> (size().height());
  auto const radius = static_cast<qreal> (5.0);
  auto const width = static_cast<qreal> (size().width());
  const QColor color(m_properties.value(Properties::BORDER_COLOR).toString());

  painter.setFont(font());
  path.addRoundedRect
    (QRectF(frameWidth1, frameWidth1, width - frameWidth, height - frameWidth),
     radius,
     radius);
  painter.fillPath
  (path,
   QBrush(QColor(m_properties.value(Properties::BACKGROUND_COLOR).toString())));
  pen.setColor(color);
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setWidthF(frameWidth);
  painter.setPen(pen);
  painter.save();
  painter.drawPath(path);
  painter.restore();
  pen.setColor(QColor(m_properties.value(Properties::FONT_COLOR).toString()));
  painter.setPen(pen);
  painter.save();
  painter.drawText
    (path.boundingRect().adjusted(5.0, 5.0, -5.0, -5.0),
     Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
     m_properties.value(Properties::COMMENT).toString().trimmed());
  painter.restore();
}

void glitch_object_block_comment_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["comment"] =
    m_properties.value(Properties::COMMENT).toString().trimmed();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_block_comment_arduino::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::COMMENT] = "";

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.simplified().startsWith("comment = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::COMMENT] = string.trimmed();
	}
    }
}

void glitch_object_block_comment_arduino::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::COMMENT:
      {
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_block_comment_arduino::slotSetCommentText(void)
{
  QInputDialog dialog(m_parent);

  dialog.resize(350, dialog.sizeHint().height());
  dialog.setLabelText(tr("Set Comment Text"));
  dialog.setTextEchoMode(QLineEdit::Normal); // A line edit!
  dialog.setTextValue
    (m_properties.value(Properties::COMMENT).toString().trimmed());
  dialog.setWindowIcon(QIcon(":/Logo/glitch-logo.png"));
  dialog.setWindowTitle(tr("Glitch: Set Comment Text"));
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (dialog.textValue().trimmed(),
	     m_properties.value(Properties::COMMENT),
	     glitch_undo_command::Types::PROPERTY_CHANGED,
	     Properties::COMMENT,
	     this);

	  undoCommand->setText
	    (tr("comment text changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}
      else
	setProperty(Properties::COMMENT, dialog.textValue().trimmed());

      emit changed();
    }
  else
    QApplication::processEvents();
}
