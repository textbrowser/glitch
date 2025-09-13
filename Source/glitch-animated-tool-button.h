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

#ifndef _glitch_animated_tool_button_h_
#define _glitch_animated_tool_button_h_

#include <QPropertyAnimation>
#include <QToolButton>

class glitch_animated_tool_button: public QToolButton
{
  Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor)

 public:
  glitch_animated_tool_button(QWidget *parent):QToolButton(parent)
  {
    m_animator.setPropertyName("color");
    m_animator.setTargetObject(this);
    m_negativeColor = QColor(255, 36, 0); // A sprite of red.
    m_originalColor = palette().color(QPalette::Window);
    m_positiveColor = QColor(1, 191, 255); // A sprite of blue.
  }

  ~glitch_animated_tool_button()
  {
    m_animator.stop();
  }

  QColor color(void) const
  {
    return QColor(Qt::blue); // A guess.
  }

  void animate(const int milliseconds)
  {
    animatePositively(milliseconds);
  }

  void animateNegatively(const int milliseconds)
  {
    animation(m_negativeColor, milliseconds);
  }

  void animatePositively(const int milliseconds)
  {
    animation(m_positiveColor, milliseconds);
  }

 private:
  QColor m_negativeColor;
  QColor m_originalColor;
  QColor m_positiveColor;
  QPropertyAnimation m_animator;

  void animation(const QColor &color, const int milliseconds)
  {
    if(milliseconds <= 0)
      {
	m_animator.stop();
	return;
      }

    m_animator.stop();
    m_animator.setDuration(milliseconds);
    m_animator.setEasingCurve(QEasingCurve::OutCubic);
    m_animator.setEndValue(m_originalColor);
    m_animator.setLoopCount(1);
    m_animator.setStartValue(color);
    m_animator.start();
  }

  void setColor(const QColor &color)
  {
    setStyleSheet
      (QString("background-color: rgb(%1, %2, %3); "
	       "border: none; "
	       "padding: 5px;").
       arg(color.red()).arg(color.green()).arg(color.blue()));
  }
};

#endif
