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
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("GlowBot: Arduino Structures"));

  /*
  ** Let's create the tree.
  */

  QStringList list;
  QTreeWidgetItem *child = 0;
  QTreeWidgetItem *item = 0;

  item = new QTreeWidgetItem(QStringList() << tr("Advanced I/O"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "noTone()"
       << "pulseIn()"
       << "shiftIn()"
       << "shiftOut()"
       << "tone()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Analog I/O"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "analogRead()"
       << "analogReference()"
       << "analogWrite()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Arithmetic Operators"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "addition (+)"
       << "assignment (=)"
       << "division (/)"
       << "modulo (%)"
       << "multiplication (*)"
       << "subtraction (-)";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Bits and Bytes"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "bit()"
       << "bitClear()"
       << "bitRead()"
       << "bitSet()"
       << "bitWrite()"
       << "highByte()"
       << "lowByte()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Bitwise Operators"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "and (&)"
       << "left shift (<<)"
       << "not (~)"
       << "or (|)"
       << "right shift (>>)"
       << "xor (^)";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Comparison Operators"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "equal to (==)"
       << "greater than (>)"
       << "greater than or equal to (>=)"
       << "less than (<)"
       << "less than or equal to (<=)"
       << "not equal to (!=)";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Compound Operators"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
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
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Constants"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "HIGH"
       << "INPUT"
       << "INPUT_PULLUP"
       << "LED_BUILTIN"
       << "LOW"
       << "OUTPUT"
       << "false"
       << "true";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Conversions"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "byte()"
       << "char()"
       << "float()"
       << "int()"
       << "long()"
       << "word()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Digital I/O"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "digitalRead()"
       << "digitalWrite()"
       << "pinMode()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("External Interrupts"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "attachInterrupt()"
       << "detachInterrupt()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Flow Control"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
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
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Logical Operators"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "and (&&)"
       << "not (!)"
       << "or (||)";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Mathematics"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "abs()"
       << "constrain()"
       << "map()"
       << "max()"
       << "min()"
       << "pow()"
       << "sqrt()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Random"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "random()"
       << "randomSeed()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Serial"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "Serial.begin()"
       << "Serial.println()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Structures"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "block comment"
       << "function()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Time"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "delay()"
       << "delayMicroseconds()"
       << "micros()"
       << "millis()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Trigonometry"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "cos()"
       << "sin()"
       << "tan()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Utilities"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << "PROGMEM"
       << "sizeof()";

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Variables"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
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
    {
      if(list.first() == "array")
	{
	  child = new QTreeWidgetItem(QStringList() << list.takeFirst());

	  for(int i = 0; i < list.size(); i++)
	    child->addChild(new QTreeWidgetItem(QStringList() << list.at(i)));
	}
      else
	child = new QTreeWidgetItem(QStringList() << list.takeFirst());

      item->addChild(child);
    }

  m_ui.tree->sortItems(0, Qt::AscendingOrder);
  m_ui.tree->expandAll();
}

glowbot_structures_arduino::~glowbot_structures_arduino()
{
}
