#ifndef __Q_CHART_INTERFACE_HPP__
#define __Q_CHART_INTERFACE_HPP__

// Qt Libraries
#include <QCheckBox>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBrowser>

// Standard Libraries
#include <iostream>

// Personal Libraries
#include "HexDayAnalysis.hpp"
#include "QCustomGraphicsScene.hpp"

class QChartInterface : public QMainWindow
{
	Q_OBJECT
	
	private:
		
		inline static QRectF			NonFlatRectangle(const QRectF&);
		
		QGraphicsView* const			candlestickView = new QGraphicsView(this);
		QCustomGraphicsScene* const		candlestickScene = new QCustomGraphicsScene(this, sceneItemInfo);
		QRectF					candlestickRect = QRectF();
		
		QLabel* const				fileLabel = new QLabel("No file loaded.", this);
		
		QLineEdit* const			cursorEdit = new QLineEdit(this);
		QLineEdit* const			timestampEdit = new QLineEdit(this);
		QLineEdit* const			highEdit = new QLineEdit(this);
		QLineEdit* const			lowEdit = new QLineEdit(this);
		
		QLineEdit* const			timeSpotEdit = new QLineEdit(this);
		QLineEdit* const			timeUnitEdit = new QLineEdit(this);
		QLineEdit* const			chartSizeEdit = new QLineEdit(this);
		QLineEdit* const			takeProfitEdit = new QLineEdit(this);
		QLineEdit* const			stopLossEdit = new QLineEdit(this);
		
		QCheckBox* const			eIBox = new QCheckBox("Elemental Increment", this);
		QCheckBox* const			level005Box = new QCheckBox("5", this);
		QCheckBox* const			level010Box = new QCheckBox("10", this);
		QCheckBox* const			level025Box = new QCheckBox("25", this);
		QCheckBox* const			level050Box = new QCheckBox("50", this);
		QCheckBox* const			level100Box = new QCheckBox("100", this);
		QCheckBox* const			level250Box = new QCheckBox("250", this);
		QCheckBox* const			level500Box = new QCheckBox("500", this);
		
		QTextBrowser* const			informationPanel = new QTextBrowser(this);
		const QString				logHeader = "<html><head><style>p.small { line-height: 0.4; }</style></head><body>";
		const QString				logFooter = "</body></html>";
		QString					logBody;
		
		HexDayAnalysis				savedInformation;
		std::vector<HexStrip>			sceneItemInfo;
		std::vector<HexLevelPack>		levelPacks;
		
		inline HexCheckFile			check(void);
		inline void				drawBlackLines(void);
		inline void				drawCandlesticks(quint32, quint32, quint32, qreal, qreal);
		inline void				drawTimeLines(void);
		inline void				updateCandlesticks(std::vector<HexStrip>&, quint32);
		inline void				updateInformationPanel(void);
	
	private slots:
	
		inline void				loadHistory(void);
		inline void				reset(void);
		inline void				showCandlesticks(void);
		inline void				showNewCandlesticks(const QUrl&);
		inline void				study(void);
		inline void				updateBlackLines(void);
	
	protected:
	
		inline void				keyReleaseEvent(QKeyEvent*) override;
	
	public:
	
		inline QChartInterface(void);
};

QChartInterface::QChartInterface(void) : QMainWindow()
{
	QMainWindow::setWindowTitle("Custom Chart Study");
	QChartInterface::candlestickScene->setEdits(QChartInterface::highEdit, QChartInterface::lowEdit, QChartInterface::timestampEdit, QChartInterface::cursorEdit);

	const auto layout = new QGridLayout(new QWidget());
	QMainWindow::setCentralWidget(layout->parentWidget());

	const auto loadButton = new QPushButton("Load", this);
	const auto resetButton = new QPushButton("Reset", this);
	const auto showButton = new QPushButton("Show", this);
	const auto studyButton = new QPushButton("Study", this);
	
	const auto cList = { loadButton, resetButton, showButton, studyButton };
	auto count = 0;
	
	for (const auto& button : cList)
		layout->addWidget(button, 0, count++, 1, 1);
	
	QObject::connect(loadButton, SIGNAL(clicked(void)), this, SLOT(loadHistory(void)));
	QObject::connect(resetButton, SIGNAL(clicked(void)), this, SLOT(reset(void)));
	QObject::connect(showButton, SIGNAL(clicked(void)), this, SLOT(showCandlesticks(void)));
	QObject::connect(studyButton, SIGNAL(clicked(void)), this, SLOT(study(void)));
	
	QChartInterface::fileLabel->setMinimumWidth(200);
	layout->addWidget(QChartInterface::fileLabel, 0, count, 1, 8);
	count += 8;
	
	const auto cursorLabel = new QLabel("Cursor", this);
	cursorLabel->setMaximumWidth(50);
	QChartInterface::cursorEdit->setReadOnly(true);
	QChartInterface::cursorEdit->setMaximumWidth(80);
	
	const auto timestampLabel = new QLabel("Timestamp", this);
	timestampLabel->setMaximumWidth(75);
	QChartInterface::timestampEdit->setReadOnly(true);
	QChartInterface::timestampEdit->setMaximumWidth(70);
	
	const auto lowLabel = new QLabel("Low", this);
	lowLabel->setMaximumWidth(30);
	QChartInterface::lowEdit->setReadOnly(true);
	QChartInterface::lowEdit->setMaximumWidth(80);
	
	const auto highLabel = new QLabel("High", this);
	highLabel->setMaximumWidth(35);
	QChartInterface::highEdit->setReadOnly(true);
	QChartInterface::highEdit->setMaximumWidth(80);
	
	const auto floatValidator = new QRegularExpressionValidator(QRegularExpression("([0-9]*[.])?[0-9]+"), this);
	const auto intValidator = new QRegularExpressionValidator(QRegularExpression("([1-9]{1}[0-9]+)"), this);
	
	const auto timeSpotLabel = new QLabel("Spot", this);
	timeSpotLabel->setMaximumWidth(35);
	QChartInterface::timeSpotEdit->setValidator(intValidator);
	QChartInterface::timeSpotEdit->setMaximumWidth(60);
	
	const auto timeUnitLabel = new QLabel("TU", this);
	timeUnitLabel->setMaximumWidth(20);
	QChartInterface::timeUnitEdit->setValidator(intValidator);
	QChartInterface::timeUnitEdit->setMaximumWidth(30);
	
	const auto chartSizeLabel = new QLabel("NC", this);
	chartSizeLabel->setMaximumWidth(20);
	QChartInterface::chartSizeEdit->setValidator(intValidator);
	QChartInterface::chartSizeEdit->setMaximumWidth(40);
	
	const auto takeProfLabel = new QLabel("TP", this);
	takeProfLabel->setMaximumWidth(20);
	QChartInterface::takeProfitEdit->setValidator(floatValidator);
	QChartInterface::takeProfitEdit->setMaximumWidth(30);
	
	const auto stopLossLabel = new QLabel("SL", this);
	stopLossLabel->setMaximumWidth(20);
	QChartInterface::stopLossEdit->setValidator(floatValidator);
	QChartInterface::stopLossEdit->setMaximumWidth(30);
	
	const std::initializer_list<QWidget*> wList = { timeSpotLabel, QChartInterface::timeSpotEdit,
							timeUnitLabel, QChartInterface::timeUnitEdit, chartSizeLabel, QChartInterface::chartSizeEdit,
							takeProfLabel, QChartInterface::takeProfitEdit, stopLossLabel, QChartInterface::stopLossEdit,
							cursorLabel, QChartInterface::cursorEdit, timestampLabel, QChartInterface::timestampEdit,
							lowLabel, QChartInterface::lowEdit, highLabel, QChartInterface::highEdit };
	
	for (const auto& widget : wList)
		layout->addWidget(widget, 0, count++, 1, 1);

	QChartInterface::level005Box->setMaximumWidth(35);
	QChartInterface::level010Box->setMaximumWidth(40);
	QChartInterface::level025Box->setMaximumWidth(40);
	QChartInterface::level050Box->setMaximumWidth(40);
	QChartInterface::level100Box->setMaximumWidth(45);
	QChartInterface::level250Box->setMaximumWidth(45);
	QChartInterface::level500Box->setMaximumWidth(45);
	
	QChartInterface::level010Box->setChecked(true);
	
	const auto bList = { QChartInterface::level005Box, QChartInterface::level010Box, QChartInterface::level025Box, QChartInterface::level050Box,
				QChartInterface::level100Box, QChartInterface::level250Box, QChartInterface::level500Box, QChartInterface::eIBox };
	
	for (const auto& box : bList)
		layout->addWidget(box, 0, count++, 1, 1);
	
	QObject::connect(QChartInterface::level005Box, SIGNAL(toggled(bool)), this, SLOT(updateBlackLines(void)));
	QObject::connect(QChartInterface::level010Box, SIGNAL(toggled(bool)), this, SLOT(updateBlackLines(void)));
	QObject::connect(QChartInterface::level025Box, SIGNAL(toggled(bool)), this, SLOT(updateBlackLines(void)));
	QObject::connect(QChartInterface::level050Box, SIGNAL(toggled(bool)), this, SLOT(updateBlackLines(void)));
	QObject::connect(QChartInterface::level100Box, SIGNAL(toggled(bool)), this, SLOT(updateBlackLines(void)));
	QObject::connect(QChartInterface::level250Box, SIGNAL(toggled(bool)), this, SLOT(updateBlackLines(void)));
	QObject::connect(QChartInterface::level500Box, SIGNAL(toggled(bool)), this, SLOT(updateBlackLines(void)));
	
	QChartInterface::informationPanel->setMinimumWidth(450);
	QChartInterface::informationPanel->setReadOnly(true);
	QChartInterface::informationPanel->setOpenLinks(false);
	
	QChartInterface::candlestickView->setMinimumWidth(3'200);
	QChartInterface::candlestickView->setMinimumHeight(800);
	QChartInterface::candlestickView->setScene(QChartInterface::candlestickScene);
	QChartInterface::candlestickView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QChartInterface::candlestickView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QChartInterface::candlestickView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
	QChartInterface::candlestickView->setFocusPolicy(Qt::NoFocus);
	
	layout->addWidget(QChartInterface::informationPanel, 1, 0, 5, 4);
	layout->addWidget(QChartInterface::candlestickView, 1, 4, 5, count - 4);

	QObject::connect(QChartInterface::informationPanel, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(showNewCandlesticks(const QUrl&)));
	QChartInterface::reset();
}

HexCheckFile QChartInterface::check(void)
{
	HexCheckFile foo;
	foo.tradeTimeSpot = QChartInterface::timeSpotEdit->text().toUInt();
	foo.numberOfCandlesticks = QChartInterface::chartSizeEdit->text().toUInt();
	
	if (foo.numberOfCandlesticks < 100u)
	{
		QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") Chart must have at least 100 candlesticks.</p>";
		QChartInterface::updateInformationPanel();
		return foo;
	}
	
	foo.timeUnit = QChartInterface::timeUnitEdit->text().toUInt();
	
	if (foo.timeUnit < 1u or foo.timeUnit > 180u)
	{
		QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") Time unit out of range.</p>";
		QChartInterface::updateInformationPanel();
		return foo;
	}
	
	const auto tp = QChartInterface::takeProfitEdit->text().toDouble();
	
	if (tp < 0.25)
	{
		QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") TP must be at least 0.25.</p>";
		QChartInterface::updateInformationPanel();
		return foo;
	}
	
	const auto sl = QChartInterface::stopLossEdit->text().toDouble();
	
	if (sl < 0.)
	{
		QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") SL must be at least 0.</p>";
		QChartInterface::updateInformationPanel();
		return foo;
	}
	
	foo.takeProfit = tp;
	foo.stopLoss = sl;
	foo.abort = false;
	return foo;
}

void QChartInterface::drawBlackLines(void)
{
	const auto minValue = static_cast<qint32>(QChartInterface::candlestickRect.top() - 0.5f)/5*5;
	const auto maxValue = static_cast<qint32>(QChartInterface::candlestickRect.bottom() - 4.5f)/5*5;
	
	std::vector<HexLevelPack> newPacks;
	newPacks.reserve(static_cast<quint32>(maxValue - minValue)/5u + 1u);
	
	for (auto i = minValue; i <= maxValue; i += 5)
	{
		if (QChartInterface::level500Box->isChecked() and i % 500 == 0)
			newPacks.emplace_back(i);
		else if (QChartInterface::level250Box->isChecked() and i % 250 == 0)
			newPacks.emplace_back(i);
		else if (QChartInterface::level100Box->isChecked() and i % 100 == 0)
			newPacks.emplace_back(i);
		else if (QChartInterface::level050Box->isChecked() and i % 50 == 0)
			newPacks.emplace_back(i);
		else if (QChartInterface::level025Box->isChecked() and i % 25 == 0)
			newPacks.emplace_back(i);
		else if (QChartInterface::level010Box->isChecked() and i % 10 == 0)
			newPacks.emplace_back(i);
		else if (QChartInterface::level005Box->isChecked())
			newPacks.emplace_back(i);
	}
	
	const auto newEnd = newPacks.end();
	auto newIt = newPacks.begin();
	
	const auto oldEnd = QChartInterface::levelPacks.end();
	auto oldIt = QChartInterface::levelPacks.begin();
	
	QPen pen(Qt::black);
	pen.setWidth(0);
	
	while (newIt != newEnd or oldIt != oldEnd)
	{
		if (newIt == newEnd or (oldIt != oldEnd and oldIt->level < newIt->level))
		{
			QChartInterface::candlestickScene->removeItem(oldIt->lineItem);
			delete oldIt->lineItem;

			++oldIt;
			continue;
		}
		
		if (oldIt == oldEnd or (newIt != newEnd and newIt->level < oldIt->level))
		{
			newIt->lineItem = QChartInterface::candlestickScene->addLine(QChartInterface::candlestickRect.left(), newIt->level, QChartInterface::candlestickRect.right(), newIt->level, pen);
			newIt->lineItem->setZValue(-90.f);
			
			++newIt;
			continue;
		}
		
		newIt->lineItem = oldIt->lineItem;
		++newIt;
		++oldIt;
	}
	
	QChartInterface::levelPacks.swap(newPacks);
}

void QChartInterface::drawCandlesticks(quint32 sampleTimeSpot, quint32 numberOfCandlesticks, quint32 timeUnit, qreal tp, qreal sl)
{
	auto newHexStrips = QChartInterface::savedInformation.extractSample(sampleTimeSpot, numberOfCandlesticks, timeUnit, tp, sl);
	QChartInterface::candlestickScene->toggleUpdating();
	QChartInterface::updateCandlesticks(newHexStrips, sampleTimeSpot);
	
	QChartInterface::drawTimeLines();
	QChartInterface::drawBlackLines();
	
	QChartInterface::candlestickScene->update();
	QChartInterface::candlestickScene->setTimeSpot(sampleTimeSpot);
	QChartInterface::candlestickScene->toggleUpdating();
}

void QChartInterface::drawTimeLines(void)
{
	auto digit = QChartInterface::sceneItemInfo[0u].timestamp.at(4u);
	const auto pen = QPen(Qt::black, 0.f, Qt::DotLine);
	
	for (const auto& s : QChartInterface::sceneItemInfo)
	{
		if (s.timestamp[4u] == digit)
			continue;
		
		const auto& rect = s.background->rect();
		QChartInterface::candlestickScene->addLine(rect.left(), rect.bottom(), rect.left(), rect.top(), pen);
		digit = s.timestamp[4u];
	}
}

void QChartInterface::keyReleaseEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		{
			QChartInterface::showCandlesticks();
			break;
		}
		
		case Qt::Key_Left:
		{
			const auto jump = (QChartInterface::eIBox->isChecked() ? 1u : QChartInterface::timeUnitEdit->text().toUInt());
			const auto oldTimeSpot = QChartInterface::timeSpotEdit->text().toUInt();
			
			if (jump < oldTimeSpot)
			{
				const auto newTimeSpot = oldTimeSpot - jump;
				QChartInterface::timeSpotEdit->setText(QString::number(newTimeSpot));
				QChartInterface::showCandlesticks();
			}
			
			break;
		}
		
		case Qt::Key_Right:
		{
			const auto jump = (QChartInterface::eIBox->isChecked() ? 1u : QChartInterface::timeUnitEdit->text().toUInt());
			const auto oldTimeSpot = QChartInterface::timeSpotEdit->text().toUInt();
			
			if (oldTimeSpot + jump < 23'400u)
			{
				const auto newTimeSpot = oldTimeSpot + jump;
				QChartInterface::timeSpotEdit->setText(QString::number(newTimeSpot));
				QChartInterface::showCandlesticks();
			}
			
			break;
		}
		
		default:
			break;
	}
}

void QChartInterface::loadHistory(void)
{
	const auto filePath = QFileDialog::getOpenFileName(nullptr, "Load historical data", "input/");
	QFile dataFile(filePath);
	
	if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") Failed to open file.</p>";
		return QChartInterface::updateInformationPanel();
	}
	
	QTextStream fileReader(&dataFile);
	const auto minData = fileReader.readLine().split(' ');
	
	if (minData.size() != 4u)
	{
		QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") File [" + filePath + "] has wrong minimum data.</p>";
		return QChartInterface::updateInformationPanel();
	}
	
	const auto maxData = fileReader.readLine().split(' ');
	
	if (maxData.size() != 4u)
	{
		QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") File [" + filePath + "] has wrong maximum data.</p>";
		return QChartInterface::updateInformationPanel();
	}
	
	QChartInterface::savedInformation.setMinima(minData[0u].toDouble(), minData[1u].toDouble(), minData[2u].toDouble(), minData[3u].toDouble());
	QChartInterface::savedInformation.setMaxima(maxData[0u].toDouble(), maxData[1u].toDouble(), maxData[2u].toDouble(), maxData[3u].toDouble());
	QChartInterface::savedInformation.clear();
	
	auto lineCount = 0u;
	
	while (!fileReader.atEnd())
	{
		const auto data = fileReader.readLine().split(' ');
		
		if (data.size() != 2u)
		{
			QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") File [" + filePath + "] Line " + QString::number(lineCount) + " does not have two integers separated by a space character.</p>";
			return QChartInterface::updateInformationPanel();
		}
		else
			QChartInterface::savedInformation.saveCandlestick(data[0u].toDouble(), data[1u].toDouble());
		
		++lineCount;
	}
	
	const auto fileName = filePath.split('/').back();
	QChartInterface::fileLabel->setText(fileName);
	QChartInterface::logBody = "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") Loaded [" + fileName + "] file.</p><p></p>";
	QChartInterface::logBody += "<p.small>(" + QTime::currentTime().toString("hh:mm:ss") + ") Chart changed.</p>";
	
	QChartInterface::reset();
	QChartInterface::updateInformationPanel();
}

QRectF QChartInterface::NonFlatRectangle(const QRectF& rect)
{
	if (rect.height() != 0.f)
		return rect;
	
	return QRectF(rect.left(), rect.top() - 0.02f, rect.width(), 0.04f);
}

void QChartInterface::reset(void)
{
	QChartInterface::takeProfitEdit->setText("9");
	QChartInterface::stopLossEdit->setText("15");
	QChartInterface::chartSizeEdit->setText("200");
	QChartInterface::timeUnitEdit->setText("1");
	
	QChartInterface::level005Box->setChecked(false);
	QChartInterface::level010Box->setChecked(true);
	QChartInterface::level025Box->setChecked(false);
	QChartInterface::level050Box->setChecked(false);
	QChartInterface::level100Box->setChecked(false);
	QChartInterface::level250Box->setChecked(false);
	QChartInterface::level500Box->setChecked(false);
	
	if (QChartInterface::fileLabel->text() != "No file loaded.")
		QChartInterface::drawCandlesticks(0u, 200u, 1u, 9., 15.);
}

void QChartInterface::showCandlesticks(void)
{
	const auto report = QChartInterface::check();
	
	if (report.abort)
		return;
	
	QChartInterface::drawCandlesticks(report.tradeTimeSpot, report.numberOfCandlesticks, report.timeUnit, report.takeProfit, report.stopLoss);
}

void QChartInterface::showNewCandlesticks(const QUrl& url)
{
	const auto report = QChartInterface::check();
	
	if (report.abort)
		return;
	
	const auto str = url.url();
	const auto sampleTimeSpot = str.toUInt();
	
	QChartInterface::drawCandlesticks(sampleTimeSpot, report.numberOfCandlesticks, report.timeUnit, report.takeProfit, report.stopLoss);
	QChartInterface::timeSpotEdit->setText(str);
}

void QChartInterface::study(void)
{
	QChartInterface::showCandlesticks();
	
	const auto timeString = '(' + QTime::currentTime().toString("hh:mm:ss") + ')';
	QChartInterface::logBody += QChartInterface::savedInformation.sumUpBreaksAndDrops(timeString);
	QChartInterface::updateInformationPanel();
}

void QChartInterface::updateCandlesticks(std::vector<HexStrip>& newHexStrips, quint32 timeSpot)
{
	QChartInterface::candlestickScene->clear();
	QChartInterface::candlestickScene->resetHighlight();
	QChartInterface::levelPacks.clear();
	
	const auto numberOfCandlesticks = QChartInterface::chartSizeEdit->text().toUInt();
	auto maxHeight = -std::numeric_limits<qreal>::max();
	auto minHeight = std::numeric_limits<qreal>::max();
	
	QPen pen(numberOfCandlesticks <= 150u ? Qt::black : Qt::transparent);
	pen.setWidth(0);
	
	for (auto& s : newHexStrips)
	{
		const auto candlestick = QChartInterface::candlestickScene->addRect(QChartInterface::NonFlatRectangle(s.rectangle), pen);
		candlestick->setBrush(s.brush);
		candlestick->setZValue(1.f);
		candlestick->setAcceptHoverEvents(true);
		
		if (s.rectangle.top() < minHeight)
			minHeight = s.rectangle.top();
		
		if (s.rectangle.bottom() > maxHeight)
			maxHeight = s.rectangle.bottom();
	}
	
	auto count = 0;
	
	for (auto& s : newHexStrips)
	{
		const auto rect = QRectF(static_cast<qreal>(count), minHeight - 5.f, 1.f, maxHeight - minHeight + 10.f);
		s.background = QChartInterface::candlestickScene->addRect(rect);
		s.background->setBrush(s.timeSpot != timeSpot ? Qt::white : QColor(204, 255, 204));
		s.background->setPen(QPen(Qt::transparent));
		s.background->setZValue(-100.f);
		s.background->setAcceptHoverEvents(true);
		++count;
	}
	
	const auto spread = maxHeight - minHeight;
	minHeight -= spread/20.f;
	maxHeight += spread/20.f;
	
	QChartInterface::candlestickRect = QRectF(-1.f, minHeight, static_cast<qreal>(newHexStrips.size()) + 2.f, maxHeight - minHeight);
	QChartInterface::candlestickView->fitInView(QChartInterface::candlestickRect);
	
	const auto leftPos = QChartInterface::candlestickView->mapFromScene(newHexStrips[0u].rectangle.left() + 0.2f, newHexStrips[0u].rectangle.top());
	const auto rightPos = QChartInterface::candlestickView->mapFromScene(newHexStrips[0u].rectangle.right() - 0.2f, newHexStrips[0u].rectangle.top());
	const auto mapDifference = rightPos.x() - leftPos.x();
	
	for (auto& s : newHexStrips)
	{
		if (static_cast<quint32>(s.breakOrDrop) <= static_cast<quint32>('Z'))
		{
			const auto topLeftPos = QChartInterface::candlestickView->mapFromScene(s.rectangle.left() + 0.2f, s.rectangle.top());
			const auto topLeftY = QChartInterface::candlestickView->mapToScene(topLeftPos.x(), topLeftPos.y() - mapDifference - mapDifference/2).y();
			const auto bottomLeftY = QChartInterface::candlestickView->mapToScene(topLeftPos.x(), topLeftPos.y() - mapDifference/2).y();
			const auto rect = QRectF(s.rectangle.left() + 0.2f, topLeftY, s.rectangle.width() - 0.4f, bottomLeftY - topLeftY);
			
			const auto info = QChartInterface::candlestickScene->addRect(rect, pen);
			info->setZValue(1.f);
			
			switch (s.breakOrDrop)
			{
				case 'D':
					info->setBrush(QColor(102, 153, 255));
					break;
				
				case 'W':
					info->setBrush(QColor(0, 85, 255));
					break;
				
				case 'M':
					info->setBrush(QColor(0, 42, 127));
					break;
				
				case 'Y':
					info->setBrush(QColor(0, 0, 0));
					break;
			}
		}
		else if (static_cast<quint32>(s.breakOrDrop) >= static_cast<quint32>('a'))
		{
			const auto bottomLeftPos = QChartInterface::candlestickView->mapFromScene(s.rectangle.left() + 0.2f, s.rectangle.bottom());
			const auto topLeftY = QChartInterface::candlestickView->mapToScene(bottomLeftPos.x(), bottomLeftPos.y() + mapDifference/2).y();
			const auto bottomLeftY = QChartInterface::candlestickView->mapToScene(bottomLeftPos.x(), bottomLeftPos.y() + mapDifference + mapDifference/2).y();
			const auto rect = QRectF(s.rectangle.left() + 0.2f, topLeftY, s.rectangle.width() - 0.4f, bottomLeftY - topLeftY);
			
			const auto info = QChartInterface::candlestickScene->addRect(rect, pen);
			info->setZValue(1.f);
			
			switch (s.breakOrDrop)
			{
				case 'd':
					info->setBrush(QColor(255, 102, 102));
					break;
				
				case 'w':
					info->setBrush(QColor(255, 0, 0));
					break;
				
				case 'm':
					info->setBrush(QColor(127, 0, 0));
					break;
				
				case 'y':
					info->setBrush(QColor(0, 0, 0));
					break;
			}
		}
	}
	
	QChartInterface::sceneItemInfo.swap(newHexStrips);
}

void QChartInterface::updateInformationPanel(void)
{
	QChartInterface::informationPanel->setText(QChartInterface::logHeader + QChartInterface::logBody + QChartInterface::logFooter);
	const auto scrollbar = QChartInterface::informationPanel->verticalScrollBar();
	scrollbar->setValue(scrollbar->maximum());
}

void QChartInterface::updateBlackLines(void)
{
	QChartInterface::drawBlackLines();
	QChartInterface::candlestickScene->update();
}

#endif
