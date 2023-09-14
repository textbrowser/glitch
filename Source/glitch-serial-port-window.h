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

#ifndef _glitch_serial_port_window_h_
#define _glitch_serial_port_window_h_

#ifdef GLITCH_SERIAL_PORT_SUPPORTED
#include <QSerialPort>
#endif

#include "ui_glitch-serial-port-window.h"

class glitch_serial_port_window: public QDialog
{
  Q_OBJECT

 public:
  glitch_serial_port_window(QWidget *parent);
  ~glitch_serial_port_window();

 private:
  Ui_glitch_serial_port_window m_ui;
  quint64 m_packetsReceived;
  void discoverDevices(void);
  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);

 private slots:
  void slotClear(void);
  void slotConnect(void);
  void slotDisconnect(void);
#ifdef GLITCH_SERIAL_PORT_SUPPORTED
  void slotErrorOccurred(QSerialPort::SerialPortError error);
#endif
  void slotReadyRead(void);
  void slotRefresh(void);
  void slotSend(void);
};

#endif
