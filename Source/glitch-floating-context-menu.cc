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

#include <QAction>
#include <QCheckBox>
#include <QPushButton>
#include <QtDebug>

#include "glitch-floating-context-menu.h"
#include "glitch-scroll-filter.h"

glitch_floating_context_menu::glitch_floating_context_menu(QWidget *parent):
  QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.button_box->button(QDialogButtonBox::Close)->setShortcut(tr("Ctrl+W"));
  connect(m_ui.frame,
	  SIGNAL(destroyed(void)),
	  this,
	  SLOT(deleteLater(void)));
  connect(m_ui.height,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotSizeChanged(int)));
  connect(m_ui.width,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotSizeChanged(int)));
  resize(sizeHint());
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

  foreach(auto widget, m_ui.property_frame->findChildren<QWidget *> ())
    {
      m_ui.property_frame->layout()->removeWidget(widget);

      if(widget)
	widget->deleteLater();
    }

  QMap<QString, QAction *> map;

  foreach(auto action, actions)
    if(action)
      map[action->text().remove('&')] = action;

  map["z-value"] = nullptr;

  QMapIterator<QString, QAction *> it(map);

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
	      m_ui.property_frame->layout()->addWidget(frame);
	    }

	  continue;
	}

      if(it.value()->isCheckable())
	{
	  auto checkBox = new QCheckBox(this);

	  checkBox->setChecked(it.value()->isChecked());
	  checkBox->setEnabled(it.value()->isEnabled());
	  checkBox->setObjectName(it.value()->text().remove('&'));
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
	  m_ui.property_frame->layout()->addWidget(checkBox);
	}
      else if(it.value()->isSeparator())
	continue;
      else
	{
	  if(it.key().startsWith(tr("Floating Context Menu")))
	    continue;

	  auto pushButton = new QPushButton(this);

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
	  pushButton->setMenu(it.value()->menu());
	  pushButton->setObjectName(it.value()->text().remove('&'));
	  pushButton->setProperty("property", it.value()->data());
	  pushButton->setText(it.value()->text());
	  pushButton->setToolTip(it.value()->toolTip());
	  m_ui.property_frame->layout()->addWidget(pushButton);
	}
    }

  QApplication::restoreOverrideCursor();
  setPushButtonColors();
}

void glitch_floating_context_menu::closeEvent(QCloseEvent *event)
{
  QDialog::closeEvent(event);
  emit closed();
}

void glitch_floating_context_menu::dockedFloatingContextMenu(const bool state)
{
  if(state)
    m_ui.button_box->button(QDialogButtonBox::Close)->setShortcut
      (QKeySequence());
}

void glitch_floating_context_menu::hideEvent(QHideEvent *event)
{
  QDialog::hideEvent(event);

  if(!isVisible()) // Minimized window?
    emit closed();
}

void glitch_floating_context_menu::setIdentifier(const qint64 id)
{
  m_ui.object_id->setText(tr("Identifier: %1").arg(id));
}

void glitch_floating_context_menu::setName(const QString &n)
{
  auto const name(n.trimmed());

  if(!name.isEmpty())
    {
      m_ui.object_name->setText(name);
      m_ui.object_name->setCursorPosition(0);
      m_ui.object_name->setToolTip(m_ui.object_name->text());
    }
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
      m_ui.object_id->setText(tr("Identifier: %1").arg(m_object->id()));
      slotObjectChanged();
    }
}

void glitch_floating_context_menu::setProperty
(const glitch_object::Properties property, const QVariant &value)
{

  switch(property)
    {
    case glitch_object::Properties::BACKGROUND_COLOR:
    case glitch_object::Properties::BORDER_COLOR:
    case glitch_object::Properties::FONT_COLOR:
      {
	setPushButtonColors();
	break;
      }
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

void glitch_floating_context_menu::setPushButtonColors(void)
{
  if(!m_object)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  foreach(auto pushButton, findChildren<QPushButton *> ())
    if(pushButton && pushButton->isEnabled())
      {
	QColor color;
	auto const property = glitch_object::DefaultMenuActions
	  (pushButton->property("property").toInt());

	switch(property)
	  {
	  case glitch_object::DefaultMenuActions::BACKGROUND_COLOR:
	    {
	      color = m_object->properties().
		value(glitch_object::Properties::BACKGROUND_COLOR).
		value<QColor> ();
	      break;
	    }
	  case glitch_object::DefaultMenuActions::BORDER_COLOR:
	    {
	      color = m_object->properties().
		value(glitch_object::Properties::BORDER_COLOR).
		value<QColor> ();
	      break;
	    }
	  case glitch_object::DefaultMenuActions::FONT_COLOR:
	    {
	      color = m_object->properties().
		value(glitch_object::Properties::FONT_COLOR).
		value<QColor> ();
	      break;
	    }
	  default:
	    {
	      break;
	    }
	  }

	if(color.isValid())
	  {
	    QPixmap pixmap(16, 16);

	    pixmap.fill(color);
	    pushButton->setIcon(pixmap);
	  }
      }

  QApplication::restoreOverrideCursor();
}

void glitch_floating_context_menu::slotActionChanged(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  auto widget = findChild<QWidget *> (action->text().remove('&'));

  if(!widget)
    return;

  widget->setEnabled(action->isEnabled());
}

void glitch_floating_context_menu::slotObjectChanged(void)
{
  if(m_object)
    {
      m_ui.height->setMinimum(m_object->minimumSize().height());
      m_ui.height->setReadOnly(m_object->isMandatory());
      m_ui.height->blockSignals(true);
      m_ui.height->setValue(m_object->size().height());
      m_ui.height->blockSignals(false);
      m_ui.position->setText
	(tr("Position: (%1, %2)").
	 arg(m_object->scenePos().x()).arg(m_object->scenePos().y()));
      m_ui.width->setMinimum(m_object->minimumSize().width());
      m_ui.width->setReadOnly(m_object->isMandatory());
      m_ui.width->blockSignals(true);
      m_ui.width->setValue(m_object->size().width());
      m_ui.width->blockSignals(false);
    }
}

void glitch_floating_context_menu::slotSizeChanged(int value)
{
  Q_UNUSED(value);
  emit propertyChanged
    (glitch_object::Properties::SIZE,
     QSize(m_ui.width->value(), m_ui.height->value()));
}

void glitch_floating_context_menu::slotZValueChanged(qreal value)
{
  emit propertyChanged(glitch_object::Properties::Z_VALUE, value);
}
