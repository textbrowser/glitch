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

#include <QScrollBar>
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
#include <QSerialPort>
#include <QSerialPortInfo>
#endif

#include "glitch-serial-port-window.h"

glitch_serial_port_window::glitch_serial_port_window(QWidget *parent):
  QDialog(parent)
{
  m_ui.setupUi(this);
  connect(&m_timer,
	  &QTimer::timeout,
	  this,
	  &glitch_serial_port_window::slotDiscoverDevices);
  connect(m_ui.clear,
	  &QPushButton::clicked,
	  m_ui.communications,
	  &QPlainTextEdit::clear);
  connect(m_ui.connect,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotConnect);
  connect(m_ui.disconnect,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotDisconnect);
  connect(m_ui.send,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotSend);

#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  m_serialPort = nullptr;
  m_timer.start(2500);
  m_ui.disconnect->setEnabled(false);
  m_ui.send->setEnabled(false);
  slotDiscoverDevices();
#endif
}

glitch_serial_port_window::~glitch_serial_port_window()
{
  m_timer.stop();
}

void glitch_serial_port_window::closeEvent(QCloseEvent *event)
{
  QDialog::closeEvent(event);
  m_timer.stop();
}

void glitch_serial_port_window::showEvent(QShowEvent *event)
{
  QDialog::showEvent(event);
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  m_timer.start();
#else
  m_timer.stop();
#endif
}

void glitch_serial_port_window::slotConnect(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  if(m_serialPort)
    {
      m_serialPort->close();
      m_serialPort->deleteLater();
    }

  m_serialPort = new QSerialPort(m_ui.port_name->currentText(), this);

  if(!m_serialPort->open(QIODevice::ReadWrite))
    return;

  connect(m_serialPort,
	  &QSerialPort::readyRead,
	  this,
	  &glitch_serial_port_window::slotReadyRead);
  m_serialPort->setBaudRate
    (static_cast<qint32> (m_ui.baud_rate_input->currentText().toInt()),
     QSerialPort::Input);
  m_serialPort->setBaudRate
    (static_cast<qint32> (m_ui.baud_rate_output->currentText().toInt()),
     QSerialPort::Output);
  m_serialPort->setBreakEnabled(m_ui.break_enabled->isChecked());
  m_serialPort->setDataBits
    (QSerialPort::DataBits(m_ui.data_bits->currentIndex() + 5));
  m_serialPort->setDataTerminalReady(m_ui.data_terminal_ready->isChecked());

  switch(m_ui.flow_control->currentIndex())
    {
    case 0:
      {
	m_serialPort->setFlowControl(QSerialPort::HardwareControl);
	break;
      }
    case 2:
      {
	m_serialPort->setFlowControl(QSerialPort::SoftwareControl);
	break;
      }
    default:
      {
	m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
	break;
      }
    }

  switch(m_ui.parity->currentIndex())
    {
    case 0:
      {
	m_serialPort->setParity(QSerialPort::EvenParity);
	break;
      }
    case 1:
      {
	m_serialPort->setParity(QSerialPort::MarkParity);
	break;
      }
    case 2:
      {
	m_serialPort->setParity(QSerialPort::NoParity);
	break;
      }
    case 3:
      {
	m_serialPort->setParity(QSerialPort::OddParity);
	break;
      }
    case 4:
      {
	m_serialPort->setParity(QSerialPort::SpaceParity);
	break;
      }
    }

  m_serialPort->setRequestToSend(m_ui.rts->isChecked());

  switch(m_ui.stop_bits->currentIndex())
    {
    case 0:
      {
	m_serialPort->setStopBits(QSerialPort::OneStop);
	break;
      }
    case 1:
      {
	m_serialPort->setStopBits(QSerialPort::OneAndHalfStop);
	break;
      }
    case 2:
      {
	m_serialPort->setStopBits(QSerialPort::TwoStop);
	break;
      }
    }

  m_ui.connect->setEnabled(false);
  m_ui.disconnect->setEnabled(true);
  m_ui.send->setEnabled(true);
#endif
}

void glitch_serial_port_window::slotDisconnect(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  if(m_serialPort)
    {
      m_serialPort->close();
      m_serialPort->deleteLater();
      m_serialPort = nullptr;
    }

  m_ui.connect->setEnabled(true);
  m_ui.disconnect->setEnabled(false);
  m_ui.send->setEnabled(false);
#endif
}

void glitch_serial_port_window::slotDiscoverDevices(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  QMap<QString, char> map;

  foreach(const auto &port, QSerialPortInfo::availablePorts())
    map[port.portName()] = 0;

  QMapIterator<QString, char> it(map);
  int i = -1;

  while(it.hasNext())
    {
      i += 1;
      it.next();

      if(m_ui.port_name->findText(it.key()) == -1)
	m_ui.port_name->insertItem(i, it.key());
    }

  for(int i = m_ui.port_name->count() - 1; i >= 0; i--)
    if(!map.contains(m_ui.port_name->itemText(i)))
      {
	if(m_serialPort &&
	   m_serialPort->portName() == m_ui.port_name->itemText(i))
	  {
	    m_ui.port_name->setCurrentIndex(0);
	    slotDisconnect();
	  }

	m_ui.port_name->removeItem(i);
      }

  if(m_ui.port_name->count() == 0)
    m_ui.port_name->addItem("/dev/null"); // Do not translate.
#endif
}

void glitch_serial_port_window::slotReadyRead(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  if(m_serialPort)
    {
      auto scrollBar = m_ui.communications->verticalScrollBar();

      while(m_serialPort->bytesAvailable() > 0)
	{
	  if(m_serialPort->canReadLine())
	    {
	      auto bytes(m_serialPort->readLine());

	      if(!bytes.isEmpty())
		{
		  m_ui.communications->insertPlainText(bytes);

		  if(scrollBar)
		    scrollBar->setValue(scrollBar->maximum());
		}
	    }
	  else
	    {
	      auto bytes(m_serialPort->readAll());

	      if(!bytes.isEmpty())
		{
		  m_ui.communications->insertPlainText(bytes);

		  if(scrollBar)
		    scrollBar->setValue(scrollBar->maximum());
		}
	    }
	}
    }
#endif
}

void glitch_serial_port_window::slotSend(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  if(m_serialPort)
    {
      m_serialPort->write(m_ui.command->toPlainText().toLatin1());
      m_serialPort->flush();
    }
#endif
}
