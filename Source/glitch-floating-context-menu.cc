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
#include "glitch-scroll-filter.h"

glitch_floating_context_menu::glitch_floating_context_menu(QWidget *parent):
  QDialog(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.frame,
	  SIGNAL(destroyed(void)),
	  this,
	  SLOT(deleteLater(void)));
  new QShortcut(tr("Ctrl+W"), this, SLOT(close(void)));
  resize(sizeHint());
  setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
  setWindowModality(Qt::NonModal);
}

glitch_floating_context_menu::~glitch_floating_context_menu()
{
  if(m_object)
    disconnect(m_object, nullptr, this, nullptr);
}

void glitch_floating_context_menu::addActions(const QList<QAction *> &actions)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto widget, m_ui.frame->findChildren<QWidget *> ())
    {
      if(m_ui.object_id == widget ||
	 m_ui.object_name == widget ||
	 m_ui.position == widget ||
	 m_ui.size == widget)
	continue;

      m_ui.frame->layout()->removeWidget(widget);

      if(widget)
	widget->deleteLater();
    }

  QMap<QString, QAction *> map;

  foreach(auto action, actions)
    if(action)
      map[action->text().remove('&')] = action;

  map["z-value"] = nullptr;

  QMapIterator<QString, QAction *> it(map);
  int i = 0;

  while(it.hasNext())
    {
      it.next();

      if(!it.value())
	{
	  if(it.key() == "z-value")
	    {
	      auto frame = new QFrame(this);
	      auto layout = new QHBoxLayout();

	      delete frame->layout();
	      frame->setLayout(layout);
	      m_zValue = new QDoubleSpinBox(this);
	      m_zValue->installEventFilter(new glitch_scroll_filter(this));
	      m_zValue->setDecimals(1);
	      m_zValue->setRange
		(glitch_common::s_minimumZValue,
		 glitch_common::s_maximumZValue);
	      m_zValue->setSizePolicy
		(QSizePolicy::Minimum, QSizePolicy::Fixed);
	      m_zValue->setToolTip
		(QString("[%1, %2]").
		 arg(m_zValue->minimum()).arg(m_zValue->maximum()));
	      m_zValue->setValue(property("z-value").toReal());
	      connect(m_zValue,
		      SIGNAL(valueChanged(qreal)),
		      this,
		      SLOT(slotZValueChanged(qreal)));
	      layout->addWidget(new QLabel(tr("Z-Value"), this));
	      layout->addWidget(m_zValue);
	      layout->addStretch();
	      m_ui.frame->layout()->addWidget(frame);
	    }

	  continue;
	}

      if(it.value()->isCheckable())
	{
	  auto checkBox = new QCheckBox(this);

	  checkBox->setChecked(it.value()->isChecked());
	  checkBox->setEnabled(it.value()->isEnabled());
	  checkBox->setIcon(it.value()->icon());
	  checkBox->setText(it.value()->text());
	  checkBox->setToolTip(it.value()->toolTip());
	  connect(it.value(),
		  SIGNAL(toggled(bool)),
		  checkBox,
		  SLOT(setChecked(bool)));
	  connect(checkBox,
		  SIGNAL(clicked(bool)),
		  it.value(),
		  SIGNAL(triggered(void)));
	  m_ui.frame->layout()->addWidget(checkBox);
	}
      else if(it.value()->isSeparator())
	continue;
      else
	{
	  if(it.key().startsWith(tr("Floating Context Menu")))
	    continue;

	  auto pushButton = new QPushButton(this);

	  it.value()->setProperty("index", i++);
	  connect(it.value(),
		  &QAction::changed,
		  this,
		  &glitch_floating_context_menu::slotActionChanged);
	  connect(pushButton,
		  &QPushButton::clicked,
		  it.value(),
		  &QAction::triggered);
	  pushButton->setEnabled(it.value()->isEnabled());
	  pushButton->setIcon(it.value()->icon());
	  pushButton->setText(it.value()->text());
	  pushButton->setToolTip(it.value()->toolTip());
	  m_ui.frame->layout()->addWidget(pushButton);
	}
    }

  QApplication::restoreOverrideCursor();
}

void glitch_floating_context_menu::closeEvent(QCloseEvent *event)
{
  QDialog::closeEvent(event);
  emit closed();
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

void glitch_floating_context_menu::setProperty
(const glitch_object::Properties property, const QVariant &value)
{
  switch(property)
    {
    case glitch_object::Properties::Z_VALUE:
      {
	QObject::setProperty("z-value", value);

	if(m_zValue)
	  {
	    disconnect(m_zValue,
		       SIGNAL(valueChanged(qreal)),
		       this,
		       SLOT(slotZValueChanged(qreal)));
	    m_zValue->setValue(value.toReal());
	    m_zValue->selectAll();
	    connect(m_zValue,
		    SIGNAL(valueChanged(qreal)),
		    this,
		    SLOT(slotZValueChanged(qreal)));
	  }

	break;
      }
    default:
      {
	break;
      }
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
    {
      m_ui.position->setText
	(tr("Position: (%1, %2)").
	 arg(m_object->pos().x()).arg(m_object->pos().y()));
      m_ui.size->setText
	(tr("Size: %1, %2").
	 arg(m_object->size().width()).arg(m_object->size().height()));
    }
}

void glitch_floating_context_menu::slotZValueChanged(qreal value)
{
  emit propertyChanged(glitch_object::Properties::Z_VALUE, value);
}
