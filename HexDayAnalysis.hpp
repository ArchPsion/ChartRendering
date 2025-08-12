#ifndef __DAY_ANALYSIS_HPP__
#define __DAY_ANALYSIS_HPP__

// Standard Libraries
#include <algorithm>
#include <array>
#include <limits>
#include <vector>

// Personal Libraries
#include "OtherClasses.hpp"

class HexDayAnalysis
{
	private:
		
		std::vector<HexCandlestick>		candlesticks;
		HexInfoFile				dInfo;
		HexInfoFile				wInfo;
		HexInfoFile				mInfo;
		HexInfoFile				yInfo;
		
		qreal					takeProfit = 0.;
		qreal					stopLoss = 0.;
		bool					studyNotCompleted = true;
		
		inline void				appendCouple(QString&, quint32&, quint32) const;
		inline std::vector<HexStrip>		extractCandlestickData(quint32, quint32, quint32) const;
		inline QString				record(const QString&, const QString&, const std::array<std::vector<quint32>, 4u>&) const;
		inline void				study(qreal, qreal);
		inline void				update(HexCandlestick&);
		inline quint32				strictBuyAndSell(std::vector<HexCandlestick>::const_iterator, qreal, qreal) const;
		inline quint32				strictSellAndBuy(std::vector<HexCandlestick>::const_iterator, qreal, qreal) const;
		inline QString				timeString(quint32) const;
	
	public:
	
		inline					HexDayAnalysis(void);
		
		inline void				clear(void);
		inline std::vector<HexStrip>		extractSample(quint32, quint32, quint32, qreal, qreal);
		inline void				saveCandlestick(qreal, qreal);
		inline void				setMaxima(qreal, qreal, qreal, qreal);
		inline void				setMinima(qreal, qreal, qreal, qreal);
		inline QString				sumUpBreaksAndDrops(const QString&) const;
};

HexDayAnalysis::HexDayAnalysis(void)
{
	HexDayAnalysis::candlesticks.reserve(23'400u);
}

void HexDayAnalysis::appendCouple(QString& result, quint32& oldCouple, quint32 count) const
{
	const auto timestamp = count*23'400u/HexDayAnalysis::candlesticks.size();
	const auto hour = 15u + (timestamp + 1'800u)/3'600u;
	const auto minute = (30u + timestamp/60u) % 60u;
	const auto newCouple = 100u*hour + minute;
	
	if (newCouple != oldCouple)
	{
		const QString zeroPadding = (minute < 10u ? "0" : "");
		result += " <a href=" + QString::number(count) + ">[" + QString::number(hour) + ':' + zeroPadding + QString::number(minute) + "]</a>";
		oldCouple = newCouple;
	}
}

void HexDayAnalysis::clear(void)
{
	HexDayAnalysis::candlesticks.clear();
	HexDayAnalysis::studyNotCompleted = true;
}

std::vector<HexStrip> HexDayAnalysis::extractCandlestickData(quint32 start, quint32 numberOfCandlesticks, quint32 secondsPerCandlestick) const
{
	std::vector<HexStrip> foo;
	foo.reserve(numberOfCandlesticks);
	
	auto it = HexDayAnalysis::candlesticks.cbegin() + start;
	
	for (auto i = 0u; i < numberOfCandlesticks; ++i)
	{
		auto max = -std::numeric_limits<qreal>::max();
		auto min = std::numeric_limits<qreal>::max();
		
		auto breakOrDrop = '_';
		auto bCount = 0u;
		auto sCount = 0u;
		auto uCount = 0u;
		auto BCount = 0u;
		auto SCount = 0u;
		
		for (auto j = 0u; j < secondsPerCandlestick; ++j)
		{
			if (it->low < min)
				min = it->low;
			
			if (it->high > max)
				max = it->high;
			
			if (it->breakOrDrop != '_' and it->breakOrDrop != breakOrDrop)
				breakOrDrop = it->breakOrDrop;
			
			switch (it->winningOrder)
			{
				case 'b':
					++bCount;
					break;
				
				case 's':
					++sCount;
					break;
				
				case 'u':
					++uCount;
					break;
				
				case 'B':
					++BCount;
					break;
				
				case 'S':
					++SCount;
					break;
			}
			
			++it;
		}
		
		const auto rect = QRectF(static_cast<qreal>(i) + 0.1f, -max, 0.8f, max - min);
		const auto timeSpot = start + i*secondsPerCandlestick;
		
		foo.emplace_back(HexDayAnalysis::timeString(timeSpot), rect, min, max, timeSpot, breakOrDrop);
		
		if (uCount != 0u)
			foo.back().brush = QBrush(QColor(153, 153, 153));
		else if (BCount != 0u)
			foo.back().brush = (SCount != 0u ? QBrush(QColor(153, 153, 153)) : QBrush(QColor(102, 153, 255)));
		else if (SCount != 0u)
			foo.back().brush = QBrush(QColor(255, 102, 102));
		else if (sCount == 0u)
			foo.back().brush = (bCount != 0u ? QBrush(QColor(102, 153, 255), Qt::Dense4Pattern) : QBrush(QColor(153, 153, 255), Qt::Dense4Pattern));
		else if (bCount == 0u)
			foo.back().brush = QBrush(QColor(255, 102, 102), Qt::Dense4Pattern);
		else
			foo.back().brush = QBrush(QColor(153, 153, 255), Qt::Dense4Pattern);
	}
	
	return foo;
}

std::vector<HexStrip> HexDayAnalysis::extractSample(quint32 positionInData, quint32 numberOfCandlesticks, quint32 timeUnit, qreal tp, qreal sl)
{
	if (HexDayAnalysis::studyNotCompleted or HexDayAnalysis::takeProfit != tp or HexDayAnalysis::stopLoss != sl)
		HexDayAnalysis::study(tp, sl);
	
	const auto size = HexDayAnalysis::candlesticks.size();
	const auto numberOfElementaryCandlesticks = numberOfCandlesticks*timeUnit;
	
	if (positionInData < numberOfElementaryCandlesticks/5u)
		return HexDayAnalysis::extractCandlestickData(0u, numberOfCandlesticks, timeUnit);
	
	if (positionInData - numberOfElementaryCandlesticks/5u + numberOfElementaryCandlesticks >= size)
		return HexDayAnalysis::extractCandlestickData(size - numberOfElementaryCandlesticks, numberOfCandlesticks, timeUnit);
	
	return HexDayAnalysis::extractCandlestickData(positionInData - numberOfElementaryCandlesticks/5u, numberOfCandlesticks, timeUnit);
}

QString HexDayAnalysis::record(const QString& time, const QString& str, const std::array<std::vector<quint32>, 4u>& info) const
{
	const auto sum = info[0u].size() + info[1u].size() + info[2u].size() + info[3u].size();
	
	if (sum == 0u)
		return "";
	
	QString letterS = (sum > 1u ? "s" : "");
	QString result = "<p.small>" + time + ' ' + str + ' ' + QString::number(sum) + " occurrence" + letterS + ".</p>";
	
	const auto bRatio = info[0u].size()*100./sum;
	result += "<ul><li>Buy wins " + QString::number(bRatio, 'f', 2) + '%';
	auto oldCouple = 0u;
	
	for (const auto& count : info[0u])
		HexDayAnalysis::appendCouple(result, oldCouple, count);
	
	const auto sRatio = info[1u].size()*100./sum;
	result += "</li><li>Sell wins " + QString::number(sRatio, 'f', 2) + '%';
	oldCouple = 0u;
	
	for (const auto& count : info[1u])
		HexDayAnalysis::appendCouple(result, oldCouple, count);
	
	const auto eRatio = info[2u].size()*100./sum;
	result += "</li><li>Either wins " + QString::number(eRatio, 'f', 2) + '%';
	oldCouple = 0u;
	
	for (const auto& count : info[2u])
		HexDayAnalysis::appendCouple(result, oldCouple, count);
	
	const auto uRatio = info[3u].size()*100./sum;
	result += "</li><li>Uncertainty wins " + QString::number(uRatio, 'f', 2) + '%';
	oldCouple = 0u;
	
	for (const auto& count : info[3u])
		HexDayAnalysis::appendCouple(result, oldCouple, count);
	
	const auto bPE = HexDayAnalysis::takeProfit*(bRatio + eRatio)/100. - HexDayAnalysis::stopLoss*sRatio/100.;
	const auto sPE = HexDayAnalysis::takeProfit*(sRatio + eRatio)/100. - HexDayAnalysis::stopLoss*bRatio/100.;
	
	result += "</li></ul><p.small>" + time + " Profit expectations: " + QString::number(bPE, 'f', 2) + " (Buy) and " + QString::number(sPE, 'f', 2) + " (Sell).</p>";
	return result;
}

void HexDayAnalysis::saveCandlestick(qreal low, qreal high)
{
	HexDayAnalysis::candlesticks.emplace_back(low, high);
}

void HexDayAnalysis::setMaxima(qreal d, qreal w, qreal m, qreal y)
{
	HexDayAnalysis::dInfo.max = d;
	HexDayAnalysis::wInfo.max = w;
	HexDayAnalysis::mInfo.max = m;
	HexDayAnalysis::yInfo.max = y;
	
	HexDayAnalysis::dInfo.rawMax = d;
	HexDayAnalysis::wInfo.rawMax = w;
	HexDayAnalysis::mInfo.rawMax = m;
	HexDayAnalysis::yInfo.rawMax = y;
}

void HexDayAnalysis::setMinima(qreal d, qreal w, qreal m, qreal y)
{
	HexDayAnalysis::dInfo.min = d;
	HexDayAnalysis::wInfo.min = w;
	HexDayAnalysis::mInfo.min = m;
	HexDayAnalysis::yInfo.min = y;
	
	HexDayAnalysis::dInfo.rawMin = d;
	HexDayAnalysis::wInfo.rawMin = w;
	HexDayAnalysis::mInfo.rawMin = m;
	HexDayAnalysis::yInfo.rawMin = y;
}

quint32 HexDayAnalysis::strictBuyAndSell(std::vector<HexCandlestick>::const_iterator it, qreal lowerPriceLimit, qreal upperPriceLimit) const
{
	const auto end = HexDayAnalysis::candlesticks.cend();
	auto count = 0u;
	
	while (it != end and it->high < upperPriceLimit)
	{
		if (it->low <= lowerPriceLimit)
			return 50'000u;
		
		++it;
		++count;
	}
	
	return (it != end and lowerPriceLimit < it->low ? count : 50'000u);
}

quint32 HexDayAnalysis::strictSellAndBuy(std::vector<HexCandlestick>::const_iterator it, qreal lowerPriceLimit, qreal upperPriceLimit) const
{
	const auto end = HexDayAnalysis::candlesticks.cend();
	auto count = 0u;
	
	while (it != end and lowerPriceLimit < it->low)
	{
		if (upperPriceLimit <= it->high)
			return 50'000u;
		
		++it;
		++count;
	}
	
	return (it != end and it->high < upperPriceLimit ? count : 50'000u);
}

void HexDayAnalysis::study(qreal tp, qreal sl)
{
	HexDayAnalysis::takeProfit = tp;
	HexDayAnalysis::stopLoss = sl;
	
	dInfo.min = dInfo.rawMin;
	wInfo.min = wInfo.rawMin;
	mInfo.min = mInfo.rawMin;
	yInfo.min = yInfo.rawMin;
	
	dInfo.max = dInfo.rawMax;
	wInfo.max = wInfo.rawMax;
	mInfo.max = mInfo.rawMax;
	yInfo.max = yInfo.rawMax;
	
	auto it = HexDayAnalysis::candlesticks.cbegin();
	
	for (auto& cs : HexDayAnalysis::candlesticks)
	{
		HexDayAnalysis::update(cs);
		
		const auto buyPrice = (cs.breakOrDrop != '_' ? cs.levelToBuyOrSell : cs.high);
		const auto buy = HexDayAnalysis::strictBuyAndSell(it + 1u, buyPrice - sl, buyPrice + tp);
		
		const auto sellPrice = (cs.breakOrDrop != '_' ? cs.levelToBuyOrSell : cs.low);
		const auto sell = HexDayAnalysis::strictSellAndBuy(it + 1u, sellPrice - tp, sellPrice + sl);
		
		if (buy > sell)
			cs.winningOrder = (buy > 23'400u ? 'S' : 's');
		else if (buy < sell)
			cs.winningOrder = (sell > 23'400u ? 'B' : 'b');
		else
			cs.winningOrder = (buy > 23'400u ? 'u' : 'e');
		
		++it;
	}
	
	HexDayAnalysis::studyNotCompleted = false;
}

QString HexDayAnalysis::sumUpBreaksAndDrops(const QString& time) const
{
	std::array<std::vector<quint32>, 4u> dBreaks = { };
	std::array<std::vector<quint32>, 4u> wBreaks = { };
	std::array<std::vector<quint32>, 4u> mBreaks = { };
	std::array<std::vector<quint32>, 4u> yBreaks = { };
	
	std::array<std::vector<quint32>, 4u> dDrops = { };
	std::array<std::vector<quint32>, 4u> wDrops = { };
	std::array<std::vector<quint32>, 4u> mDrops = { };
	std::array<std::vector<quint32>, 4u> yDrops = { };
	
	auto count = 0u;
	
	for (const auto& cs : HexDayAnalysis::candlesticks)
	{
		if (cs.breakOrDrop != '_')
		{
			quint32 index = 3u;
			
			switch (cs.winningOrder)
			{
				case 'B':
					index = 0u;
					break;
				
				case 'S':
					index = 1u;
					break;
				
				case 'b':
				case 'e':
				case 's':
					index = 2u;
					break;
			}
			
			switch (cs.breakOrDrop)
			{
				case 'y':
					yDrops[index].push_back(count);
					break; //[[fallthrough]];
				
				case 'm':
					mDrops[index].push_back(count);
					break; //[[fallthrough]];
				
				case 'w':
					wDrops[index].push_back(count);
					break; //[[fallthrough]];
				
				case 'd':
					dDrops[index].push_back(count);
					break;
				
				case 'Y':
					yBreaks[index].push_back(count);
					break; //[[fallthrough]];
				
				case 'M':
					mBreaks[index].push_back(count);
					break; //[[fallthrough]];
				
				case 'W':
					wBreaks[index].push_back(count);
					break; //[[fallthrough]];
				
				case 'D':
					dBreaks[index].push_back(count);
					break;
			}
		}
		
		++count;
	}
	
	QString aftermath = "";
	aftermath += HexDayAnalysis::record(time, "Day breaks info!", dBreaks);
	aftermath += HexDayAnalysis::record(time, "Week breaks info!", wBreaks);
	aftermath += HexDayAnalysis::record(time, "Month breaks info!", mBreaks);
	aftermath += HexDayAnalysis::record(time, "Year breaks info!", yBreaks);
	aftermath += HexDayAnalysis::record(time, "Day drops info!", dDrops);
	aftermath += HexDayAnalysis::record(time, "Week drops info!", wDrops);
	aftermath += HexDayAnalysis::record(time, "Month drops info!", mDrops);
	aftermath += HexDayAnalysis::record(time, "Year drops info!", yDrops);
	return aftermath;
}

QString HexDayAnalysis::timeString(quint32 timeSpot) const
{
	const auto timestamp = timeSpot*23'400u/HexDayAnalysis::candlesticks.size();
	const auto hour = 15u + (timestamp + 1'800u)/3'600u;
	const auto minute = (30u + timestamp/60u) % 60u;
	const auto second = timestamp % 60u;
	
	const QString zeroPadding1 = (minute < 10u ? "0" : "");
	const QString zeroPadding2 = (second < 10u ? "0" : "");
	return QString::number(hour) + ':' + zeroPadding1 + QString::number(minute) + ':' + zeroPadding2 + QString::number(second);
}

void HexDayAnalysis::update(HexCandlestick& cs)
{
	if (HexDayAnalysis::dInfo.min > cs.low)
	{
		cs.levelToBuyOrSell = HexDayAnalysis::dInfo.min - 0.25;
		HexDayAnalysis::dInfo.min = cs.low;
		cs.breakOrDrop = 'd';
		
		if (HexDayAnalysis::wInfo.min > cs.low)
		{
			HexDayAnalysis::wInfo.min = cs.low;
			cs.breakOrDrop = 'w';
			
			if (HexDayAnalysis::mInfo.min > cs.low)
			{
				HexDayAnalysis::mInfo.min = cs.low;
				cs.breakOrDrop = 'm';
				
				if (HexDayAnalysis::yInfo.min > cs.low)
				{
					HexDayAnalysis::yInfo.min = cs.low;
					cs.breakOrDrop = 'y';
				}
			}
		}
	}
	else if (HexDayAnalysis::dInfo.max < cs.high)
	{
		cs.levelToBuyOrSell = HexDayAnalysis::dInfo.max + 0.25;
		HexDayAnalysis::dInfo.max = cs.high;
		cs.breakOrDrop = 'D';
		
		if (HexDayAnalysis::wInfo.max < cs.high)
		{
			HexDayAnalysis::wInfo.max = cs.high;
			cs.breakOrDrop = 'W';
			
			if (HexDayAnalysis::mInfo.max < cs.high)
			{
				HexDayAnalysis::mInfo.max = cs.high;
				cs.breakOrDrop = 'M';
				
				if (HexDayAnalysis::yInfo.max < cs.high)
				{
					HexDayAnalysis::yInfo.max = cs.high;
					cs.breakOrDrop = 'Y';
				}
			}
		}
	}
}

#endif
