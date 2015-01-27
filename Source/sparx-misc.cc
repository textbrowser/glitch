#include <QDir>

#include "sparx-misc.h"

QString sparx_misc::homePath(void)
{
  QByteArray homepath(qgetenv("sparx_HOME"));

  if(homepath.isEmpty())
#ifdef Q_OS_WIN32
    return QDir::currentPath() + QDir::separator() + ".sparx";
#else
    return QDir::homePath() + QDir::separator() + ".sparx";
#endif
  else
    return homepath.constData();
}
