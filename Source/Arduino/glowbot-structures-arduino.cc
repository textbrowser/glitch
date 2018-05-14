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

#include "glowbot-structures-arduino.h"

glowbot_structures_arduino::glowbot_structures_arduino(QWidget *parent):
  QDialog(parent)
{
  /*
  ** https://www.arduino.cc/en/Reference/HomePage
  */

  m_ui.setupUi(this);
  m_ui.tree->setProjectType(glowbot_common::ArduinoProject);
  connect(m_ui.categories,
	  SIGNAL(itemSelectionChanged(void)),
	  this,
	  SLOT(slotCategorySelected(void)));
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("GlowBot: Arduino Structures"));

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

glowbot_structures_arduino::~glowbot_structures_arduino()
{
}

void glowbot_structures_arduino::slotCategorySelected(void)
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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 1:
      {
	list << "analogRead()"
	     << "analogReference()"
	     << "analogWrite()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 9:
      {
	list << "digitalRead()"
	     << "digitalWrite()"
	     << "pinMode()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 10:
      {
	list << "attachInterrupt()"
	     << "detachInterrupt()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 12:
      {
	list << "interrupts()"
	     << "noInterrupts()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 13:
      {
	list << "and (&&)"
	     << "not (!)"
	     << "or (||)";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 15:
      {
	list << "random()"
	     << "randomSeed()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 16:
      {
	list << "Serial.begin()"
	     << "Serial.println()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 17:
      {
	list << "block comment"
	     << "function()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 18:
      {
	list << "delay()"
	     << "delayMicroseconds()"
	     << "micros()"
	     << "millis()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 19:
      {
	list << "cos()"
	     << "sin()"
	     << "tan()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    case 20:
      {
	list << "PROGMEM"
	     << "sizeof()";

	while(!list.isEmpty())
	  m_ui.tree->addTopLevelItem
	    (new QTreeWidgetItem(QStringList() << list.takeFirst()));

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

	while(!list.isEmpty())
	  if(list.first() == "array")
	    {
	      QTreeWidgetItem *item = new QTreeWidgetItem
		(QStringList() << list.takeFirst());

	      m_ui.tree->addTopLevelItem(item);

	      for(int i = 0; i < list.size(); i++)
		item->addChild
		  (new QTreeWidgetItem(QStringList() << list.at(i)));
	    }
	  else
	    m_ui.tree->addTopLevelItem
	      (new QTreeWidgetItem(QStringList() << list.takeFirst()));

	break;
      }
    default:
      {
	break;
      }
    }

  m_ui.tree->expandAll();
}
