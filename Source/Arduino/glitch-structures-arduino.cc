/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
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

#include <QToolButton>

#include "glitch-collapse-expand-tool-button.h"
#include "glitch-structures-arduino.h"

QMap<QString, QString> glitch_structures_arduino::s_itemsForIcons;
QMap<QString, QStringList> glitch_structures_arduino::s_itemsForCategories;
QMap<QString, char> glitch_structures_arduino::s_structureNamesMap;
QStringList glitch_structures_arduino::s_nonArrayVariableTypes;
QStringList glitch_structures_arduino::s_variableTypes;

glitch_structures_arduino::glitch_structures_arduino(QWidget *parent):
  QDialog(parent)
{
  m_filterTimer.setSingleShot(true);
  m_ui.setupUi(this);
  connect(&m_filterTimer, // Legacy connect(). Please do not replace!
	  SIGNAL(timeout(void)),
	  this,
	  SLOT(slotFilter(void)));
  connect(m_ui.filter,
	  SIGNAL(textChanged(const QString &)),
	  this,
	  SLOT(slotFilter(const QString &)));
  m_collapse = new glitch_collapse_expand_tool_button(m_ui.tree);
  m_ui.tree->setMinimumWidth(200);
  m_ui.tree->setProjectType(glitch_common::ProjectTypes::ArduinoProject);
  m_ui.tree->setIconSize(QSize(24, 24));
  m_ui.tree->sortItems(0, Qt::AscendingOrder);
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("Glitch: Arduino Structures"));
}

glitch_structures_arduino::~glitch_structures_arduino()
{
}

QFrame *glitch_structures_arduino::frame(void) const
{
  return m_ui.frame;
}

QHash<QString, QColor> glitch_structures_arduino::defaultColors(void)
{
  QHash<QString, QColor> hash;

  foreach(const auto &i, keywords())
    if(i.endsWith("()"))
      hash[i] = QColor("#cc6600");
    else if(i.startsWith('/'))
      hash[i] = QColor("#cc6600");
    else
      hash[i] = QColor("#024f8b");

  return hash;
}

QStringList glitch_structures_arduino::keywords(void)
{
  QMapIterator<QString, QStringList> it(s_itemsForCategories);
  QStringList list;

  while(it.hasNext())
    {
      it.next();

      /*
      ** Please do not translate!
      */

      if(it.key() == "Decorations" ||
	 it.key() == "Arithmetic Operators" ||
	 it.key() == "Bitwise Operators" ||
	 it.key() == "Boolean Operators" ||
	 it.key() == "Compound Operators")
	continue;

      for(int i = 0; i < it.value().size(); i++)
	if(!it.value().at(i).startsWith("Other") &&
	   !it.value().at(i).startsWith("array") &&
	   !it.value().at(i).startsWith("block comment") &&
	   !it.value().at(i).startsWith("function()"))
	  list << it.value().at(i);
    }

  /*
  ** Please do not translate!
  */

  list << "//"
       << "const"
       << "loop()"
       << "setup()"
       << "static"
       << "volatile";
  return list;
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

QStringList glitch_structures_arduino::sortedKeywords(void)
{
  auto list(keywords());

  std::sort(list.begin(), list.end());
  return list;
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

  s_variableTypes << "BitOrder"
		  << "PinStatus"
		  << "String"
		  << "array"
		  << "auto"
		  << "bool"
		  << "boolean"
		  << "byte"
		  << "char"
		  << "double"
		  << "float"
		  << "int"
		  << "long"
		  << "pin_size_t"
		  << "short"
		  << "size_t"
		  << "string"
		  << "uint8_t"
		  << "uint16_t"
		  << "uint32_t"
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

bool glitch_structures_arduino::isReserved(const QString &t)
{
  QMapIterator<QString, QStringList> it(s_itemsForCategories);
  auto text(QString(t).remove('(').remove(')').trimmed());

  while(it.hasNext())
    {
      it.next();

      /*
      ** Please do not translate!
      */

      if(it.key() == "Catalog" || it.key() == "Decorations")
	continue;

      for(int i = 0; i < it.value().size(); i++)
	if(it.value().at(i) == text)
	  return true;
    }

  if(text == "const" ||
     text == "loop" ||
     text == "setup" ||
     text == "static" ||
     text == "volatile")
    return true;

  return false;
}

void glitch_structures_arduino::prepareCategories(void)
{
  if(m_categoriesMap.size() > 0)
    return;

  /*
  ** https://www.arduino.cc/en/Reference/HomePage
  */

  m_categoriesMap[tr("Advanced I/O")] = "Advanced I/O";
  m_categoriesMap[tr("Analog I/O")] = "Analog I/O";
  m_categoriesMap[tr("Arithmetic Operators")] = "Arithmetic Operators";
  m_categoriesMap[tr("Bits and Bytes")] = "Bits and Bytes";
  m_categoriesMap[tr("Bitwise Operators")] = "Bitwise Operators";
  m_categoriesMap[tr("Boolean Operators")] = "Boolean Operators";
  m_categoriesMap[tr("Catalog")] = "Catalog";
  m_categoriesMap[tr("Characters")] = "Characters";
  m_categoriesMap[tr("Compound Operators")] = "Compound Operators";
  m_categoriesMap[tr("Constants")] = "Constants";
  m_categoriesMap[tr("Conversions")] = "Conversions";
  m_categoriesMap[tr("Decorations")] = "Decorations"; // Not Arduino.
  m_categoriesMap[tr("Digital I/O")] = "Digital I/O";
  m_categoriesMap[tr("External Interrupts")] = "External Interrupts";
  m_categoriesMap[tr("Flow Control")] = "Flow Control";
  m_categoriesMap[tr("Interrupts")] = "Interrupts";
  m_categoriesMap[tr("Mathematics")] = "Mathematics";
  m_categoriesMap[tr("Random")] = "Random";
  m_categoriesMap[tr("Serial")] = "Serial";
  m_categoriesMap[tr("Structures")] = "Structures";
  m_categoriesMap[tr("Syntax")] = "Syntax";
  m_categoriesMap[tr("Time")] = "Time";
  m_categoriesMap[tr("Trigonometry")] = "Trigonometry";
  m_categoriesMap[tr("Utilities")] = "Utilities";
  m_categoriesMap[tr("Variables")] = "Variables";
  m_ui.tree->clear();
  m_ui.tree->setHeaderLabels(QStringList() << tr("Categories"));
  s_itemsForCategories["Advanced I/O"] = QStringList() << "noTone()"
						       << "pulseIn()"
						       << "pulseInLong()"
						       << "shiftIn()"
						       << "shiftOut()"
						       << "tone()";
  s_itemsForIcons["Advanced I/O"] = "pulse.png";
  s_itemsForCategories["Analog I/O"] =
    QStringList() << "analogRead()"
		  << "analogReadResolution()"
		  << "analogReference()"
		  << "analogWrite()";
  s_itemsForIcons["Analog I/O"] = "analog.png";
  s_itemsForCategories["Arithmetic Operators"] = QStringList() << "+"
							       << "/"
							       << "%"
							       << "*"
							       << "-";
  s_itemsForIcons["Arithmetic Operators"] = "arithmetic.png";
  s_itemsForCategories["Bits and Bytes"] = QStringList() << "bit()"
							 << "bitClear()"
							 << "bitRead()"
							 << "bitSet()"
							 << "bitWrite()"
							 << "highByte()"
							 << "lowByte()";
  s_itemsForIcons["Bits and Bytes"] = "binary.png";
  s_itemsForCategories["Bitwise Operators"] = QStringList() << "&"
							    << "<<"
							    << "~"
							    << "|"
							    << ">>"
							    << "^";
  s_itemsForIcons["Bitwise Operators"] = "xor.png";
  s_itemsForCategories["Boolean Operators"] = QStringList() << "&&"
							    << "!"
							    << "||";
  s_itemsForIcons["Boolean Operators"] = "boolean.png";
  s_itemsForCategories["Catalog"] = QStringList() << "method";
  s_itemsForIcons["Catalog"] = "catalog.png";
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
  s_itemsForIcons["Characters"] = "ascii.png";
  s_itemsForCategories["Compound Operators"] = QStringList() << "+="
							     << "&="
							     << "|="
							     << "^="
							     << "--"
							     << "/="
							     << "++"
							     << "%="
							     << "*="
							     << "-=";
  s_itemsForIcons["Compound Operators"] = "compound.png";
  s_itemsForCategories["Constants"] = QStringList() << "A0"
						    << "A1"
						    << "A2"
						    << "A3"
						    << "A4"
						    << "A5"
						    << "A6"
						    << "A7"
						    << "A8"
						    << "A9"
						    << "A10"
						    << "A11"
						    << "A12"
						    << "A13"
						    << "A14"
						    << "A15"
    << "HIGH"
						    << "INPUT"
						    << "INPUT_PULLUP"
						    << "LED_BUILTIN"
						    << "LOW"
						    << "OUTPUT"
						    << "Other"
						    << "false"
						    << "true";
  s_itemsForIcons["Constants"] = "constant.png";
  s_itemsForCategories["Conversions"] = QStringList() << "(uint8_t)"
						      << "(uint16_t)"
						      << "(uint32_t)"
						      << "(unsigned int)"
						      << "(unsigned long)"
						      << "byte()"
						      << "char()"
						      << "float()"
						      << "int()"
						      << "long()"
						      << "word()";
  s_itemsForIcons["Conversions"] = "convert.png";
  s_itemsForCategories["Decorations"] =
    QStringList() << "horizontal arrow"
		  << "horizontal arrow (left)"
		  << "horizontal arrow (right)";
  s_itemsForIcons["Decorations"] = "decoration.png";
  s_itemsForCategories["Digital I/O"] = QStringList() << "digitalRead()"
						      << "digitalWrite()"
						      << "pinMode()";
  s_itemsForIcons["Digital I/O"] = "digital.png";
  s_itemsForCategories["External Interrupts"] =
    QStringList() << "attachInterrupt()"
		  << "detachInterrupt()";
  s_itemsForIcons["External Interrupts"] = "interrupt.png";
  s_itemsForCategories["Flow Control"] = QStringList() << "break"
						       << "case"
						       << "continue"
						       << "default"
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
  s_itemsForIcons["Flow Control"] = "flow.png";
  s_itemsForCategories["Interrupts"] = QStringList() << "interrupts()"
						     << "noInterrupts()";
  s_itemsForIcons["Interrupts"] = "interrupt.png";
  s_itemsForCategories["Mathematics"] = QStringList() << "abs()"
						      << "constrain()"
						      << "map()"
						      << "max()"
						      << "min()"
						      << "pow()"
						      << "sq()"
						      << "sqrt()";
  s_itemsForIcons["Mathematics"] = "mathematics.png";
  s_itemsForCategories["Random"] = QStringList() << "random()"
						 << "randomSeed()";
  s_itemsForIcons["Random"] = "random.png";
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
  s_itemsForIcons["Serial"] = "serial.png";
  s_itemsForCategories["Structures"] = QStringList() << "block comment"
						     << "function()";
  s_itemsForIcons["Structures"] = "structure.png";
  s_itemsForCategories["Syntax"] = QStringList() << "#define"
						 << "#include"
						 << "Other";
  s_itemsForIcons["Syntax"] = "syntax.png";
  s_itemsForCategories["Time"] = QStringList() << "delay()"
					       << "delayMicroseconds()"
					       << "micros()"
					       << "millis()";
  s_itemsForIcons["Time"] = "time.png";
  s_itemsForCategories["Trigonometry"] = QStringList() << "cos()"
						       << "sin()"
						       << "tan()";
  s_itemsForIcons["Trigonometry"] = "trigonometry.png";
  s_itemsForCategories["Utilities"] = QStringList() << "sizeof()";
  s_itemsForIcons["Utilities"] = "utilities.png";
  s_itemsForCategories["Variables"] = nonArrayVariableTypes();
  s_itemsForIcons["Variables"] = "variable.png";

  QMapIterator<QString, QString> it(m_categoriesMap);

  while(it.hasNext())
    {
      it.next();

      auto item = new QTreeWidgetItem(m_ui.tree);

      item->setIcon
	(0,
	 QIcon(QString(":/Arduino/%1").arg(s_itemsForIcons.value(it.key()))));
      item->setText(0, it.key());
      m_ui.tree->addTopLevelItem(item);

      for(int i = 0; i < s_itemsForCategories.value(it.value()).size(); i++)
	{
	  auto child = new QTreeWidgetItem(item);

	  child->setData(0,
			 Qt::UserRole,
			 "glitch-arduino-" +
			 it.value() +
			 "-" +
			 s_itemsForCategories.value(it.value()).at(i));
	  child->setText(0, s_itemsForCategories.value(it.value()).at(i));

	  if(item->text(0).contains("operator", Qt::CaseInsensitive))
	    child->setTextAlignment(0, Qt::AlignCenter);
	}
    }
}

void glitch_structures_arduino::setIconSize(const QString &t)
{
  auto text(t.toLower().trimmed());

  if(text == "0x0")
    m_ui.tree->setIconSize(QSize(0, 0));
  else if(text == "24x24")
    m_ui.tree->setIconSize(QSize(24, 24));
  else if(text == "32x32")
    m_ui.tree->setIconSize(QSize(32, 32));
  else if(text == "48x48")
    m_ui.tree->setIconSize(QSize(48, 48));
  else
    m_ui.tree->setIconSize(QSize(24, 24));
}

void glitch_structures_arduino::slotFilter(const QString &text)
{
  Q_UNUSED(text);
  m_filterTimer.start(250);
}

void glitch_structures_arduino::slotFilter(void)
{
  auto text(m_ui.filter->text().trimmed());

  for(int i = 0; i < m_ui.tree->topLevelItemCount(); i++)
    {
      auto child = m_ui.tree->topLevelItem(i);

      if(!child)
	continue;

      child->setHidden(!text.isEmpty());

      for(int j = 0; j < child->childCount(); j++)
	{
	  auto item = child->child(j);

	  if(!item)
	    continue;

	  if(item->text(0).contains(text, Qt::CaseInsensitive) ||
	     text.isEmpty())
	    {
	      if(!text.isEmpty())
		child->setExpanded(true);

	      child->setHidden(false);
	      item->setHidden(false);
	    }
	  else if(!text.isEmpty())
	    item->setHidden(true);
	}
    }
}
