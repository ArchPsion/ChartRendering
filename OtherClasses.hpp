#ifndef __OTHER_CLASSES_HPP__
#define __OTHER_CLASSES_HPP__

// Qt Libraries
#include <QBrush>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>

struct HexCandlestick
{
	qreal		low;
	qreal		high;
	
	qreal		levelToBuyOrSell = 0.;
	char		winningOrder = 'u';
	char		breakOrDrop = '_';
	
	inline HexCandlestick(qreal l, qreal h) : low(l), high(h)
	{
	}
};

struct HexCheckFile
{
	quint32		tradeTimeSpot;
	quint32		timeUnit;
	quint32		numberOfCandlesticks;
	
	qreal		takeProfit;
	qreal		stopLoss;
	bool		abort = true;
};

struct HexInfoFile
{
	qreal		min = std::numeric_limits<qreal>::max();
	qreal		rawMin = std::numeric_limits<qreal>::max();
	
	qreal		max = -std::numeric_limits<qreal>::max();
	qreal		rawMax = -std::numeric_limits<qreal>::max();
};

struct HexLevelPack
{
	QGraphicsLineItem*		lineItem = nullptr;
	qint32				level;
	
	inline HexLevelPack(qint32 l) : level(l)
	{
	}
};

struct HexStrip
{
	QGraphicsRectItem*		background = nullptr;
	QBrush				brush;
	
	QString				timestamp;
	QRectF				rectangle;
	
	qreal				low = 0.;
	qreal				high = 0.;
	
	quint32				timeSpot;
	char				breakOrDrop = '_';
	
	inline HexStrip(const QString& t, QRectF r, qreal l, qreal h, quint32 ts, char b) : timestamp(t), rectangle(r), low(l), high(h), timeSpot(ts), breakOrDrop(b)
	{
	}
};

#endif
