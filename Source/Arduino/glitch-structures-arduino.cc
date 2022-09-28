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

QMap<QString, QStringList> glitch_structures_arduino::s_itemsForCategories;
QMap<QString, char> glitch_structures_arduino::s_structureNamesMap;
QStringList glitch_structures_arduino::s_nonArrayVariableTypes;
QStringList glitch_structures_arduino::s_variableTypes;

glitch_structures_arduino::glitch_structures_arduino(QWidget *parent):
  QDialog(parent)
{
  m_filterTimer.setSingleShot(true);
  m_ui.setupUi(this);
  connect(&m_filterTimer,
	  &QTimer::timeout,
	  this,
	  &glitch_structures_arduino::slotFilter);
  connect(m_ui.categories,
	  &QListWidget::itemSelectionChanged,
	  this,
	  &glitch_structures_arduino::slotCategorySelected);
  connect(m_ui.close,
	  &QPushButton::clicked,
	  this,
	  &glitch_structures_arduino::close);
  connect(m_ui.filter,
	  SIGNAL(textChanged(const QString &)),
	  this,
	  SLOT(slotFilterTextChanged(void)));
  m_ui.close->setIcon(QIcon::fromTheme("window-close"));
  m_ui.tree->setProjectType(glitch_common::ProjectTypes::ArduinoProject);
  m_ui.tree->sortItems(0, Qt::AscendingOrder);
  new QShortcut(tr("Ctrl+F"),
		m_ui.filter,
		SLOT(setFocus(void)));
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(close(void)));
  prepareCategories();
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("Glitch: Arduino Structures"));
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

  QMapIterator<QString, QStringList> it(s_itemsForCategories);

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
  /*
  ** https://www.arduino.cc/en/Reference/HomePage
  */

  m_categoriesMap[tr("Advanced I/O")] = "Advanced I/O";
  m_categoriesMap[tr("Analog I/O")] = "Analog I/O";
  m_categoriesMap[tr("Arithmetic Operators")] = "Arithmetic Operators";
  m_categoriesMap[tr("Bits and Bytes")] = "Bits and Bytes";
  m_categoriesMap[tr("Bitwise Operators")] = "Bitwise Operators";
  m_categoriesMap[tr("Boolean Operators")] = "Boolean Operators";
  m_categoriesMap[tr("Characters")] = "Characters";
  m_categoriesMap[tr("Compound Operators")] = "Compound Operators";
  m_categoriesMap[tr("Constants")] = "Constants";
  m_categoriesMap[tr("Conversions")] = "Conversions";
  m_categoriesMap[tr("Digital I/O")] = "Digital I/O";
  m_categoriesMap[tr("External Interrupts")] = "External Interrupts";
  m_categoriesMap[tr("Flow Control")] = "Flow Control";
  m_categoriesMap[tr("Interrupts")] = "Interrupts";
  m_categoriesMap[tr("Logical Operators")] = "Logical Operators";
  m_categoriesMap[tr("Mathematics")] = "Mathematics";
  m_categoriesMap[tr("Random")] = "Random";
  m_categoriesMap[tr("Serial")] = "Serial";
  m_categoriesMap[tr("Stream")] = "Stream";
  m_categoriesMap[tr("Structures")] = "Structures";
  m_categoriesMap[tr("Syntax")] = "Syntax";
  m_categoriesMap[tr("Time")] = "Time";
  m_categoriesMap[tr("Trigonometry")] = "Trigonometry";
  m_categoriesMap[tr("Utilities")] = "Utilities";
  m_categoriesMap[tr("Variables")] = "Variables";
  m_ui.categories->clear();
  m_ui.categories->addItems(m_categoriesMap.keys());

  for(int i = 0; i < m_ui.categories->count(); i++)
    {
      auto item = m_ui.categories->item(i);

      if(item)
	item->setData(Qt::UserRole, m_categoriesMap.value(item->text()));
    }

  s_itemsForCategories["Advanced I/O"] = QStringList() << "noTone()"
						       << "pulseIn()"
						       << "pulseInLong()"
						       << "shiftIn()"
						       << "shiftOut()"
						       << "tone()";
  s_itemsForCategories["Analog I/O"] = QStringList() << "analogRead()"
						     << "analogReference()"
						     << "analogWrite()";
  s_itemsForCategories["Arithmetic Operators"] =
    QStringList() << "addition (+)"
		  << "division (/)"
		  << "modulo (%)"
		  << "multiplication (*)"
		  << "subtraction (-)";
  s_itemsForCategories["Bits and Bytes"] = QStringList() << "bit()"
							 << "bitClear()"
							 << "bitRead()"
							 << "bitSet()"
							 << "bitWrite()"
							 << "highByte()"
							 << "lowByte()";
  s_itemsForCategories["Bitwise Operators"] =
    QStringList() << "and (&)"
		  << "left shift (<<)"
		  << "not (~)"
		  << "or (|)"
		  << "right shift (>>)"
		  << "xor (^)";
  s_itemsForCategories["Boolean Operators"] =
    QStringList() << "and (&&)"
		  << "not (!)"
		  << "or (||)";
  s_itemsForCategories["Characters"] = QStringList() << "isAlpha()"
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
  s_itemsForCategories["Compound Operators"] =
    QStringList() << "addition (+=)"
		  << "bitwise and (&=)"
		  << "bitwise or (|=)"
		  << "bitwise xor (^=)"
		  << "decrement (--)"
		  << "division (/=)"
		  << "increment (++)"
		  << "modulo (%=)"
		  << "multiplication (*=)"
		  << "subtraction (-=)";
  s_itemsForCategories["Constants"] = QStringList() << "HIGH"
						    << "INPUT"
						    << "INPUT_PULLUP"
						    << "LED_BUILTIN"
						    << "LOW"
						    << "OUTPUT"
						    << "Other"
						    << "false"
						    << "true";
  s_itemsForCategories["Conversions"] = QStringList() << "(unsigned int)"
						      << "(unsigned long)"
						      << "byte()"
						      << "char()"
						      << "float()"
						      << "int()"
						      << "long()"
						      << "word()";
  s_itemsForCategories["Digital I/O"] = QStringList() << "digitalRead()"
						      << "digitalWrite()"
						      << "pinMode()";
  s_itemsForCategories["External Interrupts"] =
    QStringList() << "attachInterrupt()"
		  << "detachInterrupt()";
  s_itemsForCategories["Flow Control"] = QStringList() << "break"
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
  s_itemsForCategories["Interrupts"] = QStringList() << "interrupts()"
						     << "noInterrupts()";
  s_itemsForCategories["Logical Operators"] = QStringList() << "and (&&)"
							    << "not (!)"
							    << "or (||)";
  s_itemsForCategories["Mathematics"] = QStringList() << "abs()"
						      << "constrain()"
						      << "map()"
						      << "max()"
						      << "min()"
						      << "pow()"
						      << "sq()"
						      << "sqrt()";
  s_itemsForCategories["Random"] = QStringList() << "random()"
						 << "randomSeed()";
  s_itemsForCategories["Serial"] = QStringList() << "Serial.available()"
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
  s_itemsForCategories["Stream"] = QStringList() << "stream.available()"
						 << "stream.find()"
						 << "stream.findUntil()"
						 << "stream.flush()"
						 << "stream.parseFloat()"
						 << "stream.parseInt()"
						 << "stream.peek()"
						 << "stream.read()"
						 << "stream.readBytes()"
						 << "stream.readBytesUntil()"
						 << "stream.readString()"
						 << "stream.readStringUntil()"
						 << "stream.setTimeout()";
  s_itemsForCategories["Structures"] = QStringList() << "block comment"
						     << "function()";
  s_itemsForCategories["Syntax"] = QStringList() << "#define"
						 << "#include";
  s_itemsForCategories["Time"] = QStringList() << "delay()"
					       << "delayMicroseconds()"
					       << "micros()"
					       << "millis()";
  s_itemsForCategories["Trigonometry"] = QStringList() << "cos()"
						       << "sin()"
						       << "tan()";
  s_itemsForCategories["Utilities"] = QStringList() << "sizeof()";

  QStringList arrays;
  auto list(variableTypes());

  list.removeOne("array");

  foreach(const auto &i, list)
    {
      arrays << "array " + i;
      arrays << i;
    }

  s_itemsForCategories["Variables"] = arrays;
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
  auto string(m_categoriesMap.value(items.at(0)->text()));

  if(string != "Variables")
    {
      list = s_itemsForCategories.value(string);

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

void glitch_structures_arduino::slotFilter(void)
{
  QListWidgetItem *found = nullptr;
  QRegularExpression rx
    (
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
     "*" + m_ui.filter->text().toLower().trimmed() + "*",
#else
     QRegularExpression::
     wildcardToRegularExpression("*" +
				 m_ui.filter->text().toLower().trimmed() +
				 "*"),
#endif
     QRegularExpression::CaseInsensitiveOption);
  auto string(m_ui.filter->text().trimmed());

  for(int i = 0; i < m_ui.categories->count(); i++)
    {
      auto item = m_ui.categories->item(i);

      if(!item)
	continue;

      if(string.isEmpty())
	{
	  if(!found)
	    found = item;

	  item->setHidden(false);
	  continue;
	}

      if(item->text().contains(string, Qt::CaseInsensitive))
	item->setHidden(false);
      else
	item->setHidden
	  (s_itemsForCategories.value(item->text()).indexOf(rx) == -1);

      if(!found && !item->isHidden())
	found = item;
    }

  if(found)
    {
      found->setSelected(true);
      slotCategorySelected();
    }
  else
    {
      m_ui.tree->clear();
      m_ui.tree->setHeaderLabel("");
    }
}

void glitch_structures_arduino::slotFilterTextChanged(void)
{
  m_filterTimer.start(250);
}
