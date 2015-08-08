#pragma once

#include <cstdint>
#include <string>

class MapMarker
{
public:
	enum Flags{
		FLAG_VISIBLE = 0x1,
		FLAG_CANTRAVEL = 0x2,
		FLAG_HIDDEN = 0x4
	};
	enum Type{
		TYPE_NONE = 0,
		TYPE_CITY,
		TYPE_SETTLEMENT,
		TYPE_ENCAMPMENT,
		TYPE_NATURALLANDMARK,
		TYPE_CAVE,
		TYPE_FACTORY,
		TYPE_MEMORIAL,
		TYPE_MILITARY,
		TYPE_OFFICE,
		TYPE_TOWNRUINS,
		TYPE_URBANRUINS,
		TYPE_SEWERRUINS,
		TYPE_METRO,
		TYPE_VAULT
	};

	MapMarker(uint32_t id, float x, float y, float z, enum Type type, const std::string &name, uint16_t flags, const std::string &reputation);
	~MapMarker();

	inline void setVisible(bool visible) { m_flags = (m_flags & ~FLAG_VISIBLE) | (visible ? FLAG_VISIBLE : 0); }
	inline bool isVisible() const { return (m_flags & FLAG_VISIBLE) != 0; }
	inline void setCanTravel(bool visible) { m_flags = (m_flags & ~FLAG_CANTRAVEL) | (visible ? FLAG_CANTRAVEL : 0); }
	inline bool canTravel() const { return (m_flags & FLAG_CANTRAVEL) != 0; }
	inline void setHidden(bool visible) { m_flags = (m_flags & ~FLAG_HIDDEN) | (visible ? FLAG_HIDDEN : 0); }
	inline bool isHidden() const { return (m_flags & FLAG_HIDDEN) != 0; }
	inline const std::string &getName() const { return m_fullName; }
	inline void setName(const std::string &name) { m_fullName = name; }
	inline const std::string &getRepuation() const { return m_reputation; }
	inline void setReputation(const std::string &reputation) { m_reputation = reputation; }
	inline enum Type getType() const { return m_type; }
	inline void setType(enum Type type) { m_type = type; }
	inline uint16_t getFlags() const { return m_flags; }
	inline void setFlags(uint16_t flags) { m_flags = flags; }
	inline float getX() const { return m_x; }
	inline void setX(float x) { m_x = x; }
	inline float getY() const { return m_y; }
	inline void setY(float y) { m_y = y; }
	inline float getZ() const { return m_z; }
	inline void setZ(float z) { m_z = z; }
	inline uint32_t getID() const { return m_id; }
	inline void setID(uint32_t id) { m_id = id; }

private:
	enum Type m_type;
	uint16_t m_flags;
	std::string m_fullName;
	std::string m_reputation;
	float m_x, m_y, m_z;
	uint32_t m_id;
};

