#ifndef __Q_CUSTOM_GRAPHICS_SCENE_HPP__
#define __Q_CUSTOM_GRAPHICS_SCENE_HPP__

// Qt Libraries
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QString>

// Standard Libraries
#include <vector>
#include <iostream>

// Personal Libraries
#include "OtherClasses.hpp"

class QCustomGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
	
	private:
		
		const std::vector<HexStrip>&	strips;
		QLineEdit*			highEdit;
		QLineEdit*			lowEdit;
		QLineEdit*			timestampEdit;
		QLineEdit*			cursorEdit;
		
		quint32				currentTimeSpot = 0u;
		qint32				currentHexStrip = -1'000'000;
		bool				stopUpdating = false;
	
	public:
	
		inline QCustomGraphicsScene(QObject*, const std::vector<HexStrip>&);
		
		inline void			mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
		inline void			resetHighlight(void);
		inline void			setEdits(QLineEdit*, QLineEdit*, QLineEdit*, QLineEdit*);
		inline void			setTimeSpot(quint32);
		inline void 			toggleUpdating(void);
};

QCustomGraphicsScene::QCustomGraphicsScene(QObject* parent, const std::vector<HexStrip>& str) : QGraphicsScene(parent), strips(str)
{
}

void QCustomGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	if (QCustomGraphicsScene::strips.empty() or QCustomGraphicsScene::stopUpdating)
		return;
	
	const auto xValue = static_cast<qint32>(mouseEvent->scenePos().x());;
	
	if (xValue != QCustomGraphicsScene::currentHexStrip)
	{
		bool shouldUpdate = false;
		
		if (QCustomGraphicsScene::currentHexStrip >= 0 and static_cast<quint32>(QCustomGraphicsScene::currentHexStrip) < QCustomGraphicsScene::strips.size())
		{
			const auto& strip = QCustomGraphicsScene::strips[QCustomGraphicsScene::currentHexStrip];
			strip.background->setBrush(strip.timeSpot != QCustomGraphicsScene::currentTimeSpot ? Qt::white : QColor(204, 255, 204));
			shouldUpdate = true;
		}
		
		if (xValue >= 0 and static_cast<quint32>(xValue) < QCustomGraphicsScene::strips.size())
		{
			QCustomGraphicsScene::currentHexStrip = xValue;
			const auto& strip = QCustomGraphicsScene::strips[static_cast<quint32>(xValue)];
			
			strip.background->setBrush(QColor(225, 225, 225));
			shouldUpdate = true;
			
			QCustomGraphicsScene::timestampEdit->setText(strip.timestamp);
			QCustomGraphicsScene::highEdit->setText(QString::number(strip.high, 'g', 7));
			QCustomGraphicsScene::lowEdit->setText(QString::number(strip.low, 'g', 7));
		}
		else
		{
			QCustomGraphicsScene::currentHexStrip = -1'000'000;
			QCustomGraphicsScene::timestampEdit->setText("");
			QCustomGraphicsScene::highEdit->setText("");
			QCustomGraphicsScene::lowEdit->setText("");
		}
		
		if (shouldUpdate)
			QGraphicsScene::update();
	}
	
	const auto yValue = static_cast<qreal>(static_cast<qint32>(0.5 - mouseEvent->scenePos().y()*100.)*0.01);
	const auto value = QString::number(yValue, 'g', yValue > 10'000. ? 7 : 6);
	QCustomGraphicsScene::cursorEdit->setText(value);
}

void QCustomGraphicsScene::resetHighlight(void)
{
	QCustomGraphicsScene::currentHexStrip = -1'000'000;
}

void QCustomGraphicsScene::setEdits(QLineEdit* he, QLineEdit* le, QLineEdit* te, QLineEdit* ce)
{
	QCustomGraphicsScene::highEdit = he;
	QCustomGraphicsScene::lowEdit = le;
	QCustomGraphicsScene::timestampEdit = te;
	QCustomGraphicsScene::cursorEdit = ce;
}

void QCustomGraphicsScene::setTimeSpot(quint32 ts)
{
	QCustomGraphicsScene::currentTimeSpot = ts;
}

void QCustomGraphicsScene::toggleUpdating(void)
{
	QCustomGraphicsScene::stopUpdating = not QCustomGraphicsScene::stopUpdating;
}

#endif
