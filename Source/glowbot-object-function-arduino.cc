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
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QInputDialog>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>

#include "glowbot-object-edit-window.h"
#include "glowbot-object-function-arduino.h"
#include "glowbot-object-view.h"

QMap<QString, char> glowbot_object_function_arduino::s_functionNames;

glowbot_object_function_arduino::glowbot_object_function_arduino
(QWidget *parent):glowbot_object(parent)
{
  initialize(parent);

  /*
  ** Do not initialize the function's name in initialize().
  */

  QString name(nextUniqueFunctionName());

  m_editWindow->setWindowTitle(tr("GlowBot: %1").arg(name));
  m_ui.label->setText(name);
  s_functionNames[name] = 0;
  setProperty("function_name", m_ui.label->text());
}

glowbot_object_function_arduino::glowbot_object_function_arduino
(const quint64 id, QWidget *parent):glowbot_object(id, parent)
{
  initialize(parent);
}

glowbot_object_function_arduino::~glowbot_object_function_arduino()
{
  m_editWindow->deleteLater();
  s_functionNames.remove(m_ui.label->text());
}

QString glowbot_object_function_arduino::name(void) const
{
  return m_ui.label->text();
}

QString glowbot_object_function_arduino::nextUniqueFunctionName(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QString name("function_0()");
  quint64 i = 0;

  while(s_functionNames.contains(name))
    {
      i += 1;
      name = QString("function_%1()").arg(i);
    }

  QApplication::restoreOverrideCursor();
  return name;
}

bool glowbot_object_function_arduino::hasView(void) const
{
  return true;
}

bool glowbot_object_function_arduino::isMandatory(void) const
{
  return false;
}

void glowbot_object_function_arduino::addActions(QMenu &menu) const
{
  menu.addAction(tr("&Edit..."),
		 this,
		 SLOT(slotEdit(void)));
  menu.addAction(tr("Set Function &Name..."),
		 this,
		 SLOT(slotSetFunctionName(void)));
  addDefaultActions(menu);
}

glowbot_object_function_arduino *glowbot_object_function_arduino::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  glowbot_object_function_arduino *object = new glowbot_object_function_arduino
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString());
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glowbot_object_function_arduino::initialize(QWidget *parent)
{
  if(m_initialized)
    return;
  else
    m_initialized = true;

  m_editView = new glowbot_object_view(m_id, this);
  m_editWindow = new glowbot_object_edit_window(parent->parentWidget());
  m_editWindow->setCentralWidget(m_editView);
  m_editWindow->setWindowIcon(QIcon(":Logo/glowbot-logo.png"));
  m_editWindow->resize(600, 600);
  m_ui.setupUi(this);
  m_ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  m_ui.label->setAutoFillBackground(true);
  m_type = "arduino-function";
  connect(m_editView,
	  SIGNAL(changed(void)),
	  this,
	  SIGNAL(changed(void)));
}

void glowbot_object_function_arduino::mouseDoubleClickEvent(QMouseEvent *event)
{
  slotEdit();
  QWidget::mouseDoubleClickEvent(event);
}

void glowbot_object_function_arduino::save
(const QSqlDatabase &db, QString &error)
{
  glowbot_object::save(db, error);

  if(!error.isEmpty())
    return;

  QSqlQuery query(db);

  query.prepare("UPDATE objects SET properties = ? WHERE myoid = ?");
  query.addBindValue(QString("name = \"%1\"").arg(m_ui.label->text()));
  query.addBindValue(m_id);
  query.exec();

  if(query.lastError().isValid())
    error = query.lastError().text();
  else
    m_editView->save(db, error);
}

void glowbot_object_function_arduino::setProperties(const QString &properties)
{
  QStringList list(properties.split("&"));

  for(int i = 0; i < list.size(); i++)
    if(list.at(i).startsWith("name = "))
      {
	QString str(list.at(i).mid(7));

	str.remove("\"");
	s_functionNames.remove(str);
	setProperty("function_name", str);
	m_editWindow->setWindowTitle(tr("GlowBot: %1").arg(str));
	m_ui.label->setText(str);
	s_functionNames[str] = 0;
      }
}

void glowbot_object_function_arduino::slotEdit(void)
{
  m_editWindow->showNormal();
  m_editWindow->raise();
}

void glowbot_object_function_arduino::slotSetFunctionName(void)
{
  QInputDialog dialog(m_parent);

  dialog.setTextEchoMode(QLineEdit::Normal); // A line edit!
  dialog.setTextValue(m_ui.label->text());
  dialog.setWindowTitle(tr("GlowBot: Set Function Name"));

  QLineEdit *lineEdit = dialog.findChild<QLineEdit *> ();

  if(lineEdit)
    {
      lineEdit->selectAll();
      lineEdit->setValidator

	/*
	** A mandatory letter followed by an optional word. Allow
	** trailing parentheses.
	*/

	(new QRegExpValidator(QRegExp("[A-Za-z][\\w]*\\(\\)"), &dialog));
    }
  else
    qDebug() << "glowbot_object_function_arduino::slotSetFunctionName(): "
	     << "QInputDialog does not have a textfield! Cannot set "
	     << "an input validator.";

  if(dialog.exec() == QDialog::Accepted)
    {
      QString text(dialog.textValue().remove("(").remove(")").trimmed());

      if(text.isEmpty())
	return;
      else
	text.append("()");

      if(s_functionNames.contains(text))
	return;

      QString name(m_ui.label->text());

      s_functionNames.remove(m_ui.label->text());
      setProperty("function_name", text);
      m_editWindow->setWindowTitle(tr("GlowBot: %1").arg(text));
      m_ui.label->setText(text);
      s_functionNames[text] = 0;
      emit changed();
      emit nameChanged(name, m_ui.label->text());
    }
}
