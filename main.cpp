#include "DearDiary.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DearDiary w;
    w.show();

    return a.exec();
}
