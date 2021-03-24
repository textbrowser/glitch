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

#include <QShortcut>

#include "glitch-structures-arduino.h"

QMap<QString, char> glitch_structures_arduino::s_structureNamesMap;
QMap<int, QStringList> glitch_structures_arduino::s_itemsForCategories;
QStringList glitch_structures_arduino::s_nonArrayTypes;
QStringList glitch_structures_arduino::s_types;

glitch_structures_arduino::glitch_structures_arduino(QWidget *parent):
  QDialog(parent)
{
  /*
  ** https://www.arduino.cc/en/Reference/HomePage
  */

  m_ui.setupUi(this);
  m_ui.tree->setProjectType(glitch_common::ArduinoProject);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  connect(m_ui.categories,
	  SIGNAL(itemSelectionChanged(void)),
	  this,
	  SLOT(slotCategorySelected(void)));
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("Glitch: Arduino Structures"));

  /*
  ** Create the list.
  */

  int i = 0;

  m_ui.categories->addItem(tr("Advanced I/O"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Advanced I/O");
  m_ui.categories->addItem(tr("Analog I/O"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Analog I/O");
  m_ui.categories->addItem(tr("Arithmetic Operators"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Arithmetic Operators");
  m_ui.categories->addItem(tr("Bits and Bytes"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Bits and Bytes");
  m_ui.categories->addItem(tr("Bitwise Operators"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Bitwise Operators");
  m_ui.categories->addItem(tr("Comparison Operators"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Comparison Operators");
  m_ui.categories->addItem(tr("Compound Operators"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Compound Operators");
  m_ui.categories->addItem(tr("Constants"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Constants");
  m_ui.categories->addItem(tr("Conversions"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Conversions");
  m_ui.categories->addItem(tr("Digital I/O"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Digital I/O");
  m_ui.categories->addItem(tr("External Interrupts"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "External Interrupts");
  m_ui.categories->addItem(tr("Flow Control"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Flow Control");
  m_ui.categories->addItem(tr("Interrupts"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Interrupts");
  m_ui.categories->addItem(tr("Logical Operators"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Logical Operators");
  m_ui.categories->addItem(tr("Mathematics"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Mathematics");
  m_ui.categories->addItem(tr("Random"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Random");
  m_ui.categories->addItem(tr("Serial"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Serial");
  m_ui.categories->addItem(tr("Structures"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Structures");
  m_ui.categories->addItem(tr("Time"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Time");
  m_ui.categories->addItem(tr("Trigonometry"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Trigonometry");
  m_ui.categories->addItem(tr("Utilities"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Utilities");
  m_ui.categories->addItem(tr("Variables"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Variables");

  if(s_itemsForCategories.isEmpty())
    {
      s_itemsForCategories[0] = QStringList() << "noTone()"
					      << "pulseIn()"
					      << "pulseInLong()"
					      << "shiftIn()"
					      << "shiftOut()"
					      << "tone()";
      s_itemsForCategories[1] = QStringList() << "analogRead()"
					      << "analogReference()"
					      << "analogWrite()";
      s_itemsForCategories[2] = QStringList() << "addition (+)"
					      << "assignment (=)"
					      << "division (/)"
					      << "modulo (%)"
					      << "multiplication (*)"
					      << "subtraction (-)";
      s_itemsForCategories[3] = QStringList() << "bit()"
					      << "bitClear()"
					      << "bitRead()"
					      << "bitSet()"
					      << "bitWrite()"
					      << "highByte()"
					      << "lowByte()";
      s_itemsForCategories[4] = QStringList() << "and (&)"
					      << "left shift (<<)"
					      << "not (~)"
					      << "or (|)"
					      << "right shift (>>)"
					      << "xor (^)";
      s_itemsForCategories[5] = QStringList() << "equal to (==)"
					      << "greater than (>)"
					      << "greater than or equal to (>=)"
					      << "less than (<)"
					      << "less than or equal to (<=)"
					      << "not equal to (!=)";
      s_itemsForCategories[6] = QStringList() << "addition (+=)"
					      << "bitwise and (&=)"
					      << "bitwise or (|=)"
					      << "bitwise xor (^=)"
					      << "decrement (--)"
					      << "division (/=)"
					      << "increment (++)"
					      << "modulo (%=)"
					      << "multiplication (*=)"
					      << "subtraction (-=)";
      s_itemsForCategories[7] = QStringList() << "HIGH"
					      << "INPUT"
					      << "INPUT_PULLUP"
					      << "LED_BUILTIN"
					      << "LOW"
					      << "OUTPUT"
					      << "false"
					      << "true";
      s_itemsForCategories[8] = QStringList() << "(unsigned int)"
					      << "(unsigned long)"
					      << "byte()"
					      << "char()"
					      << "float()"
					      << "int()"
					      << "long()"
					      << "word()";
      s_itemsForCategories[9] = QStringList() << "digitalRead()"
					      << "digitalWrite()"
					      << "pinMode()";
      s_itemsForCategories[10] = QStringList() << "attachInterrupt()"
					       << "detachInterrupt()";
      s_itemsForCategories[11] = QStringList() << "break"
					       << "continue"
					       << "do while loop"
					       << "for loop"
					       << "goto"
					       << "if statement"
					       << "if-else statement"
					       << "return"
					       << "switch case"
					       << "while loop";
      s_itemsForCategories[12] = QStringList() << "interrupts()"
					       << "noInterrupts()";
      s_itemsForCategories[13] = QStringList() << "and (&&)"
					       << "not (!)"
					       << "or (||)";
      s_itemsForCategories[14] = QStringList() << "abs()"
					       << "constrain()"
					       << "map()"
					       << "max()"
					       << "min()"
					       << "pow()"
					       << "sq()"
					       << "sqrt()";
      s_itemsForCategories[15] = QStringList() << "random()"
					       << "randomSeed()";
      s_itemsForCategories[16] = QStringList() << "Serial.begin()"
					       << "Serial.println()";
      s_itemsForCategories[17] = QStringList() << "block comment"
					       << "function()";
      s_itemsForCategories[18] = QStringList() << "delay()"
					       << "delayMicroseconds()"
					       << "micros()"
					       << "millis()";
      s_itemsForCategories[19] = QStringList() << "cos()"
					       << "sin()"
					       << "tan()";
      s_itemsForCategories[20] = QStringList() << "PROGMEM"
					       << "sizeof()";

      QStringList arrays;
      QStringList list(types());

      list.removeOne("array");
      list.removeOne("void");

      for(const auto &i : list)
	{
	  arrays << "array " + i;
	  arrays << i;
	}

      s_itemsForCategories[21] = arrays;
    }

  m_ui.categories->setCurrentRow(0);
}

glitch_structures_arduino::~glitch_structures_arduino()
{
}

QStringList glitch_structures_arduino::nonArrayTypes(void)
{
  if(!s_nonArrayTypes.isEmpty())
    return s_nonArrayTypes;

  s_nonArrayTypes = types();

  for(int i = s_nonArrayTypes.size() - 1; i >= 0; i--)
    if(s_nonArrayTypes.at(i).contains("array"))
      s_nonArrayTypes.removeAt(i);

  return s_nonArrayTypes;
}

QStringList glitch_structures_arduino::structureNames(void)
{
  if(!s_structureNamesMap.isEmpty())
    return s_structureNamesMap.keys();

  QMapIterator<int, QStringList> it(s_itemsForCategories);

  while(it.hasNext())
    {
      it.next();

      const auto &list(it.value());

      for(const auto &i : list)
	s_structureNamesMap[QString("arduino-%1").arg(i)] = 0;
    }

  return s_structureNamesMap.keys();
}

QStringList glitch_structures_arduino::types(void)
{
  if(!s_types.isEmpty())
    return s_types;

  s_types << "String"
	  << "array"
	  << "bool"
	  << "boolean"
	  << "byte"
	  << "char"
	  << "double"
	  << "float"
	  << "int"
	  << "long"
	  << "short"
	  << "size_t"
	  << "string"
	  << "unsigned char"
	  << "unsigned int"
	  << "unsigned long"
	  << "void"
	  << "word";
  return s_types;
}

bool glitch_structures_arduino::containsStructure(const QString &structureName)
{
  foreach(auto const &i, structureNames())
    if(i.toLower() == structureName.toLower())
      return true;

  return false;
}

void glitch_structures_arduino::slotCategorySelected(void)
{
  /*
  ** Let's create the tree.
  */

  auto items(m_ui.categories->selectedItems());

  if(items.isEmpty() || !items.at(0))
    return;

  m_ui.tree->clear();
  m_ui.tree->setCategory(items.at(0)->data(Qt::UserRole).toString());
  m_ui.tree->setHeaderLabel(items.at(0)->text());

  QStringList list;
  int row = m_ui.categories->row(items.at(0));

  if(row >= 0 && row <= 20)
    {
      list = s_itemsForCategories.value(row);

      for(int i = 0; i < list.size(); i++)
	m_ui.tree->addTopLevelItem
	  (new QTreeWidgetItem(QStringList() << list.at(i)));
    }
  else
    {
      list = types();

      for(int i = 0; i < list.size(); i++)
	if(list.at(i) == "array")
	  {
	    auto *item = new QTreeWidgetItem(QStringList() << list.at(i));

	    m_ui.tree->addTopLevelItem(item);

	    for(int i = 0; i < list.size(); i++)
	      if(list.at(i) == "array" || list.at(i) == "void")
		continue;
	      else
		item->addChild
		  (new QTreeWidgetItem(QStringList() << list.at(i)));
	  }
	else
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));
    }

  m_ui.tree->expandAll();
}
