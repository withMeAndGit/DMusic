#include "yapi.hpp"
#include<QDebug>

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

Yapi::Yapi(QObject *parent) : QObject(parent)
{
	Py_Initialize();
//	PyRun_SimpleString("from yandex_music import *\n"
//										 "me = Client('AgAAAAAwR49zAAG8XvNMwDoyKUj6lw3xFFjPS_Y')\n");
}

Yapi::~Yapi()
{
	Py_FinalizeEx();
}

void Yapi::download(QString id)
{
  py::object str = "Привет, мир!";
  qStdOut() << str.to<QString>() << '\n';
//	QString r("track = me.tracks(["); r += id; r += "])[0]\n"
//			 "track.download('a.mp3')\n";
//	PyRun_SimpleString(r.toUtf8().data());
}
