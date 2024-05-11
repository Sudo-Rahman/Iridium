//
// Created by sr-71 on 11/05/2024.
//

#include "Preview.hpp"

#include <QMessageBox>
#include <Settings.hpp>

Preview::Preview(const QString &file, const QByteArray &data) : _data(data), _name(file)
{
	switch (Settings::getSystem().os)
	{
		case Settings::MacOs:
			macos();
			break;
		default:
			QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Système d'exploitation non supporté"));
			break;
	}
}

Preview::Preview(unique_file file) : _file(std::move(file))
{
	switch (Settings::getSystem().os)
	{
		case Settings::MacOs:
			macos();
			break;
		default:
			QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Système d'exploitation non supporté"));
			break;
	}
}

void Preview::macos()
{
	if(_file == nullptr)
	{
		auto file = new QFile(Settings::saveFileToTemp(_data, _name));
		_file = unique_file(file);
	}
	try
	{
		boost::process::child proc(boost::process::shell ,"open",
		                           boost::process::args = {QFileInfo(_file->fileName()).absoluteFilePath().toStdString()});
		proc.wait();
		if (proc.exit_code() not_eq 0)
			QMessageBox::critical(nullptr, QObject::tr("Erreur"),
			                      QObject::tr("Impossible d'ouvrir le fichier : ") + _name);
	}
	catch (std::exception &e) { QMessageBox::critical(nullptr, QObject::tr("Erreur"), e.what()); }
}
