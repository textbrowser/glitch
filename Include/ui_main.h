/********************************************************************************
** Form generated from reading UI file 'main.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_main
{
public:
    QAction *action_New_Diagram;
    QAction *action_Open_Diagram;
    QAction *action_Quit;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QListWidget *widgets;
    QGraphicsView *arena;
    QMenuBar *menubar;
    QMenu *menu_File;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *main)
    {
        if (main->objectName().isEmpty())
            main->setObjectName(QString::fromUtf8("main"));
        main->resize(800, 600);
        action_New_Diagram = new QAction(main);
        action_New_Diagram->setObjectName(QString::fromUtf8("action_New_Diagram"));
        action_Open_Diagram = new QAction(main);
        action_Open_Diagram->setObjectName(QString::fromUtf8("action_Open_Diagram"));
        action_Quit = new QAction(main);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        centralwidget = new QWidget(main);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        splitter->setChildrenCollapsible(false);
        widgets = new QListWidget(splitter);
        widgets->setObjectName(QString::fromUtf8("widgets"));
        splitter->addWidget(widgets);
        arena = new QGraphicsView(splitter);
        arena->setObjectName(QString::fromUtf8("arena"));
        splitter->addWidget(arena);

        verticalLayout->addWidget(splitter);

        main->setCentralWidget(centralwidget);
        menubar = new QMenuBar(main);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 27));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        main->setMenuBar(menubar);
        statusbar = new QStatusBar(main);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        main->setStatusBar(statusbar);
        toolBar = new QToolBar(main);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        main->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menu_File->menuAction());
        menu_File->addAction(action_New_Diagram);
        menu_File->addAction(action_Open_Diagram);
        menu_File->addSeparator();
        menu_File->addAction(action_Quit);

        retranslateUi(main);

        QMetaObject::connectSlotsByName(main);
    } // setupUi

    void retranslateUi(QMainWindow *main)
    {
        main->setWindowTitle(QApplication::translate("main", "SparX", 0, QApplication::UnicodeUTF8));
        action_New_Diagram->setText(QApplication::translate("main", "&New Diagram", 0, QApplication::UnicodeUTF8));
        action_Open_Diagram->setText(QApplication::translate("main", "&Open Diagram", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("main", "&Quit", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("main", "&File", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("main", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class main: public Ui_main {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_H
