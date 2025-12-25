/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include "glitch-scroll-filter.h"
#include "glitch-serial-port-window.h"
#include "glitch-variety.h"

glitch_serial_port_window::glitch_serial_port_window(QWidget *parent):
  QDialog(parent)
{
  m_packetsReceived = 0;
  m_ui.setupUi(this);
  glitch_variety::sortCombinationBox(m_ui.flow_control);
  glitch_variety::sortCombinationBox(m_ui.parity);
  glitch_variety::sortCombinationBox(m_ui.stop_bits);
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

  foreach(auto widget, findChildren<QWidget *> ())
    if(qobject_cast<QComboBox *> (widget))
      widget->installEventFilter(new glitch_scroll_filter(this));

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

#ifndef Q_OS_ANDROID
  settings.setValue("serial_port_window/geometry", saveGeometry());
#endif
  settings.setValue
    ("serial_port_window/splitter_1", m_ui.splitter_1->saveState());
  settings.setValue
    ("serial_port_window/splitter_2", m_ui.splitter_2->saveState());
  QDialog::closeEvent(event);
}

void glitch_serial_port_window::discoverDevices(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.port_name->clear();

  foreach(auto const &port, QSerialPortInfo::availablePorts())
    m_ui.port_name->addItem(port.portName());

  glitch_variety::sortCombinationBox(m_ui.port_name);

  auto serialPort = findChild<QSerialPort *> ();

  if(serialPort)
    {
      int index = -1;

      if((index = m_ui.port_name->findText(serialPort->portName())) == -1 ||
	 (serialPort->isOpen() == false))
	QTimer::singleShot(250, this, SLOT(slotDisconnect(void)));
      else if(index >= 0)
	m_ui.port_name->setCurrentIndex(index);
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
#ifndef Q_OS_ANDROID
  restoreGeometry(settings.value("serial_port_window/geometry").toByteArray());
#endif
}

void glitch_serial_port_window::slotClear(void)
{
  m_ui.communications->setRowCount(0);
}

void glitch_serial_port_window::slotConnect(void)
{
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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
      m_ui.connect->animateNegatively(2500);
      m_ui.last_error->setText
	(QString("%1:%2:%3").
	 arg(serialPort->portName().trimmed()).
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 0))
	 arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs)).
#else
	 arg(QDateTime::currentDateTime().toString(Qt::ISODate)).
#endif
	 arg(serialPort->errorString().trimmed().toUpper()));
      m_ui.last_error->setCursorPosition(0);
      return;
    }
  else
    m_ui.last_error->clear();

  connect(serialPort,
	  &QSerialPort::readyRead,
	  this,
	  &glitch_serial_port_window::slotReadyRead);
  connect(serialPort,
	  SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
	  this,
	  SLOT(slotErrorOccurred(QSerialPort::SerialPortError)));

  auto ok = true;

  ok &= serialPort->setBaudRate
    (static_cast<qint32> (m_ui.baud_rate_input->currentText().toInt()),
     QSerialPort::Input);
  ok &= serialPort->setBaudRate
    (static_cast<qint32> (m_ui.baud_rate_output->currentText().toInt()),
     QSerialPort::Output);
  ok &= serialPort->setBreakEnabled(m_ui.break_enabled->isChecked());
  ok &= serialPort->setDataBits
    (QSerialPort::DataBits(m_ui.data_bits->currentIndex() + 5));
  ok &= serialPort->setDataTerminalReady(m_ui.data_terminal_ready->isChecked());

  if(m_ui.flow_control->currentText() == tr("Hardware"))
    ok &= serialPort->setFlowControl(QSerialPort::HardwareControl);
  else if(m_ui.flow_control->currentText() == tr("Software"))
    ok &= serialPort->setFlowControl(QSerialPort::SoftwareControl);
  else
    ok &= serialPort->setFlowControl(QSerialPort::NoFlowControl);

  if(m_ui.parity->currentText() == tr("Even"))
    ok &= serialPort->setParity(QSerialPort::EvenParity);
  else if(m_ui.parity->currentText() == tr("Mark"))
    ok &= serialPort->setParity(QSerialPort::MarkParity);
  else if(m_ui.parity->currentText() == tr("Odd"))
    ok &= serialPort->setParity(QSerialPort::OddParity);
  else if(m_ui.parity->currentText() == tr("Space"))
    ok &= serialPort->setParity(QSerialPort::SpaceParity);
  else
    ok &= serialPort->setParity(QSerialPort::NoParity);

  ok &= serialPort->setRequestToSend(m_ui.rts->isChecked());

  if(m_ui.stop_bits->currentText() == tr("OneAndHalf"))
    ok &= serialPort->setStopBits(QSerialPort::OneAndHalfStop);
  else if(m_ui.stop_bits->currentText() == tr("Two"))
    ok &= serialPort->setStopBits(QSerialPort::TwoStop);
  else
    ok &= serialPort->setStopBits(QSerialPort::OneStop);

  ok ? m_ui.connect->animate(2500) : m_ui.connect->animateNegatively(2500);
  ok ? m_ui.last_error->clear() :
    m_ui.last_error->setText
    (QString("%1:%2:%3").
     arg(serialPort->portName().trimmed()).
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 0))
     arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs)).
#else
     arg(QDateTime::currentDateTime().toString(Qt::ISODate)).
#endif
     arg(serialPort->errorString().trimmed().toUpper())),
    m_ui.last_error->setCursorPosition(0);
  m_ui.connect->setEnabled(false);
  m_ui.disconnect->setEnabled(true);
  m_ui.send->setEnabled(true);
  m_ui.serial_port->setText(serialPort->portName());
  serialPort->clear();
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

  if(error != QSerialPort::NoError && serialPort)
    {
      m_ui.last_error->setText
	(QString("%1:%2:%3 (%4)").
	 arg(serialPort->portName().trimmed()).
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 0))
	 arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs)).
#else
	 arg(QDateTime::currentDateTime().toString(Qt::ISODate)).
#endif
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
	    auto const error(serialPort->errorString().toLower());

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
      QByteArray bytes;

      while(serialPort->bytesAvailable() > 0)
	if(serialPort->canReadLine())
	  bytes.append(serialPort->readLine());
	else
	  bytes.append(serialPort->readAll());

      m_ui.communications->setSortingEnabled(false);

      foreach(auto const &bytes, bytes.split('\n'))
	if(!bytes.trimmed().isEmpty())
	  {
	    m_ui.communications->setRowCount
	      (m_ui.communications->rowCount() + 1);

	    QStringList list;

	    list << QString(qUtf8Printable(bytes)).trimmed()
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 0))
		 << QDateTime::currentDateTime().toString(Qt::ISODateWithMs)
#else
		 << QDateTime::currentDateTime().toString(Qt::ISODate)
#endif
		 << serialPort->portName();

	    for(int i = 0; i < list.size(); i++)
	      {
		auto item = new QTableWidgetItem
		  (list.at(i).trimmed().isEmpty() ?
		   tr("(Empty)") : list.at(i).trimmed());

		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		m_ui.communications->setItem
		  (m_ui.communications->rowCount() - 1, i, item);
	      }
	  }

      m_ui.communications->scrollToBottom();
      m_ui.communications->setSortingEnabled(true);
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
      serialPort->write(m_ui.command->toPlainText().toUtf8()) != -1 ?
	m_ui.send->animate(2500) : m_ui.send->animateNegatively(2500);
      serialPort->flush();
    }
#endif
}
