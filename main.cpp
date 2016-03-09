#include "wohlnet_sendfile_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Wohlnet_Sendfile_Window w;
    w.show();
    QStringList args=a.arguments();
    args.pop_front();//remove application path
    w.uploadFileS(args);
    return a.exec();
}
