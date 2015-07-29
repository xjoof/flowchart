
#include <QtWidgets/QApplication>

#include "photonmainwin.h"


#ifdef _DEBUG
#define CONSOLEMODE 0
#endif
int main(int argc, char *argv[])
{

#if CONSOLEMODE == 1

#else


    int mode = 0;

    if(argc > 1)
    {
        std::string argstr(argv[1]);
        if(argstr.compare("engineer") == 0)
            mode = 1;
    }

    QApplication a(argc, argv);




    PhotonMainWin w(nullptr,mode);
    w.show();

    return a.exec();
#endif
}
