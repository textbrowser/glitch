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

#include <QComboBox>
#include <QShortcut>

#include "glitch-object-function-parameters-arduino.h"
#include "glitch-structures-arduino.h"

QSize glitch_object_function_parameters_arduino_item_delegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *glitch_object_function_parameters_arduino_item_delegate::
createEditor(QWidget *parent,
	     const QStyleOptionViewItem &option,
	     const QModelIndex &index) const
{
  switch(index.column())
    {
    case glitch_object_function_parameters_arduino::ColumnIndices::TYPE_COLUMN:
      {
	auto comboBox = new QComboBox(parent);
	auto types(glitch_structures_arduino::types());

	types.removeAll("void");
	comboBox->addItems(types);
	comboBox->setCurrentIndex
	  (comboBox->findText(index.data().toString().trimmed()));

	if(comboBox->currentIndex() < 0)
	  comboBox->setCurrentIndex(0);

	return comboBox;
      }
    default:
      {
	break;
      }
    }

  return QStyledItemDelegate::createEditor(parent, option, index);
}

void glitch_object_function_parameters_arduino_item_delegate::
setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QStyledItemDelegate::setEditorData(editor, index);
}

void glitch_object_function_parameters_arduino_item_delegate::
setModelData(QWidget *editor,
	     QAbstractItemModel *model,
             const QModelIndex &index) const
{
  QStyledItemDelegate::setModelData(editor, model, index);
}

void glitch_object_function_parameters_arduino_item_delegate::
updateEditorGeometry(QWidget *editor,
		     const QStyleOptionViewItem &option,
		     const QModelIndex &index) const
{
  QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

glitch_object_function_parameters_arduino::
glitch_object_function_parameters_arduino
(const QMap<QString, QString> &parameters, QWidget *parent):QDialog(parent)
{
  m_ui.setupUi(this);
  m_ui.parameters->setItemDelegateForColumn
    (ColumnIndices::TYPE_COLUMN,
     new glitch_object_function_parameters_arduino_item_delegate(this));
  m_ui.parameters->setRowCount(parameters.size());

  QMapIterator<QString, QString> it(parameters);
  int i = 0;

  while(it.hasNext())
    {
      it.next();

      auto item = new QTableWidgetItem(it.key());

      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.parameters->setItem(i, ColumnIndices::PARAMETER_COLUMN, item);
      item = new QTableWidgetItem(it.value());
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.parameters->setItem(i, ColumnIndices::TYPE_COLUMN, item);
      i += 1;
    }

  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  connect(m_ui.add,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAdd(void)));
  connect(m_ui.apply,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotApply(void)));
  connect(m_ui.remove,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDelete(void)));
}

glitch_object_function_parameters_arduino::
~glitch_object_function_parameters_arduino()
{
}

void glitch_object_function_parameters_arduino::slotAdd(void)
{
  m_ui.parameters->setSortingEnabled(false);

  auto item = new QTableWidgetItem();

  item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  m_ui.parameters->setRowCount(m_ui.parameters->rowCount() + 1);
  m_ui.parameters->setItem
    (m_ui.parameters->rowCount() - 1, ColumnIndices::PARAMETER_COLUMN, item);
  item = new QTableWidgetItem(tr("int"));
  item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  m_ui.parameters->setItem
    (m_ui.parameters->rowCount() - 1, ColumnIndices::TYPE_COLUMN, item);
  m_ui.parameters->setSortingEnabled(true);
}

void glitch_object_function_parameters_arduino::slotApply(void)
{
}

void glitch_object_function_parameters_arduino::slotDelete(void)
{
}
