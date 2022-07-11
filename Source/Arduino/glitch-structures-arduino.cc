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
#include <QtDebug>

#include "glitch-structures-arduino.h"

QMap<QString, char> glitch_structures_arduino::s_structureNamesMap;
QMap<int, QStringList> glitch_structures_arduino::s_itemsForCategories;
QStringList glitch_structures_arduino::s_nonArrayVariableTypes;
QStringList glitch_structures_arduino::s_variableTypes;

glitch_structures_arduino::glitch_structures_arduino(QWidget *parent):
  QDialog(parent)
{
  /*
  ** https://www.arduino.cc/en/Reference/HomePage
  */

  m_ui.setupUi(this);
  m_ui.tree->setProjectType(glitch_common::ProjectTypes::ArduinoProject);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  connect(m_ui.categories,
	  &QListWidget::itemSelectionChanged,
	  this,
	  &glitch_structures_arduino::slotCategorySelected);
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
  m_ui.categories->addItem(tr("Characters"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Characters");
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
  m_ui.categories->addItem(tr("Stream"));
  m_ui.categories->item(i++)->setData(Qt::UserRole, "Stream");
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
      i = 0;
      s_itemsForCategories[i++] = QStringList() << "noTone()"
						<< "pulseIn()"
						<< "pulseInLong()"
						<< "shiftIn()"
						<< "shiftOut()"
						<< "tone()";
      s_itemsForCategories[i++] = QStringList() << "analogRead()"
						<< "analogReference()"
						<< "analogWrite()";
      s_itemsForCategories[i++] = QStringList() << "addition (+)"
						<< "assignment (=)"
						<< "division (/)"
						<< "modulo (%)"
						<< "multiplication (*)"
						<< "subtraction (-)";
      s_itemsForCategories[i++] = QStringList() << "bit()"
						<< "bitClear()"
						<< "bitRead()"
						<< "bitSet()"
						<< "bitWrite()"
						<< "highByte()"
						<< "lowByte()";
      s_itemsForCategories[i++] = QStringList() << "and (&)"
						<< "left shift (<<)"
						<< "not (~)"
						<< "or (|)"
						<< "right shift (>>)"
						<< "xor (^)";
      s_itemsForCategories[i++] = QStringList() << "isAlpha()"
						<< "isAlphaNumeric()"
						<< "isAscii()"
						<< "isControl()"
						<< "isDigit()"
						<< "isGraph()"
						<< "isHexadecimalDigit()"
						<< "isLowerCase()"
						<< "isPrintable()"
						<< "isPunct()"
						<< "isSpace()"
						<< "isUpperCase()"
						<< "isWhitespace()"; 
      s_itemsForCategories[i++] =
	QStringList() << "equal to (==)"
		      << "greater than (>)"
		      << "greater than or equal to (>=)"
		      << "less than (<)"
		      << "less than or equal to (<=)"
		      << "not equal to (!=)";
      s_itemsForCategories[i++] = QStringList() << "addition (+=)"
						<< "bitwise and (&=)"
						<< "bitwise or (|=)"
						<< "bitwise xor (^=)"
						<< "decrement (--)"
						<< "division (/=)"
						<< "increment (++)"
						<< "modulo (%=)"
						<< "multiplication (*=)"
						<< "subtraction (-=)";
      s_itemsForCategories[i++] = QStringList() << "HIGH"
						<< "INPUT"
						<< "INPUT_PULLUP"
						<< "LED_BUILTIN"
						<< "LOW"
						<< "OUTPUT"
						<< "Other"
						<< "false"
						<< "true";
      s_itemsForCategories[i++] = QStringList() << "(unsigned int)"
						<< "(unsigned long)"
						<< "byte()"
						<< "char()"
						<< "float()"
						<< "int()"
						<< "long()"
						<< "word()";
      s_itemsForCategories[i++] = QStringList() << "digitalRead()"
						<< "digitalWrite()"
						<< "pinMode()";
      s_itemsForCategories[i++] = QStringList() << "attachInterrupt()"
						<< "detachInterrupt()";
      s_itemsForCategories[i++] = QStringList() << "break"
						<< "case"
						<< "continue"
						<< "do while"
						<< "else if"
						<< "for"
						<< "goto"
						<< "if"
						<< "label"
						<< "return"
						<< "switch"
						<< "while";
      s_itemsForCategories[i++] = QStringList() << "interrupts()"
						<< "noInterrupts()";
      s_itemsForCategories[i++] = QStringList() << "and (&&)"
						<< "not (!)"
						<< "or (||)";
      s_itemsForCategories[i++] = QStringList() << "abs()"
						<< "constrain()"
						<< "map()"
						<< "max()"
						<< "min()"
						<< "pow()"
						<< "sq()"
						<< "sqrt()";
      s_itemsForCategories[i++] = QStringList() << "random()"
						<< "randomSeed()";
      s_itemsForCategories[i++] = QStringList() << "Serial.begin()"
						<< "Serial.println()";
      s_itemsForCategories[i++] = QStringList() << "stream.available()";
      s_itemsForCategories[i++] = QStringList() << "block comment"
						<< "function()";
      s_itemsForCategories[i++] = QStringList() << "delay()"
						<< "delayMicroseconds()"
						<< "micros()"
						<< "millis()";
      s_itemsForCategories[i++] = QStringList() << "cos()"
						<< "sin()"
						<< "tan()";
      s_itemsForCategories[i++] = QStringList() << "PROGMEM"
						<< "sizeof()";

      QStringList arrays;
      auto list(variableTypes());

      list.removeOne("array");

      for(const auto &i : list)
	{
	  arrays << "array " + i;
	  arrays << i;
	}

      s_itemsForCategories[i++] = arrays;
    }

  m_ui.categories->setCurrentRow(0);
}

glitch_structures_arduino::~glitch_structures_arduino()
{
}

QStringList glitch_structures_arduino::nonArrayVariableTypes(void)
{
  if(!s_nonArrayVariableTypes.isEmpty())
    return s_nonArrayVariableTypes;

  s_nonArrayVariableTypes = variableTypes();

  for(int i = s_nonArrayVariableTypes.size() - 1; i >= 0; i--)
    if(s_nonArrayVariableTypes.at(i).contains("array"))
      s_nonArrayVariableTypes.removeAt(i);

  return s_nonArrayVariableTypes;
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

QStringList glitch_structures_arduino::variableTypes(void)
{
  if(!s_variableTypes.isEmpty())
    return s_variableTypes;

  s_variableTypes << "String"
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
  return s_variableTypes;
}

bool glitch_structures_arduino::containsStructure(const QString &structureName)
{
  auto sn(structureName.toLower().trimmed());

  if(!sn.startsWith("arduino-"))
    sn.prepend("arduino-");

  if(sn.endsWith(" (array)"))
    sn.remove(" (array)");

  foreach(auto const &i, structureNames())
    if(i.toLower() == sn)
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
  auto row = m_ui.categories->row(items.at(0));

  if(row >= 0 && row <= 22)
    {
      list = s_itemsForCategories.value(row);

      for(int i = 0; i < list.size(); i++)
	m_ui.tree->addTopLevelItem
	  (new QTreeWidgetItem(QStringList() << list.at(i)));
    }
  else
    {
      list = variableTypes();

      for(int i = 0; i < list.size(); i++)
	if(list.at(i) == "array")
	  {
	    auto item = new QTreeWidgetItem(QStringList() << list.at(i));

	    m_ui.tree->addTopLevelItem(item);

	    for(int i = 0; i < list.size(); i++)
	      if(list.at(i) == "array" || list.at(i) == "void")
		continue;
	      else
		item->addChild
		  (new QTreeWidgetItem(QStringList() << QString("%1 (array)").
				                        arg(list.at(i))));
	  }
	else
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));
    }

  m_ui.tree->expandAll();
}
