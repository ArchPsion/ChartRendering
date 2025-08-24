// Qt Libraries
#include <QApplication>
#include <QString>

// Personal Libraries
#include "QChartInterface.hpp"

int main(int argc, char *argv[])
{
	const QString stylesheet = "QLineEdit { min-width: 20px } QLabel { min-width: 20px }";
	
	auto app = QApplication(argc, argv);
	app.setStyleSheet(stylesheet);
	
	auto p = app.palette();
	p.setColor(QPalette::Window, QColor(255, 255, 255));
	p.setColor(QPalette::WindowText, QColor(51, 51, 51));
	
	p.setColor(QPalette::Base, QColor(255, 255, 255));
	p.setColor(QPalette::AlternateBase, QColor(255, 255, 255));
	
	p.setColor(QPalette::PlaceholderText, QColor(51, 51, 51));
	p.setColor(QPalette::Text, QColor(51, 51, 51));
	
	p.setColor(QPalette::Disabled, QPalette::Button, QColor(170, 170, 170));
	p.setColor(QPalette::Active, QPalette::Button, QColor(204, 204, 204));
	p.setColor(QPalette::Inactive, QPalette::Button, QColor(204, 204, 204));
	p.setColor(QPalette::ButtonText, QColor(51, 51, 51));
	
	p.setColor(QPalette::Highlight, QColor(142, 45, 197));
	p.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
	app.setPalette(p);
	
	auto window = QChartInterface();
	window.show();
	
	return app.exec();
}
