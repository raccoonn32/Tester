/********************************************************************************
** Form generated from reading UI file 'selftestdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELFTESTDIALOG_H
#define UI_SELFTESTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelfTestDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelT1;
    QLabel *lb_remainTime;
    QProgressBar *progressBar;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pb_cancel;

    void setupUi(QDialog *SelfTestDialog)
    {
        if (SelfTestDialog->objectName().isEmpty())
            SelfTestDialog->setObjectName(QString::fromUtf8("SelfTestDialog"));
        SelfTestDialog->resize(961, 436);
        verticalLayout = new QVBoxLayout(SelfTestDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelT1 = new QLabel(SelfTestDialog);
        labelT1->setObjectName(QString::fromUtf8("labelT1"));

        horizontalLayout_2->addWidget(labelT1);

        lb_remainTime = new QLabel(SelfTestDialog);
        lb_remainTime->setObjectName(QString::fromUtf8("lb_remainTime"));

        horizontalLayout_2->addWidget(lb_remainTime);


        verticalLayout->addLayout(horizontalLayout_2);

        progressBar = new QProgressBar(SelfTestDialog);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);
        progressBar->setTextVisible(false);

        verticalLayout->addWidget(progressBar);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pb_cancel = new QPushButton(SelfTestDialog);
        pb_cancel->setObjectName(QString::fromUtf8("pb_cancel"));

        horizontalLayout->addWidget(pb_cancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SelfTestDialog);

        QMetaObject::connectSlotsByName(SelfTestDialog);
    } // setupUi

    void retranslateUi(QDialog *SelfTestDialog)
    {
        SelfTestDialog->setWindowTitle(QApplication::translate("SelfTestDialog", "Wait for SelfTest", 0, QApplication::UnicodeUTF8));
        labelT1->setText(QApplication::translate("SelfTestDialog", "Please Wait for Self Test,remaintime :", 0, QApplication::UnicodeUTF8));
        lb_remainTime->setText(QString());
        pb_cancel->setText(QApplication::translate("SelfTestDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SelfTestDialog: public Ui_SelfTestDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELFTESTDIALOG_H
