/********************************************************************************
** Form generated from reading UI file 'simpledisplay.ui'
**
** Created: Fri 15. Feb 09:58:30 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMPLEDISPLAY_H
#define UI_SIMPLEDISPLAY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_simpleDisplayClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *simpleDisplayClass)
    {
        if (simpleDisplayClass->objectName().isEmpty())
            simpleDisplayClass->setObjectName(QString::fromUtf8("simpleDisplayClass"));
        simpleDisplayClass->resize(600, 400);
        menuBar = new QMenuBar(simpleDisplayClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        simpleDisplayClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(simpleDisplayClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        simpleDisplayClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(simpleDisplayClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        simpleDisplayClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(simpleDisplayClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        simpleDisplayClass->setStatusBar(statusBar);

        retranslateUi(simpleDisplayClass);

        QMetaObject::connectSlotsByName(simpleDisplayClass);
    } // setupUi

    void retranslateUi(QMainWindow *simpleDisplayClass)
    {
        simpleDisplayClass->setWindowTitle(QApplication::translate("simpleDisplayClass", "simpleDisplay", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class simpleDisplayClass: public Ui_simpleDisplayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMPLEDISPLAY_H
