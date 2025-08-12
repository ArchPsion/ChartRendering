// Qt Libraries
#include <QApplication>
#include <QString>

// Personal Libraries
#include "QChartInterface.hpp"

int main(int argc, char *argv[])
{
	auto app = QApplication(argc, argv);
	const QString stylesheet = "QLineEdit { min-width: 20px } QLabel { min-width: 20px }";
	app.setStyleSheet(stylesheet);
	
	auto window = QChartInterface();
	window.show();
	return app.exec();
}
