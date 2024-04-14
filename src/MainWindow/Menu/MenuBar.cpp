//
// Created by Rahman on 14/04/2023.
//

#include "MenuBar.hpp"
#include "SettingsDialog.hpp"
#include <QMessageBox>
#include <QFormLayout>
#include <QLabel>
#include <Config.h>
#include <Settings.hpp>
#include <boost/version.hpp>
#include <iridium/process.hpp>
#include <iridium/parsers/version_parser.hpp>
#include <iridium/entities/version.hpp>
#include <IridiumApp.hpp>

MenuBar::MenuBar(QWidget * parent) : QMenuBar(parent)
{
	_about_menu = addMenu(tr("À propos"));
	_about_action = _about_menu->addAction("About");

	_settings_menu = addMenu(tr("Paramètres"));
	_settings_action = _settings_menu->addAction("Settings");

	connectSignals();
}

void MenuBar::connectSignals()
{
	connect(_about_action, &QAction::triggered, this, [this]()
	{
		auto dialog = QDialog(this);
		dialog.setWindowTitle(tr("À propos d'Iridium"));
		dialog.setModal(true);

		auto formLayout = QFormLayout(&dialog);
		formLayout.setContentsMargins(30, 30, 30, 30);
		formLayout.setSizeConstraint(QLayout::SetFixedSize);
		formLayout.setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		formLayout.setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);

		// add icon
		auto icon = QLabel();
		icon.setPixmap(
			QPixmap(":/ressources/Iridium.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		icon.setAlignment(Qt::AlignCenter);
		icon.setContentsMargins(0, 0, 0, 20);
		formLayout.addRow(&icon);

		auto version = QLabel(
			"<a href=\"https://github.com/Sudo-Rahman/Iridium/releases/tag/v" +
			QString::fromStdString(IRIDIUM_VERSION) +
			"\">" + QString::fromStdString(IRIDIUM_VERSION) + "</a>");
		version.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Version d'Iridium : ")), &version);

		auto cppVersion = QLabel("<a href=\"https://isocpp.org/\">" + QString::fromStdString(CPP_VERSION) + "</a>");
		cppVersion.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("C++ Version : ")), &cppVersion);

		auto qtVersion = QLabel("<a href=\"https://www.qt.io/\">" + QString::fromStdString(QT_VERSION_STR) + "</a>");
		qtVersion.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Qt Version : ")), &qtVersion);

		auto boostVersion = QLabel(
			"<a href=\"https://www.boostVersion.org/\">" + QString::fromStdString(BOOST_LIB_VERSION) + "</a>");
		boostVersion.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Boost Version : ")), &boostVersion);

		QLabel rcloneVersion;
		iridium::rclone::process()
				.version()
				.on_finish_parser(iridium::rclone::parser::version_parser::create(
					new iridium::rclone::parser::version_parser(
						[&](const ire::version& version)
						{
							IridiumApp::runOnMainThread([&rcloneVersion,version = std::move(version)]
							{
								rcloneVersion.setText(
									"<a href=\"https://rclone.org/\">" + QString::fromStdString(version.rclone_version)
									+ "</a>");
							});
						})))
				.execute()
				.wait_for_finish();
		rcloneVersion.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Rclone Version : ")), &rcloneVersion);


		auto auteur = QLabel("<a href=\"https://github.com/Sudo-Rahman\">" + QString::fromStdString(AUTHOR) + "</a>");
		auteur.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Auteur : ")), &auteur);

		auto licence = QLabel("<a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GPL V3</a>");
		licence.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Licence : ")), &licence);

		auto text = []
		{
			QString text;
			for (auto const& ressource: OTHER_RESSOURCES_AUHTOR)
				text += "<a href=\"" + QString::fromStdString(ressource) + "\">" + QString::fromStdString(ressource) +
						"</a><br>";
			return text;
		}();
		auto otherRessources = QLabel(text);
		otherRessources.setWordWrap(true);
		otherRessources.setOpenExternalLinks(true);
		formLayout.addRow(new QLabel(tr("Autres ressources : ")), &otherRessources);
		dialog.exec();
	});

	connect(_settings_action, &QAction::triggered, this, [this]()
	{
		auto dialog = SettingsDialog(this);
		dialog.exec();
	});
}
