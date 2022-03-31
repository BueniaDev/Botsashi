#include <QApplication>
#include "qsimsashi.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
	cout << "Usage: " << argv[0] << " [s-record]" << endl;
	return 1;
    }

    QApplication app(argc, argv);
    QSimsashi window;
    window.show();

    if (!window.loadSRecord(argv[1]))
    {
	return 1;
    }

    window.runCore();

    return app.exec();
}