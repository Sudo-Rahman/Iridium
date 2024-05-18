//
// Created by sr-71 on 11/05/2024.
//

#pragma once

#include "Settings.hpp"

using unique_file = std::unique_ptr<QFile>;

class Preview
{
public:
	explicit Preview(const QString &file, const QByteArray &data);

	Preview(unique_file file);

	static bool isPreviewable(const RcloneFile &file);

	static bool isPreviewable(const QList<RcloneFilePtr> &files);

private:
	QByteArray _data;
	QString _name;
	unique_file _file{};

	void macos();

	void linuxPrev();

	void windowsPrev();
};
