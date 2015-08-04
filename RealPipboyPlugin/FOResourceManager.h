#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "common/IPrefix.h"
#include "common/IDataStream.h"
#include "common/IFileStream.h"

class FOResourceFile {
public:
	FOResourceFile(uint64_t hash, int archive, uint32_t offset, uint32_t size, 
		uint32_t uncompressedSize = 0, bool compressed = false);
	FOResourceFile(uint64_t hash, const std::string &path, uint32_t fileSize);
	~FOResourceFile();

	void setPath(const std::string &path);
	const std::string &getPath() const;
	void setName(const std::string &name);
	const std::string &getName() const;

	bool isInArchive() const;
	int getArchive() const;
	void setArchive(int archive);
	uint64_t getHash() const;
	void setHash(uint64_t hash);
	uint32_t getOffset() const;
	void setOffset(uint32_t offset);
	uint32_t getFileSize() const;
	void setFileSize(uint32_t fileSize);
	uint32_t getRawFileSize() const;
	uint32_t getCompressedFileSize() const;
	void setCompressedFileSize(uint32_t size);
	bool isCompressed() const;
	void setCompressed(bool compressed);

private:
	int m_archive;
	bool m_compressed;
	std::string m_path;
	std::string m_name;
	uint64_t m_hash;
	uint32_t m_offset;
	uint32_t m_fileSize;
	uint32_t m_uncompressedSize;
};

class FOResourceFolder {
public:
	FOResourceFolder(uint64_t hash);
	~FOResourceFolder();

	void setFile(std::string fileName, FOResourceFile *file);
	void setFile(uint64_t hash, FOResourceFile *file);
	FOResourceFile *getFile(std::string file);
	FOResourceFile *getFile(uint64_t hash);
	const FOResourceFile *getFile(uint64_t hash) const;

	void setPath(const std::string &path);
	const std::string &getPath();

private:
	std::map<uint64_t, FOResourceFile *> m_files;
	std::string m_path;
	uint64_t m_hash;
};

class FOResourceManager
{
	static FOResourceManager m_instance;
public:
	inline static FOResourceManager *getSingleton() { return &
		m_instance; }

	void load(std::string path);

	void loadArchive(std::string archive);
	void loadFolder(std::string base, std::string folder);

	const FOResourceFolder *getFolder(std::string path) const;
	const FOResourceFolder *getFolder(uint64_t hash) const;

	const FOResourceFile *getFile(std::string path) const;
	const FOResourceFile *getFile(std::string path, std::string fileName) const;
	const FOResourceFile *getFile(uint64_t pathHash, uint64_t fileHash) const;

	const std::string getArchive(int id);

	static std::string normalizePath(std::string path);
	static uint32_t hashStr(std::string str);
	static uint64_t hashPair(std::string file, std::string ext);
	static uint64_t hash(std::string path, std::string filename);
private:
	static int m_archiveID;
	std::map<int, std::string> m_archives;
	std::map<uint64_t, FOResourceFolder *> m_folders;

	FOResourceManager();
	~FOResourceManager();

	int getOrAddArchiveID(const std::string &archive);

};

