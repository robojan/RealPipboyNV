#include "FOResourceManager.h"

using namespace std;

FOResourceFile::FOResourceFile(uint64_t hash, int archive, uint32_t offset, uint32_t size,
	uint32_t uncompressedSize /*= 0*/, bool compressed /*= false*/) :
	m_hash(hash), m_archive(archive), m_offset(offset), m_fileSize(size),
	m_uncompressedSize(uncompressedSize), m_compressed(compressed)
{

}

FOResourceFile::FOResourceFile(uint64_t hash, const std::string &path, uint32_t fileSize) :
	m_hash(hash), m_path(path), m_compressed(false), m_archive(-1),
	m_offset(0), m_fileSize(fileSize)
{
}

FOResourceFile::~FOResourceFile()
{

}

void FOResourceFile::setPath(const std::string &path)
{
	m_path = path;
}

const std::string & FOResourceFile::getPath() const
{
	return m_path;
}

void FOResourceFile::setName(const std::string &name)
{
	m_name = name;
}

const std::string & FOResourceFile::getName() const
{
	return m_name;
}

bool FOResourceFile::isInArchive() const
{
	return m_archive >= 0;
}

int FOResourceFile::getArchive() const
{
	return m_archive;
}

void FOResourceFile::setArchive(int archive)
{
	m_archive = archive;
}

uint64_t FOResourceFile::getHash() const
{
	return m_hash;
}

void FOResourceFile::setHash(uint64_t hash)
{
	m_hash = hash;
}

uint32_t FOResourceFile::getOffset() const
{
	return m_offset;
}

void FOResourceFile::setOffset(uint32_t offset)
{
	m_offset = offset;
}

uint32_t FOResourceFile::getFileSize() const
{
	return m_compressed ? m_uncompressedSize : m_fileSize;
}

void FOResourceFile::setFileSize(uint32_t fileSize)
{
	if (m_compressed)
		m_uncompressedSize = fileSize;
	else
		m_fileSize = fileSize;
}

uint32_t FOResourceFile::getCompressedFileSize() const
{
	return m_compressed ? m_fileSize : 0;
}

void FOResourceFile::setCompressedFileSize(uint32_t size)
{
	if (m_compressed) {
		m_fileSize = size;
	}
}

uint32_t FOResourceFile::getRawFileSize() const {
	return m_fileSize;
}

bool FOResourceFile::isCompressed() const
{
	return m_compressed;
}

void FOResourceFile::setCompressed(bool compressed)
{
	m_compressed = compressed;
}



FOResourceFolder::FOResourceFolder(uint64_t hash) : 
	m_hash(hash)
{

}

FOResourceFolder::~FOResourceFolder()
{
	for (map<uint64_t, FOResourceFile *>::iterator it = m_files.begin(); it != m_files.end();
		++it) {
		delete it->second;
	}
}

void FOResourceFolder::setFile(std::string fileName, FOResourceFile *file)
{
	uint64_t hash = FOResourceManager::hash("", fileName);
	setFile(hash, file);
}

void FOResourceFolder::setFile(uint64_t hash, FOResourceFile *file)
{
	map<uint64_t, FOResourceFile*>::iterator it = m_files.find(hash);
	if (it != m_files.end()) {
		delete it->second;
	}
	m_files[hash] = file;
}

FOResourceFile * FOResourceFolder::getFile(std::string file)
{
	uint64_t hash = FOResourceManager::hash("", file);
	return getFile(hash);
}

FOResourceFile * FOResourceFolder::getFile(uint64_t hash)
{
	map<uint64_t, FOResourceFile*>::iterator it = m_files.find(hash);
	if (it == m_files.end()) {
		return NULL;
	}
	return m_files.at(hash);
}

const FOResourceFile * FOResourceFolder::getFile(uint64_t hash) const
{
	map<uint64_t, FOResourceFile*>::const_iterator it = m_files.find(hash);
	if (it == m_files.end()) {
		return NULL;
	}
	return m_files.at(hash);
}


void FOResourceFolder::setPath(const std::string &path)
{
	m_path = path;
}

const std::string & FOResourceFolder::getPath()
{
	return m_path;
}
