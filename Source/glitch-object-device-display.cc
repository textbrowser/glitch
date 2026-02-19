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

#include <QColorDialog>
#include <QDataStream>
#include <QFile>
#include <QJSEngine>

#include "glitch-floating-context-menu.h"
#include "glitch-object-device-display.h"
#include "glitch-undo-command.h"
#include "glitch-variety.h"

glitch_object_device_display::glitch_object_device_display(QWidget *parent):
  glitch_object_device_display(1, parent)
{
}

glitch_object_device_display::glitch_object_device_display
(const qint64 id, QWidget *parent):glitch_object(id, parent)
{
  connect(&m_timer,
	  &QTimer::timeout,
	  this,
	  &glitch_object_device_display::slotReadDevice);
  m_deviceDisplayPropertiesUI = nullptr;
  m_properties[Properties::BORDER_COLOR] = QColor(70, 130, 180, 255);
  m_type = "digitalio-device-display";
  m_value = QVariant();
  resize(100, 30);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setName("digitalio-device-display");
  setStyleSheet("background-color: transparent;");
}

glitch_object_device_display::~glitch_object_device_display()
{
  delete m_deviceDisplayPropertiesDialog;
  delete m_deviceDisplayPropertiesUI;
  m_timer.stop();
}

glitch_object_device_display *glitch_object_device_display::clone
(QWidget *parent) const
{
  auto clone = new glitch_object_device_display(parent);

  clone->m_originalPosition = scene() ? scenePos() : m_originalPosition;
  clone->m_properties = m_properties;
  clone->prepareDevice();
  clone->resize(size());
  clone->setAttribute
    (Qt::WA_OpaquePaintEvent, testAttribute(Qt::WA_OpaquePaintEvent));
  clone->setCanvasSettings(m_canvasSettings);
  clone->setName(clone->name());
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_device_display *glitch_object_device_display::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_device_display
    (values.value("myoid").toLongLong(), parent);

  object->setProperties
    (splitPropertiesRegularExpression(values.value("properties")));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

QMap<QString, QVariant> glitch_object_device_display::
mapFromProperties(void) const
{
  QByteArray bytes
    (m_properties.value(Properties::DEVICE_DISPLAY_PROPERTIES).toByteArray());
  QDataStream stream(&bytes, QIODevice::ReadOnly);
  QMap<QString, QVariant> map;

  stream.setVersion(QDataStream::Qt_5_0);
  stream >> map;

  if(stream.status() != QDataStream::Ok)
    map.clear();

  return map;
}

void glitch_object_device_display::addActions(QMenu &menu)
{
  if(!m_actions.contains(DefaultMenuActions::SET_DEVICE_DISPLAY_DEVICE))
    {
      auto action = new QAction(tr("Set Device &Information..."), this);

      connect(action,
	      &QAction::triggered,
	      this,
	      &glitch_object_device_display::slotSetDeviceInformation,
	      Qt::QueuedConnection);
      m_actions[DefaultMenuActions::SET_DEVICE_DISPLAY_DEVICE] = action;
      menu.addAction(action);
    }
  else if(m_actions.value(DefaultMenuActions::SET_DEVICE_DISPLAY_DEVICE,
			  nullptr))
    menu.addAction
      (m_actions.value(DefaultMenuActions::SET_DEVICE_DISPLAY_DEVICE));

  addDefaultActions(menu);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setChecked(false);
  m_actions.value(DefaultMenuActions::COMPRESS_WIDGET)->setEnabled(false);
  m_actions.value(DefaultMenuActions::GENERATE_SOURCE)->setChecked(false);
  m_actions.value(DefaultMenuActions::GENERATE_SOURCE)->setEnabled(false);
  m_actions.value(DefaultMenuActions::SOURCE_PREVIEW)->setEnabled(false);
}

void glitch_object_device_display::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  QPainterPath path;
  QPen pen;
  auto const frameWidth = static_cast<qreal> (1.5);
  auto const frameWidth1 = frameWidth / 2.0;
  auto const height = static_cast<qreal> (size().height());
  auto const radius = static_cast<qreal> (5.0);
  auto const width = static_cast<qreal> (size().width());
  const QColor color(m_properties.value(Properties::BORDER_COLOR).toString());

  painter.setFont(font());
  path.addRoundedRect
    (QRectF(frameWidth1, frameWidth1, width - frameWidth, height - frameWidth),
     radius,
     radius);
  painter.fillPath
  (path,
   QBrush(QColor(m_properties.value(Properties::BACKGROUND_COLOR).toString())));
  pen.setColor(color);
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setWidthF(frameWidth);
  painter.setPen(pen);
  painter.save();
  painter.drawPath(path);
  painter.restore();
  pen.setColor(QColor(m_properties.value(Properties::FONT_COLOR).toString()));
  painter.setPen(pen);
  painter.save();
  painter.drawText
  (path.boundingRect().adjusted(5.0, 5.0, -5.0, -5.0),
   Qt::AlignCenter | Qt::TextWordWrap,
   m_value.toString());
  painter.restore();
}

void glitch_object_device_display::prepareDevice(void)
{
  auto const map(mapFromProperties());
  auto const url
    (QUrl::fromUserInput(map.value("device_url").toString().trimmed()));

  if(url.isLocalFile() && url.toLocalFile().trimmed().isEmpty() == false)
    {
      m_device ? m_device->deleteLater() : (void) 0;
      m_device = new QFile(url.toLocalFile(), this);
      m_timer.stop();

      if(m_device->open(QIODevice::ReadOnly))
	{
	  m_device->setProperty
	    ("javascript", map.value("javascript").toString());
	  m_device->setProperty
	    ("read_rate_size", map.value("read_rate_size").toLongLong());
	  m_timer.start
	    (qBound(100, map.value("read_rate_interval").toInt(), 10000));
	}
      else
	m_device->deleteLater();
    }
  else
    {
      m_device ? m_device->deleteLater() : (void) 0;
      m_timer.stop();
    }
}

void glitch_object_device_display::save
(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  properties["device_properties"] = m_properties.value
    (Properties::DEVICE_DISPLAY_PROPERTIES).toByteArray().toBase64();
  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_device_display::setDevicePropertiesInformation(void)
{
  if(!m_deviceDisplayPropertiesUI)
    return;

  auto const map(mapFromProperties());

  m_deviceDisplayPropertiesUI->data_type->setCurrentIndex
    (m_deviceDisplayPropertiesUI->data_type->
     findText(map.value("data_type").toString()));
  m_deviceDisplayPropertiesUI->data_type->setCurrentIndex
    (m_deviceDisplayPropertiesUI->data_type->currentIndex() < 0 ?
     0 : m_deviceDisplayPropertiesUI->data_type->currentIndex());
  m_deviceDisplayPropertiesUI->device_url->setText
    (map.value("device_url").toString().trimmed());
  m_deviceDisplayPropertiesUI->javascript->setPlainText
    (map.value("javascript").toString().trimmed());
  m_deviceDisplayPropertiesUI->read_rate_interval->setValue
    (map.value("read_rate_interval").toInt());
  m_deviceDisplayPropertiesUI->read_rate_size->setValue
    (map.value("read_rate_size").toInt());
}

void glitch_object_device_display::setProperties(const QStringList &list)
{
  glitch_object::setProperties(list);
  m_properties[Properties::DEVICE_DISPLAY_PROPERTIES] = QByteArray();

  for(int i = 0; i < list.size(); i++)
    {
      auto string(list.at(i));

      if(string.startsWith("device_properties = "))
	{
	  string = string.mid(string.indexOf('=') + 1);
	  string.remove("\"");
	  m_properties[Properties::DEVICE_DISPLAY_PROPERTIES] =
	    QByteArray::fromBase64(string.trimmed().toUtf8());
	  prepareDevice();
	}
    }
}

void glitch_object_device_display::setProperty
(const Properties property, const QVariant &value)
{
  glitch_object::setProperty(property, value);

  switch(property)
    {
    case Properties::DEVICE_DISPLAY_PROPERTIES:
      {
	prepareDevice();
	setDevicePropertiesInformation();
	break;
      }
    default:
      {
	break;
      }
    }
}

void glitch_object_device_display::simulateAdd(void)
{
  glitch_object::simulateAdd();

  if(m_device)
    {
      if(!m_device->isOpen())
	{
	  if(m_device->open(QIODevice::ReadOnly))
	    m_timer.start();
	  else
	    m_timer.stop();
	}
      else if(!m_timer.isActive())
	m_timer.start();
    }
}

void glitch_object_device_display::simulateDelete(void)
{
  glitch_object::simulateDelete();
  m_device ? m_device->close() : (void) 0;
#ifdef Q_OS_ANDROID
  m_deviceDisplayPropertiesDialog ?
    (void) m_deviceDisplayPropertiesDialog->hide() : (void) 0;
#else
  m_deviceDisplayPropertiesDialog ?
    (void) m_deviceDisplayPropertiesDialog->close() : (void) 0;
#endif
  m_timer.stop();
}

void glitch_object_device_display::slotReadDevice(void)
{
  if(!m_device || !m_device->isOpen())
    return;

  auto const bytes
    (m_device->read(qBound(1LL,
			   m_device->property("read_rate_size").toLongLong(),
			   1048576LL)));

  if(bytes.isEmpty())
    return;

  QJSEngine engine;
  auto const value = engine.evaluate
    (m_device->property("javascript").
     toString().trimmed().replace("%1", bytes));

  if(value.isError() == false && value.toVariant().isValid())
    m_value = value.toVariant();
  else
    m_value = bytes.toHex();

  update();
}

void glitch_object_device_display::slotSetDeviceInformation(void)
{
  if(!m_deviceDisplayPropertiesDialog)
    {
      m_deviceDisplayPropertiesDialog = new QDialog(m_parent);
      m_deviceDisplayPropertiesDialog->setModal(false);
      m_deviceDisplayPropertiesUI = new Ui::glitch_device_display_properties;
      m_deviceDisplayPropertiesUI->setupUi(m_deviceDisplayPropertiesDialog);
      connect
	(m_deviceDisplayPropertiesUI->apply,
	 &QPushButton::clicked,
	 this,
	 &glitch_object_device_display::slotSetDeviceInformationAccepted);
#ifdef Q_OS_ANDROID
      connect(m_deviceDisplayPropertiesUI->close,
	      &QPushButton::clicked,
	      m_deviceDisplayPropertiesDialog,
	      &QDialog::hide);
#else
      connect(m_deviceDisplayPropertiesUI->close,
	      &QPushButton::clicked,
	      m_deviceDisplayPropertiesDialog,
	      &QDialog::close);
#endif
      glitch_variety::sortCombinationBox
	(m_deviceDisplayPropertiesUI->data_type);
     }

#ifdef Q_OS_ANDROID
  m_deviceDisplayPropertiesDialog->showMaximized();
#else
  m_deviceDisplayPropertiesDialog->showNormal();
#endif
  m_deviceDisplayPropertiesDialog->activateWindow();
  m_deviceDisplayPropertiesDialog->raise();
  setDevicePropertiesInformation();
}

void glitch_object_device_display::slotSetDeviceInformationAccepted(void)
{
  if(!m_deviceDisplayPropertiesUI)
    return;

  QMap<QString, QVariant> map;

  map["data_type"] = m_deviceDisplayPropertiesUI->data_type->currentText();
  map["device_url"] = m_deviceDisplayPropertiesUI->device_url->text();
  map["javascript"] = m_deviceDisplayPropertiesUI->javascript->toPlainText();
  map["read_rate_interval"] = m_deviceDisplayPropertiesUI->
    read_rate_interval->value();
  map["read_rate_size"] = m_deviceDisplayPropertiesUI->read_rate_size->
    value();

  QByteArray bytes;
  QDataStream stream(&bytes, QIODevice::WriteOnly);

  stream.setVersion(QDataStream::Qt_5_0);
  stream << map;

  auto const before
    (m_properties.value(Properties::DEVICE_DISPLAY_PROPERTIES).toByteArray());

  if(before != bytes)
    {
      if(m_undoStack)
	{
	  auto undoCommand = new glitch_undo_command
	    (bytes,
	     before,
	     glitch_undo_command::Types::PROPERTY_CHANGED,
	     Properties::DEVICE_DISPLAY_PROPERTIES,
	     this);

	  undoCommand->setText
	    (tr("item property changed (%1, %2)").
	     arg(scenePos().x()).arg(scenePos().y()));
	  m_undoStack->push(undoCommand);
	}
      else
	m_properties[Properties::DEVICE_DISPLAY_PROPERTIES] = bytes;

      emit changed();
    }
}
