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

#ifdef GLITCH_SERIAL_PORT_SUPPORTED
#include <QDateTime>
#include <QSerialPortInfo>
#endif
#include <QSettings>
#include <QShortcut>
#include <QTimer>

#include "glitch-serial-port-window.h"

glitch_serial_port_window::glitch_serial_port_window(QWidget *parent):
  QDialog(parent)
{
  m_packetsReceived = 0;
  m_ui.setupUi(this);
  connect(m_ui.clear,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotClear);
  connect(m_ui.connect,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotConnect);
  connect(m_ui.disconnect,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotDisconnect);
  connect(m_ui.refresh,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotRefresh);
  connect(m_ui.send,
	  &QPushButton::clicked,
	  this,
	  &glitch_serial_port_window::slotSend);
  m_ui.clear->setIcon(QIcon(":/clear.png"));
#ifndef Q_OS_ANDROID
  new QShortcut(tr("Ctrl+W"), this, SLOT(close(void)));
#endif
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  discoverDevices();
  m_ui.disconnect->setEnabled(false);
  m_ui.send->setEnabled(false);
#endif
  m_ui.splitter_1->setStretchFactor(0, 0);
  m_ui.splitter_1->setStretchFactor(1, 1);
  m_ui.splitter_2->setStretchFactor(0, 0);
  m_ui.splitter_2->setStretchFactor(1, 1);
}

glitch_serial_port_window::~glitch_serial_port_window()
{
}

void glitch_serial_port_window::closeEvent(QCloseEvent *event)
{
  QSettings settings;

  settings.setValue("serial_port_window/geometry", saveGeometry());
  settings.setValue
    ("serial_port_window/splitter_1", m_ui.splitter_1->saveState());
  settings.setValue
    ("serial_port_window/splitter_2", m_ui.splitter_2->saveState());
  QDialog::closeEvent(event);
}

void glitch_serial_port_window::discoverDevices(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto portName(m_ui.port_name->currentText());

  m_ui.port_name->clear();

  QMap<QString, char> map;

  foreach(const auto &port, QSerialPortInfo::availablePorts())
    map[port.portName()] = 0;

  m_ui.port_name->addItems(map.keys());

  auto serialPort = findChild<QSerialPort *> ();

  if(serialPort)
    {
      if(!serialPort->isOpen() || m_ui.port_name->findText(portName) == -1)
	QTimer::singleShot(250, this, SLOT(slotDisconnect(void)));
    }
  else
    m_ui.port_name->setCurrentIndex(0);

  if(m_ui.port_name->count() == 0)
    {
      m_ui.port_name->addItem("/dev/null"); // Do not translate.
      m_ui.port_name->setCurrentIndex(0);
    }

  QApplication::restoreOverrideCursor();
#endif
}

void glitch_serial_port_window::showEvent(QShowEvent *event)
{
  QDialog::showEvent(event);

  QSettings settings;

  m_ui.splitter_1->restoreState
    (settings.value("serial_port_window/splitter_1").toByteArray());
  m_ui.splitter_2->restoreState
    (settings.value("serial_port_window/splitter_2").toByteArray());
  restoreGeometry(settings.value("serial_port_window/geometry").toByteArray());
}

void glitch_serial_port_window::slotClear(void)
{
  m_ui.communications->clear();
}

void glitch_serial_port_window::slotConnect(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto serialPort = findChild<QSerialPort *> ();

  if(serialPort)
    {
      /*
      ** Awful error!
      */

      serialPort->close();
      serialPort->deleteLater();
    }

  serialPort = new QSerialPort(m_ui.port_name->currentText(), this);

  if(!serialPort->open(QIODevice::ReadWrite))
    {
      QApplication::restoreOverrideCursor();
      QTimer::singleShot(250, this, SLOT(slotDisconnect(void)));
      return;
    }

  connect(serialPort,
	  &QSerialPort::readyRead,
	  this,
	  &glitch_serial_port_window::slotReadyRead);
  connect(serialPort,
	  SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
	  this,
	  SLOT(slotErrorOccurred(QSerialPort::SerialPortError)));
  serialPort->setBaudRate
    (static_cast<qint32> (m_ui.baud_rate_input->currentText().toInt()),
     QSerialPort::Input);
  serialPort->setBaudRate
    (static_cast<qint32> (m_ui.baud_rate_output->currentText().toInt()),
     QSerialPort::Output);
  serialPort->setBreakEnabled(m_ui.break_enabled->isChecked());
  serialPort->setDataBits
    (QSerialPort::DataBits(m_ui.data_bits->currentIndex() + 5));
  serialPort->setDataTerminalReady(m_ui.data_terminal_ready->isChecked());

  switch(m_ui.flow_control->currentIndex())
    {
    case 0:
      {
	serialPort->setFlowControl(QSerialPort::HardwareControl);
	break;
      }
    case 2:
      {
	serialPort->setFlowControl(QSerialPort::SoftwareControl);
	break;
      }
    default:
      {
	serialPort->setFlowControl(QSerialPort::NoFlowControl);
	break;
      }
    }

  switch(m_ui.parity->currentIndex())
    {
    case 0:
      {
	serialPort->setParity(QSerialPort::EvenParity);
	break;
      }
    case 1:
      {
	serialPort->setParity(QSerialPort::MarkParity);
	break;
      }
    case 2:
      {
	serialPort->setParity(QSerialPort::NoParity);
	break;
      }
    case 3:
      {
	serialPort->setParity(QSerialPort::OddParity);
	break;
      }
    case 4:
      {
	serialPort->setParity(QSerialPort::SpaceParity);
	break;
      }
    }

  serialPort->setRequestToSend(m_ui.rts->isChecked());

  switch(m_ui.stop_bits->currentIndex())
    {
    case 0:
      {
	serialPort->setStopBits(QSerialPort::OneStop);
	break;
      }
    case 1:
      {
	serialPort->setStopBits(QSerialPort::OneAndHalfStop);
	break;
      }
    case 2:
      {
	serialPort->setStopBits(QSerialPort::TwoStop);
	break;
      }
    }

  m_ui.connect->setEnabled(false);
  m_ui.disconnect->setEnabled(true);
  m_ui.send->setEnabled(true);
  m_ui.serial_port->setText(serialPort->portName());
  QApplication::restoreOverrideCursor();
#endif
}

void glitch_serial_port_window::slotDisconnect(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  auto serialPort = findChild<QSerialPort *> ();

  if(serialPort)
    {
      serialPort->close();
      serialPort->deleteLater();
    }

  m_packetsReceived = 0;
  m_ui.connect->setEnabled(true);
  m_ui.disconnect->setEnabled(false);
  m_ui.send->setEnabled(false);
  m_ui.serial_port->clear();
#endif
}

#ifdef GLITCH_SERIAL_PORT_SUPPORTED
void glitch_serial_port_window::slotErrorOccurred
(QSerialPort::SerialPortError error)
{
  auto serialPort = qobject_cast<QSerialPort *> (sender());

  if(serialPort)
    {
      m_ui.last_error->setText
	(QString("%1:%2:%3 (%4)").
	 arg(serialPort->portName()).
	 arg(QDateTime::currentDateTime().toString(Qt::ISODate)).
	 arg(serialPort->errorString().trimmed().toUpper()).
	 arg(error));
      m_ui.last_error->setCursorPosition(0);
    }

  switch(error)
    {
    case QSerialPort::DeviceNotFoundError:
    case QSerialPort::NotOpenError:
    case QSerialPort::OpenError:
    case QSerialPort::PermissionError:
    case QSerialPort::ReadError:
    case QSerialPort::ResourceError:
    case QSerialPort::UnsupportedOperationError:
    case QSerialPort::WriteError:
      {
	discoverDevices();
	break;
      }
    default:
      {
	if(serialPort)
	  {
	    auto error(serialPort->errorString().toLower());

	    if(error.contains(tr("broken pipe")) ||
	       serialPort->isOpen() == false)
	      QTimer::singleShot(250, this, SLOT(slotDisconnect(void)));
	  }

	break;
      }
    }
}
#endif

void glitch_serial_port_window::slotReadyRead(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  auto serialPort = qobject_cast<QSerialPort *> (sender());

  if(serialPort)
    {
      while(serialPort->bytesAvailable() > 0)
	{
	  QByteArray bytes;

	  if(serialPort->canReadLine())
	    bytes = serialPort->readLine();
	  else
	    bytes = serialPort->readAll();

	  if(!bytes.isEmpty())
	    {
	      QString string("");

	      m_packetsReceived += 1;
	      string.append
		(QString("<font color='#33a532'>%1</font>:"
			 "<b>%2</b>:"
			 "<font color='#00238b'>%3</font>: ").
		 arg(serialPort->portName()).
		 arg(QDateTime::currentDateTime().toString(Qt::ISODate)).
		 arg(QString::number(m_packetsReceived)));
	      string.append(bytes);
	      m_ui.communications->append(string);
	    }
	}
    }
#endif
}

void glitch_serial_port_window::slotRefresh(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  discoverDevices();
#endif
}

void glitch_serial_port_window::slotSend(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  auto serialPort = findChild<QSerialPort *> ();

  if(serialPort)
    {
      serialPort->write(m_ui.command->toPlainText().toUtf8());
      serialPort->flush();
    }
#endif
}