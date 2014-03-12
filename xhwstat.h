#ifndef XHWSTAT_H
#define XHWSTAT_H

#include <QMainWindow>
#include <QtWebKit/QWebView>
#include <QTabWidget>
#include <QVBoxLayout>

namespace Ui {
class xhwstat;
class XQWebView;
}

/* implement our own mousepress and release handlers to pause refresh */
class XQWebView : public QWebView
{
    Q_OBJECT

    public:
    explicit XQWebView(QWidget *parent);
    ~XQWebView();

    protected:
    /* our handlers */
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
};


class xhwstat : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit xhwstat(QWidget *parent = 0);
    ~xhwstat();
    
private slots:
    /* these functions collect the data and display in the webkit view */
    void up_hwstat();
    void up_sockstat();
    void up_top();
    void up_msgs();
    void up_eml();
    void up_mem();
    /* update time string */
    void up_time();

private:
    Ui::xhwstat *ui;
    /* main layout and layouts for each tab */
    QVBoxLayout *ml;
    QVBoxLayout *ml1;
    QVBoxLayout *ml2;
    QVBoxLayout *ml3;
    QVBoxLayout *ml4;
    QVBoxLayout *ml5;
    QVBoxLayout *ml6;
    /* webkit views */
    XQWebView *view1;
    XQWebView *view2;
    XQWebView *view3;
    XQWebView *view4;
    XQWebView *view5;
    XQWebView *view6;
    /* tab containers */
    QTabWidget *tabWidget;
    /* tabs */
    QWidget *tab1;
    QWidget *tab2;
    QWidget *tab3;
    QWidget *tab4;
    QWidget *tab5;
    QWidget *tab6;
    /* html header and footer */
    QString hd;
    QString ft;
    /* keep the time */
    QString tm;
};

#endif // XHWSTAT_H

//EOF
