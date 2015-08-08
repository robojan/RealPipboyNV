#include "MapMarker.h"


MapMarker::MapMarker(uint32_t id, float x, float y, float z, enum Type type, const std::string &name, uint16_t flags, const std::string &reputation) :
	m_x(x), m_y(y), m_z(z), m_type(type), m_fullName(name), m_flags(flags), m_reputation(reputation), m_id(id)
{

}

MapMarker::~MapMarker()
{
}
