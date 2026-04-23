#include <QskWindow.h>

int main( int argc, char* argv[] )
{
    QGuiApplication app( argc, argv );
    QskWindow window;
    window.show();
    return app.exec();
}
