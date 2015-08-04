#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		year
// 7		1		int8		month
// 8		1		int8		day
// 9		1		int8		hour
// 10		1		int8		minute
// 11		2		int16		locationName length
// 13		n		char[]		locationName
// ..		4		int32		map usableWidth
// ..		4		int32		map usableHeight
// ..		2		int16		map cell NW X
// ..		2		int16		map cell NW Y
// ..		2		int16		map cell SE	X
// ..		2		int16		map cell SE Y
// ..		4		float		map scale
// ..		4		float		map offset X
// ..		4		float		map offset Y
// ..		2		int16		map texure length
// ..		n		char[]		texture path

class SetWorldInfoPacket :
	public DataPacket
{
public:
	SetWorldInfoPacket(const char *buffer, size_t bufferSize);
	SetWorldInfoPacket(short year, signed char month, signed char day, signed char hour, 
					   signed char minute, const std::string &location,
		int usableMapWidth, int usableMapHeight, int16_t cellNWX, int16_t cellNWY,
		int16_t cellSEX, int16_t cellSEY, float mapScale, float mapOffsetX, float mapOffsetY,
		const std::string &mapPath);
	virtual ~SetWorldInfoPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

	short getYear() const;
	signed char getMonth() const;
	signed char getDay() const;
	signed char getHour() const;
	signed char getMinute() const;
	const std::string &getLocation() const;
	inline int getUsableWidth() const { return m_usableWidth; }
	inline int getUsableHeight() const { return m_usableHeight; }
	inline int16_t getCellNWX() const { return m_cellNWX; }
	inline int16_t getCellNWY() const { return m_cellNWY; }
	inline int16_t getCellSEX() const { return m_cellSEX; }
	inline int16_t getCellSEY() const { return m_cellSEY; }
	inline float getMapScale() const { return m_mapScale; }
	inline float getMapOffsetX() const { return m_mapOffsetX; }
	inline float getMapOffsetY() const { return m_mapOffsetY; }
	inline const std::string &getMapPath() const { return m_mapPath; }
	void setYear(short year);
	void setMonth(signed char month);
	void setDay(signed char day);
	void setHour(signed char hour);
	void setMinute(signed char minute);
	inline void setUsableDim(int width, int height) { m_usableWidth = width; m_usableHeight = height; }
	inline void setCellNW(int16_t x, int16_t y) { m_cellNWX = x, m_cellNWY = y; }
	inline void setCellSE(int16_t x, int16_t y) { m_cellSEX = x, m_cellSEY = y; }
	inline void setMapScale(float scale) { m_mapScale = scale; }
	inline void setMapOffset(float x, float y) { m_mapOffsetX = x; m_mapOffsetY = y; }
	inline void setMapPath(const std::string &path) { m_mapPath = path; }


private:
	void readPacket(const char *buffer, size_t bufferSize);

	short m_year;
	signed char m_month, m_day, m_hour, m_minute;
	std::string m_locationName;
	int m_usableWidth, m_usableHeight;
	int16_t m_cellNWX, m_cellNWY, m_cellSEX, m_cellSEY;
	float m_mapScale, m_mapOffsetX, m_mapOffsetY;
	std::string m_mapPath;
};

