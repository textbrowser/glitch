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

#include "glitch-structures-arduino.h"

QMap<QString, char> glitch_structures_arduino::s_structureNamesMap;

glitch_structures_arduino::glitch_structures_arduino(QWidget *parent):
  QDialog(parent)
{
  /*
  ** https://www.arduino.cc/en/Reference/HomePage
  */

  m_ui.setupUi(this);
  m_ui.tree->setProjectType(glitch_common::ArduinoProject);
  connect(m_ui.categories,
	  SIGNAL(itemSelectionChanged(void)),
	  this,
	  SLOT(slotCategorySelected(void)));
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("Glitch: Arduino Structures"));

  /*
  ** Create the list.
  */

  m_ui.categories->addItem(tr("Advanced I/O"));
  m_ui.categories->addItem(tr("Analog I/O"));
  m_ui.categories->addItem(tr("Arithmetic Operators"));
  m_ui.categories->addItem(tr("Bits and Bytes"));
  m_ui.categories->addItem(tr("Bitwise Operators"));
  m_ui.categories->addItem(tr("Comparison Operators"));
  m_ui.categories->addItem(tr("Compound Operators"));
  m_ui.categories->addItem(tr("Constants"));
  m_ui.categories->addItem(tr("Conversions"));
  m_ui.categories->addItem(tr("Digital I/O"));
  m_ui.categories->addItem(tr("External Interrupts"));
  m_ui.categories->addItem(tr("Flow Control"));
  m_ui.categories->addItem(tr("Interrupts"));
  m_ui.categories->addItem(tr("Logical Operators"));
  m_ui.categories->addItem(tr("Mathematics"));
  m_ui.categories->addItem(tr("Random"));
  m_ui.categories->addItem(tr("Serial"));
  m_ui.categories->addItem(tr("Structures"));
  m_ui.categories->addItem(tr("Time"));
  m_ui.categories->addItem(tr("Trigonometry"));
  m_ui.categories->addItem(tr("Utilities"));
  m_ui.categories->addItem(tr("Variables"));
  m_ui.categories->setCurrentRow(0);
}

glitch_structures_arduino::~glitch_structures_arduino()
{
}

QStringList glitch_structures_arduino::structureNames(void)
{
  if(!s_structureNamesMap.isEmpty())
    return s_structureNamesMap.keys();

  s_structureNamesMap["arduino-HIGH"] = 0;
  s_structureNamesMap["arduino-INPUT"] = 0;
  s_structureNamesMap["arduino-INPUT_PULLUP"] = 0;
  s_structureNamesMap["arduino-LED_BUILTIN"] = 0;
  s_structureNamesMap["arduino-LOW"] = 0;
  s_structureNamesMap["arduino-OUTPUT"] = 0;
  s_structureNamesMap["arduino-PROGMEM"] = 0;
  s_structureNamesMap["arduino-Serial.begin()"] = 0;
  s_structureNamesMap["arduino-Serial.println()"] = 0;
  s_structureNamesMap["arduino-abs()"] = 0;
  s_structureNamesMap["arduino-addition (+)"] = 0;
  s_structureNamesMap["arduino-addition (+=)"] = 0;
  s_structureNamesMap["arduino-analogRead()"] = 0;
  s_structureNamesMap["arduino-analogReference()"] = 0;
  s_structureNamesMap["arduino-analogWrite()"] = 0;
  s_structureNamesMap["arduino-and (&&)"] = 0;
  s_structureNamesMap["arduino-and (&)"] = 0;
  s_structureNamesMap["arduino-array boolean"] = 0;
  s_structureNamesMap["arduino-array byte"] = 0;
  s_structureNamesMap["arduino-array char"] = 0;
  s_structureNamesMap["arduino-array double"] = 0;
  s_structureNamesMap["arduino-array float"] = 0;
  s_structureNamesMap["arduino-array int"] = 0;
  s_structureNamesMap["arduino-array long"] = 0;
  s_structureNamesMap["arduino-array short"] = 0;
  s_structureNamesMap["arduino-array unsigned char"] = 0;
  s_structureNamesMap["arduino-array unsigned int"] = 0;
  s_structureNamesMap["arduino-array unsigned long"] = 0;
  s_structureNamesMap["arduino-array word"] = 0;
  s_structureNamesMap["arduino-assignment (=)"] = 0;
  s_structureNamesMap["arduino-attachInterrupt()"] = 0;
  s_structureNamesMap["arduino-bit()"] = 0;
  s_structureNamesMap["arduino-bitClear()"] = 0;
  s_structureNamesMap["arduino-bitRead()"] = 0;
  s_structureNamesMap["arduino-bitSet()"] = 0;
  s_structureNamesMap["arduino-bitWrite()"] = 0;
  s_structureNamesMap["arduino-bitwise and (&=)"] = 0;
  s_structureNamesMap["arduino-bitwise or (|=)"] = 0;
  s_structureNamesMap["arduino-block comment"] = 0;
  s_structureNamesMap["arduino-break"] = 0;
  s_structureNamesMap["arduino-byte()"] = 0;
  s_structureNamesMap["arduino-char()"] = 0;
  s_structureNamesMap["arduino-constrain()"] = 0;
  s_structureNamesMap["arduino-continue"] = 0;
  s_structureNamesMap["arduino-cos()"] = 0;
  s_structureNamesMap["arduino-decrement (--)"] = 0;
  s_structureNamesMap["arduino-delay()"] = 0;
  s_structureNamesMap["arduino-delayMicroseconds()"] = 0;
  s_structureNamesMap["arduino-detachInterrupt()"] = 0;
  s_structureNamesMap["arduino-digitalRead()"] = 0;
  s_structureNamesMap["arduino-digitalWrite()"] = 0;
  s_structureNamesMap["arduino-division (/)"] = 0;
  s_structureNamesMap["arduino-division (/=)"] = 0;
  s_structureNamesMap["arduino-do while loop"] = 0;
  s_structureNamesMap["arduino-equal to (==)"] = 0;
  s_structureNamesMap["arduino-false"] = 0;
  s_structureNamesMap["arduino-float()"] = 0;
  s_structureNamesMap["arduino-for loop"] = 0;
  s_structureNamesMap["arduino-function()"] = 0;
  s_structureNamesMap["arduino-goto"] = 0;
  s_structureNamesMap["arduino-greater than (>)"] = 0;
  s_structureNamesMap["arduino-greater than or equal to (>=)"] = 0;
  s_structureNamesMap["arduino-highByte()"] = 0;
  s_structureNamesMap["arduino-if statement"] = 0;
  s_structureNamesMap["arduino-if-else statement"] = 0;
  s_structureNamesMap["arduino-increment (++)"] = 0;
  s_structureNamesMap["arduino-int()"] = 0;
  s_structureNamesMap["arduino-interrupts()"] = 0;
  s_structureNamesMap["arduino-left shift (<<)"] = 0;
  s_structureNamesMap["arduino-less than (<)"] = 0;
  s_structureNamesMap["arduino-less than or equal to (<=)"] = 0;
  s_structureNamesMap["arduino-long()"] = 0;
  s_structureNamesMap["arduino-lowByte()"] = 0;
  s_structureNamesMap["arduino-map()"] = 0;
  s_structureNamesMap["arduino-max()"] = 0;
  s_structureNamesMap["arduino-micros()"] = 0;
  s_structureNamesMap["arduino-millis()"] = 0;
  s_structureNamesMap["arduino-min()"] = 0;
  s_structureNamesMap["arduino-modulo (%)"] = 0;
  s_structureNamesMap["arduino-modulo (%=)"] = 0;
  s_structureNamesMap["arduino-multiplication (*)"] = 0;
  s_structureNamesMap["arduino-multiplication (*=)"] = 0;
  s_structureNamesMap["arduino-noInterrupts()"] = 0;
  s_structureNamesMap["arduino-noTone()"] = 0;
  s_structureNamesMap["arduino-not (!)"] = 0;
  s_structureNamesMap["arduino-not (~)"] = 0;
  s_structureNamesMap["arduino-not equal to (!=)"] = 0;
  s_structureNamesMap["arduino-or (|)"] = 0;
  s_structureNamesMap["arduino-or (||)"] = 0;
  s_structureNamesMap["arduino-pinMode()"] = 0;
  s_structureNamesMap["arduino-pow()"] = 0;
  s_structureNamesMap["arduino-pulseIn()"] = 0;
  s_structureNamesMap["arduino-random()"] = 0;
  s_structureNamesMap["arduino-randomSeed()"] = 0;
  s_structureNamesMap["arduino-return"] = 0;
  s_structureNamesMap["arduino-right shift (>>)"] = 0;
  s_structureNamesMap["arduino-shiftIn()"] = 0;
  s_structureNamesMap["arduino-shiftOut()"] = 0;
  s_structureNamesMap["arduino-sin()"] = 0;
  s_structureNamesMap["arduino-sizeof()"] = 0;
  s_structureNamesMap["arduino-sqrt()"] = 0;
  s_structureNamesMap["arduino-subtraction (-)"] = 0;
  s_structureNamesMap["arduino-subtraction (-=)"] = 0;
  s_structureNamesMap["arduino-switch case"] = 0;
  s_structureNamesMap["arduino-tan()"] = 0;
  s_structureNamesMap["arduino-tone()"] = 0;
  s_structureNamesMap["arduino-true"] = 0;
  s_structureNamesMap["arduino-while loop"] = 0;
  s_structureNamesMap["arduino-word()"] = 0;
  s_structureNamesMap["arduino-xor (^)"] = 0;
  return s_structureNamesMap.keys();
}

bool glitch_structures_arduino::containsStructure(const QString &structureName)
{
  return structureNames().contains(structureName);
}

void glitch_structures_arduino::slotCategorySelected(void)
{
  /*
  ** Let's create the tree.
  */

  QList<QListWidgetItem *> items(m_ui.categories->selectedItems());

  if(items.isEmpty())
    return;

  m_ui.tree->clear();

  QStringList list;

  switch(m_ui.categories->row(items.at(0)))
    {
    case 0:
      {
	list << "noTone()"
	     << "pulseIn()"
	     << "shiftIn()"
	     << "shiftOut()"
	     << "tone()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 1:
      {
	list << "analogRead()"
	     << "analogReference()"
	     << "analogWrite()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 2:
      {
	list << "addition (+)"
	     << "assignment (=)"
	     << "division (/)"
	     << "modulo (%)"
	     << "multiplication (*)"
	     << "subtraction (-)";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 3:
      {
	list << "bit()"
	     << "bitClear()"
	     << "bitRead()"
	     << "bitSet()"
	     << "bitWrite()"
	     << "highByte()"
	     << "lowByte()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 4:
      {
	list << "and (&)"
	     << "left shift (<<)"
	     << "not (~)"
	     << "or (|)"
	     << "right shift (>>)"
	     << "xor (^)";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 5:
      {
	list << "equal to (==)"
	     << "greater than (>)"
	     << "greater than or equal to (>=)"
	     << "less than (<)"
	     << "less than or equal to (<=)"
	     << "not equal to (!=)";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 6:
      {
	list << "addition (+=)"
	     << "bitwise and (&=)"
	     << "bitwise or (|=)"
	     << "decrement (--)"
	     << "division (/=)"
	     << "increment (++)"
	     << "modulo (%=)"
	     << "multiplication (*=)"
	     << "subtraction (-=)";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 7:
      {
	list << "HIGH"
	     << "INPUT"
	     << "INPUT_PULLUP"
	     << "LED_BUILTIN"
	     << "LOW"
	     << "OUTPUT"
	     << "false"
	     << "true";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 8:
      {
	list << "byte()"
	     << "char()"
	     << "float()"
	     << "int()"
	     << "long()"
	     << "word()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 9:
      {
	list << "digitalRead()"
	     << "digitalWrite()"
	     << "pinMode()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 10:
      {
	list << "attachInterrupt()"
	     << "detachInterrupt()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 11:
      {
	list << "break"
	     << "continue"
	     << "do while loop"
	     << "for loop"
	     << "goto"
	     << "if statement"
	     << "if-else statement"
	     << "return"
	     << "switch case"
	     << "while loop";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 12:
      {
	list << "interrupts()"
	     << "noInterrupts()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 13:
      {
	list << "and (&&)"
	     << "not (!)"
	     << "or (||)";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 14:
      {
	list << "abs()"
	     << "constrain()"
	     << "map()"
	     << "max()"
	     << "min()"
	     << "pow()"
	     << "sqrt()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 15:
      {
	list << "random()"
	     << "randomSeed()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 16:
      {
	list << "Serial.begin()"
	     << "Serial.println()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 17:
      {
	list << "block comment"
	     << "function()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 18:
      {
	list << "delay()"
	     << "delayMicroseconds()"
	     << "micros()"
	     << "millis()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 19:
      {
	list << "cos()"
	     << "sin()"
	     << "tan()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 20:
      {
	list << "PROGMEM"
	     << "sizeof()";

	for(int i = 0; i < list.size(); i++)
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    case 21:
      {
	list << "array"
	     << "boolean"
	     << "byte"
	     << "char"
	     << "double"
	     << "float"
	     << "int"
	     << "long"
	     << "short"
	     << "unsigned char"
	     << "unsigned int"
	     << "unsigned long"
	     << "word";

	for(int i = 0; i < list.size(); i++)
	  if(list.first() == "array")
	    {
	      auto *item = new QTreeWidgetItem(QStringList() << list.at(i));

	      m_ui.tree->addTopLevelItem(item);

	      for(int i = 0; i < list.size(); i++)
		item->addChild
		  (new QTreeWidgetItem(QStringList() << list.at(i)));
	    }
	  else
	    m_ui.tree->addTopLevelItem
	      (new QTreeWidgetItem(QStringList() << list.at(i)));

	break;
      }
    default:
      {
	break;
      }
    }

  m_ui.tree->expandAll();
}
