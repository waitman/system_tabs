/* *****************************************************************************
 *
 *system tabs

Copyright (c) 2014, Waitman C. Gobble, II <ns@waitman.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions
   and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
   and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   **
   ***
***************************************************************************** */
#include "xhwstat.h"
#include "ui_xhwstat.h"
#include <QtWebKit/QWebView>
#include <QProcess>
#include <QTimer>
#include <QTabWidget>
#include <QTabBar>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QDateTime>

/* timer times in milliseconds */
#define HWSTAT_TIMER    1000
#define SOCKSTAT_TIMER  1000
#define TOP_TIMER       1000
#define MSGS_TIMER      1000
#define EML_TIMER       1000
#define MEM_TIMER       1000
#define TIME_TIMER      1000

/* use quotes in defines below */
/* show stats for IPv4 (4) or IPv6 (6) */
#define SOCKSTAT_PACKETS    "4"
//#define SOCKSTAT_PACKETS  "6"

/* # of records to read in /var/log/maillog */
#define EML_RECORDS     "16"

/* # of records to read in /var/log/message */
#define MSGS_RECORDS    "20"

/* arguments to commands */
#define TOP_ARGS        "CHn"
#define VMSTAT_ARGS     "P"
#define DF_ARGS         "h"

/* press the mouse button to pause display refresh, release to continue */
bool toggle_display;

//TODO: figure out scrolling issue

xhwstat::xhwstat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xhwstat)
{
    /* running is yes */
    toggle_display=false;
    ui->setupUi(this);

    /* header and footer for html output */
    hd = "<HTML><BODY STYLE='background-color:#000;color:#00ff00;'><style type='text/css'>pre { font-size:86%; white-space:pre-wrap; }</style><div id='dis'><pre>";
    ft = "</pre></div></BODY></HTML>";

    /* init time */

    QTimer *timer0 = new QTimer(this);
    connect(timer0, SIGNAL(timeout()), this, SLOT(up_time()));
    timer0->start(TIME_TIMER);

    /* create the tab container widget */
    tabWidget = new QTabWidget();
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* create the tabs */
    tab1 = new QWidget;
    tab2 = new QWidget;
    tab3 = new QWidget;
    tab4 = new QWidget;
    tab5 = new QWidget;
    tab6 = new QWidget;

    /* add the tabs to the container, label with tab name */
    tabWidget->addTab(tab1,tr("hwstat"));
    tabWidget->addTab(tab2,tr("sockstat"));
    tabWidget->addTab(tab3,tr("top"));
    tabWidget->addTab(tab4,tr("msgs"));
    tabWidget->addTab(tab5,tr("eml"));
    tabWidget->addTab(tab6,tr("memory"));

    /* set up tab1, hwstat output from ports/sysutils/hwstat */
    view1 = new XQWebView(tab1);
    ml1 = new QVBoxLayout();
    ml1->addWidget(view1);
    tab1->setLayout(ml1);
    view1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view1->setHtml(hd + "hwstat" + ft);
    view1->show();
    QTimer *timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(up_hwstat()));
    timer1->start(HWSTAT_TIMER);

    /* set up tab2, sockstat -4 */
    view2 = new XQWebView(tab2);
    ml2 = new QVBoxLayout();
    ml2->addWidget(view2);
    tab2->setLayout(ml2);
    view2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view2->setHtml(hd + "sockstat" + ft);
    view2->show();
    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(up_sockstat()));
    timer2->start(SOCKSTAT_TIMER);

    /* set up tab3, output from top command */
    view3 = new XQWebView(tab3);
    ml3 = new QVBoxLayout();
    ml3->addWidget(view3);
    tab3->setLayout(ml3);
    view3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view3->setHtml(hd + "top" + ft);
    view3->show();
    QTimer *timer3 = new QTimer(this);
    connect(timer3, SIGNAL(timeout()), this, SLOT(up_top()));
    timer3->start(TOP_TIMER);

    /* set up tab4, last 20 records in /var/log/messages */
    view4 = new XQWebView(tab4);
    ml4 = new QVBoxLayout();
    ml4->addWidget(view4);
    tab4->setLayout(ml4);
    view4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view4->setHtml(hd + "messages" + ft);
    view4->show();
    QTimer *timer4 = new QTimer(this);
    connect(timer4, SIGNAL(timeout()), this, SLOT(up_msgs()));
    timer4->start(MSGS_TIMER);

    /* set up tab5, last 12 records in /var/log/maillog */
    view5 = new XQWebView(tab5);
    ml5 = new QVBoxLayout();
    ml5->addWidget(view5);
    tab5->setLayout(ml5);
    view5->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view5->setHtml(hd + "maillog" + ft);
    view5->show();
    QTimer *timer5 = new QTimer(this);
    connect(timer5, SIGNAL(timeout()), this, SLOT(up_eml()));
    timer5->start(EML_TIMER);

    /* set up tab6, memory and drive space */
    view6 = new XQWebView(tab6);
    ml6 = new QVBoxLayout();
    ml6->addWidget(view6);
    tab6->setLayout(ml6);
    view6->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view6->setHtml(hd + "memory" + ft);
    view6->show();
    QTimer *timer6 = new QTimer(this);
    connect(timer6, SIGNAL(timeout()), this, SLOT(up_mem()));
    timer6->start(MEM_TIMER);

    /* display everything */
    ml = new QVBoxLayout();
    tabWidget->setLayout(ml);
    setCentralWidget(tabWidget);

}

xhwstat::~xhwstat()
{
    delete ui;
}

void xhwstat::up_time()
{
    /* update the time string */
    QDateTime dateTime = QDateTime::currentDateTime();
    tm = dateTime.toString()+ "\n\n";
}

void xhwstat::up_hwstat()
{
    /* are we paused? */
    if (!toggle_display)
    {
        /* set up command and create QProcess */
        QStringList arguments;
        QString foo = "/usr/local/bin/hwstat";
        arguments << "-c" << foo;
        QProcess* process = new QProcess(this);
        process->start("/bin/sh", arguments);
        process->waitForFinished();
        /* collect returned output and stick in html layout, feed to webkit widget */
        QString tmp = process->readAll();
        QString out = hd + tm + tmp + ft;
        view1->setHtml(out);
    }
}

void xhwstat::up_sockstat()
{
    /* are we paused? */
    if (!toggle_display)
    {
        /* set up command and create QProcess */
        QStringList arguments;
        QString foo = "/usr/bin/sockstat -" + QString(SOCKSTAT_PACKETS);
        arguments << "-c" << foo;
        QProcess* process = new QProcess(this);
        process->start("/bin/sh", arguments);
        process->waitForFinished();
        /* collect returned output and stick in html layout, feed to webkit widget */
        QString tmp = process->readAll();
        QString out = hd + tm + tmp + ft;
        view2->setHtml(out);
    }
}

void xhwstat::up_top()
{
    /* are we paused? */
    if (!toggle_display)
    {
        /* set up command and create QProcess */
        QStringList arguments;
        QString foo = "/usr/bin/top -" + QString(TOP_ARGS);
        arguments << "-c" << foo;
        QProcess* process = new QProcess(this);
        process->start("/bin/sh", arguments);
        process->waitForFinished();
        /* collect returned output and stick in html layout, feed to webkit widget */
        QString tmp = process->readAll();
        QString out = hd + tm + tmp + ft;
        view3->setHtml(out);
    }
}

void xhwstat::up_msgs()
{
    /* are we paused? */
    if (!toggle_display)
    {
        /* set up command and create QProcess */
        QStringList arguments;
        QString foo = "tail -" + QString(MSGS_RECORDS) + " /var/log/messages";
        arguments << "-c" << foo;
        QProcess* process = new QProcess(this);
        process->start("/bin/sh", arguments);
        process->waitForFinished();
        /* collect returned output and stick in html layout, feed to webkit widget */
        QString tmp = process->readAll();
        QString out = hd + tm + tmp + ft;
        view4->setHtml(out);
    }
}

void xhwstat::up_eml()
{
    /* are we paused? */
    if (!toggle_display)
    {
        /* set up command and create QProcess */
        QStringList arguments;
        QString foo = "tail -" + QString(EML_RECORDS) + " /var/log/maillog";
        arguments << "-c" << foo;
        QProcess* process = new QProcess(this);
        process->start("/bin/sh", arguments);
        process->waitForFinished();
        /* collect returned output and stick in html layout, feed to webkit widget */
        QString tmp = process->readAll();
        QString out = hd + tm + tmp + ft;
        view5->setHtml(out);
    }
}

void xhwstat::up_mem()
{
    /* are we paused? */
    if (!toggle_display)
    {
        /* set up command and create QProcess */
        QStringList arguments;
        /* do vmstat and df with same call to process->start() */
        QString foo = "/usr/bin/vmstat -" + QString(VMSTAT_ARGS) + " ; echo ; df -" + QString(DF_ARGS) + "";
        arguments << "-c" << foo;
        QProcess* process = new QProcess(this);
        process->start("/bin/sh", arguments);
        process->waitForFinished();
        /* collect returned output and stick in html layout, feed to webkit widget */
        QString tmp = process->readAll();
        QString out = hd + tm + tmp + ft;
        view6->setHtml(out);
    }
}

XQWebView::XQWebView(QWidget *parent):
QWebView(parent)
{
    //nothing special, hello
}

void XQWebView::mousePressEvent(QMouseEvent *event)
{
    /* press mouse button to pause refresh */
    toggle_display = true;
    event->accept();
}

//TODO: allow user to copy/paste

void XQWebView::mouseReleaseEvent(QMouseEvent *event)
{
    /* release mouse button to re-enable refresh */
    toggle_display = false;
    event->accept();
}

XQWebView::~XQWebView()
{
    //nothing happening, bye
}

//EOF
