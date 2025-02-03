#include "wohlnet_sendfile_window.h"
#include <QApplication>

#include "sender_app.h"

int main(int argc, char *argv[])
{
    PGE_Application a(argc, argv);
    Wohlnet_Sendfile_Window w;
    w.show();
    QStringList args = a.arguments();
    args.pop_front();//remove application path

    w.uploadFileS(args);
#ifdef __APPLE__
    w.uploadFileS(a.getOpenFileChain());
#endif

#ifdef __APPLE__
    QObject::connect(&a, &PGE_Application::openFileRequested,
                     &w, &Wohlnet_Sendfile_Window::uploadFile);
    a.setConnected();
#endif

    if(args.size()>0)
        w.closeOnFinish();

    return a.exec();
}
