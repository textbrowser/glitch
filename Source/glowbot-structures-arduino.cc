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
  m_ui.setupUi(this);
  setWindowModality(Qt::NonModal);
  setWindowTitle(tr("GlowBot: Arduino Structures"));

  /*
  ** Let's create the tree.
  */

  QStringList list;
  QTreeWidgetItem *child = 0;
  QTreeWidgetItem *item = 0;

  item = new QTreeWidgetItem(QStringList() << tr("Arithmetic"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << tr("addition (+)")
       << tr("division (/)")
       << tr("multiplication (*)")
       << tr("subtraction (-)");

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Comparison Operators"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << tr("equal to (==)")
       << tr("greater than (>)")
       << tr("greater than or equal to (>=)")
       << tr("less than (<)")
       << tr("less than or equal to (<=)")
       << tr("not equal to (!=)");

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Constants"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << tr("FALSE")
       << tr("HIGH")
       << tr("INPUT")
       << tr("LOW")
       << tr("OUTPUT")
       << tr("TRUE");

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Logical Operators"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << tr("and (&&)")
       << tr("not (!)")
       << tr("or (||)");

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Structures"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << tr("block comment") << tr("function");

  while(!list.isEmpty())
    {
      child = new QTreeWidgetItem(QStringList() << list.takeFirst());
      item->addChild(child);
    }

  item = new QTreeWidgetItem(QStringList() << tr("Variables"));
  m_ui.tree->addTopLevelItem(item);
  list.clear();
  list << tr("array")
       << tr("byte")
       << tr("float")
       << tr("int")
       << tr("long");

  while(!list.isEmpty())
    {
      if(list.first() == tr("array"))
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
