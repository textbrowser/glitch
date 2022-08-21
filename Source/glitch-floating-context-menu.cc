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

#include <QAction>
#include <QCheckBox>
#include <QPushButton>
#include <QShortcut>

#include "glitch-floating-context-menu.h"
#include "glitch-object.h"

glitch_floating_context_menu::glitch_floating_context_menu(QWidget *parent):
  QDialog(parent)
{
  m_ui.setupUi(this);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  resize(sizeHint());
  setWindowModality(Qt::NonModal);
}

glitch_floating_context_menu::~glitch_floating_context_menu()
{
}

void glitch_floating_context_menu::addActions(const QList<QAction *> actions)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto widget, m_ui.frame->findChildren<QWidget *> ())
    {
      m_ui.frame->layout()->removeWidget(widget);
      widget->deleteLater();
    }

  for(int i = 0; i < actions.size(); i++)
    if(actions.at(i)->isCheckable())
      {
	auto checkBox = new QCheckBox(this);

	checkBox->setChecked(actions.at(i)->isChecked());
	checkBox->setEnabled(actions.at(i)->isEnabled());
	checkBox->setIcon(actions.at(i)->icon());
	checkBox->setText(actions.at(i)->text());
	checkBox->setToolTip(actions.at(i)->toolTip());
	connect(actions.at(i),
		QOverload<bool>::of(&QAction::toggled),
		checkBox,
		QOverload<bool>::of(&QCheckBox::setChecked));
	connect(checkBox,
		QOverload<bool>::of(&QCheckBox::clicked),
		actions.at(i),
		&QAction::triggered);
	m_ui.frame->layout()->addWidget(checkBox);
      }
    else if(actions.at(i)->isSeparator())
      continue;
    else
      {
	auto pushButton = new QPushButton(this);

	actions.at(i)->setProperty("index", m_ui.frame->layout()->count());
	connect(actions.at(i),
		&QAction::changed,
		this,
		&glitch_floating_context_menu::slotActionChanged);
	connect(pushButton,
		&QPushButton::clicked,
		actions.at(i),
		&QAction::triggered);
	pushButton->setEnabled(actions.at(i)->isEnabled());
	pushButton->setIcon(actions.at(i)->icon());
	pushButton->setText(actions.at(i)->text());
	pushButton->setToolTip(actions.at(i)->toolTip());
	m_ui.frame->layout()->addWidget(pushButton);
      }

  QApplication::restoreOverrideCursor();
}

void glitch_floating_context_menu::setIdentifier(const qint64 id)
{
  m_ui.object_id->setText(tr("Identifier: %1").arg(id));
}

void glitch_floating_context_menu::setName(const QString &n)
{
  auto name(n.trimmed());

  if(!name.isEmpty())
    m_ui.object_name->setText(name);
}

void glitch_floating_context_menu::setObject(glitch_object *object)
{
  if(m_object)
    return;

  m_object = object;

  if(m_object)
    {
      connect(m_object,
	      &glitch_object::changed,
	      this,
	      &glitch_floating_context_menu::slotObjectChanged);
      m_ui.object_id->setText(tr("ID: %1").arg(m_object->id()));
      slotObjectChanged();
    }
}

void glitch_floating_context_menu::slotActionChanged(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  auto item = m_ui.frame->layout()->itemAt(action->property("index").toInt());

  if(!item)
    return;

  auto widget = item->widget();

  if(!widget)
    return;

  widget->setEnabled(action->isEnabled());
}

void glitch_floating_context_menu::slotObjectChanged(void)
{
  if(m_object)
    m_ui.position->setText
      (QString("(%1, %2)").arg(m_object->pos().x()).arg(m_object->pos().y()));
}
