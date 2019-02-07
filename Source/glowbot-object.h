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

#ifndef _glowbot_object_h_
#define _glowbot_object_h_

#include <QMenu>
#include <QSqlDatabase>
#include <QWidget>

class glowbot_object: public QWidget
{
  Q_OBJECT

 public:
  glowbot_object(QWidget *parent);
  glowbot_object(const quint64 id, QWidget *parent);
  virtual glowbot_object *clone(QWidget *parent) const = 0;
  QString type(void) const;
  quint64 id(void) const;
  static glowbot_object *createFromValues
    (const QMap<QString, QVariant> &values, QString &error, QWidget *parent);
  virtual ~glowbot_object();
  virtual bool hasView(void) const = 0;
  virtual bool isMandatory(void) const = 0;
  virtual void addActions(QMenu &menu) const = 0;
  virtual void save(const QSqlDatabase &db, QString &error);
  void move(const QPoint &point);
  void move(int x, int y);

 private:
  static quint64 s_id;

 protected:
  QString m_type;
  QWidget *m_parent;
  bool m_initialized;
  quint64 m_id;
  void addDefaultActions(QMenu &menu) const;

 protected slots:
  void slotSetStyleSheet(void);

 signals:
  void changed(void);
};

#endif
