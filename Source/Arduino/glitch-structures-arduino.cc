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
QStringList glitch_structures_arduino::s_nonArrayVariableTypes;
QStringList glitch_structures_arduino::s_variableTypes;

glitch_structures_arduino::glitch_structures_arduino(QWidget *parent):
  QDialog(parent)
{
  /*
  ** https://www.arduino.cc/en/Reference/HomePage
  */

  m_ui.setupUi(this);
  connect(m_ui.categories,
	  &QListWidget::itemSelectionChanged,
	  this,
	  &glitch_structures_arduino::slotCategorySelected);
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_structures_arduino::close);
  m_ui.close->setIcon(QIcon::fromTheme("window-close"));
  m_ui.tree->setProjectType(glitch_common::ProjectTypes::ArduinoProject);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  prepareCategories();
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("Glitch: Arduino Structures"));

  /*
  ** Create the list.
  */

  if(s_itemsForCategories.isEmpty())
    {
      int i = 0;

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
						<< "else"
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
      s_itemsForCategories[i++] = QStringList() << "Serial.available()"
						<< "Serial.availableForWrite()"
						<< "Serial.begin()"
						<< "Serial.end()"
						<< "Serial.find()"
						<< "Serial.findUntil()"
						<< "Serial.flush()"
						<< "Serial.parseFloat()"
						<< "Serial.parseInt()"
						<< "Serial.peek()"
						<< "Serial.print()"
						<< "Serial.println()"
						<< "Serial.read()"
						<< "Serial.readBytes()"
						<< "Serial.readBytesUntil()"
						<< "Serial.readString()"
						<< "Serial.readStringUntil()"
						<< "Serial.setTimeout()"
						<< "Serial.write()";
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
      s_itemsForCategories[i++] = QStringList() << "sizeof()";

      QStringList arrays;
      auto list(variableTypes());

      list.removeOne("array");

      foreach(const auto &i, list)
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

      foreach(const auto &i, list)
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

void glitch_structures_arduino::prepareCategories(void)
{
  QMap<QString, QString> map;

  map[tr("Advanced I/O")] = "Advanced I/O";
  map[tr("Analog I/O")] = "Analog I/O";
  map[tr("Arithmetic Operators")] = "Arithmetic Operators";
  map[tr("Bits and Bytes")] = "Bits and Bytes";
  map[tr("Bitwise Operators")] = "Bitwise Operators";
  map[tr("Characters")] = "Characters";
  map[tr("Compound Operators")] = "Compound Operators";
  map[tr("Constants")] = "Constants";
  map[tr("Conversions")] = "Conversions";
  map[tr("Digital I/O")] = "Digital I/O";
  map[tr("External Interrupts")] = "External Interrupts";
  map[tr("Flow Control")] = "Flow Control";
  map[tr("Interrupts")] = "Interrupts";
  map[tr("Logical Operators")] = "Logical Operators";
  map[tr("Mathematics")] = "Mathematics";
  map[tr("Random")] = "Random";
  map[tr("Serial")] = "Serial";
  map[tr("Stream")] = "Stream";
  map[tr("Structures")] = "Structures";
  map[tr("Time")] = "Time";
  map[tr("Trigonometry")] = "Trigonometry";
  map[tr("Utilities")] = "Utilities";
  map[tr("Variables")] = "Variables";
  m_ui.categories->clear();
  m_ui.categories->addItems(map.keys());

  for(int i = 0; i < m_ui.categories->count(); i++)
    {
      auto item = m_ui.categories->item(i);

      if(item)
	item->setData(Qt::UserRole, map.value(item->text()));
    }
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

  if(row >= 0 && row <= 21)
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
