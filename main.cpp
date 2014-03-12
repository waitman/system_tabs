#include "xhwstat.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xhwstat w;
    w.show();
    
    return a.exec();
}
