#include "FOResourceManager.h"

#include "Common/IDirectoryIterator.h"
#include "Common/IFileStream.h"

#include <algorithm>

using namespace std;

enum class ArchiveFlags {
	ARCHIVE_DIRNAMES =   0x01,
	ARCHIVE_FILENAMES =  0x02,
	ARCHIVE_COMPRESSED = 0x04,
	ARCHIVE_BIGENDIAN =  0x40,
	ARCHIVE_HASFILENAME = 0x100,
};

class bzString {
public:
	bzString(const std::string &str) : m_len(0), m_str(NULL) {
		set(str);
	}
	bzString(IDataStream &dataStream) : m_len(0), m_str(NULL) {
		read(dataStream);
	}
	~bzString() {
		if (m_str != NULL) {
			delete[] m_str;
		}
	}

	void read(IDataStream &dataStream) {
		if (m_str != NULL) {
			delete[] m_str;
			m_str = NULL;
			m_len = 0;
		}
		m_len = dataStream.Read8();
		m_str = new char[m_len + 1];
		dataStream.ReadBuf(m_str, m_len);
		m_str[m_len] = 0;
	}

	void set(const std::string &str) {
		if (m_str != NULL) {
			delete[] m_str;
			m_str = NULL;
			m_len = 0;
		}
		m_len = str.length() + 1;
		m_str = new char[m_len];
		strcpy_s(m_str, m_len, str.data());
	}

	const char *get() const { return m_str; }

	int getSize() { return 1 + m_len; }
	int len() { return m_len; }
private:
	UInt8 m_len;
	char *m_str;
};

int FOResourceManager::m_archiveID = 0;
FOResourceManager FOResourceManager::m_instance;

FOResourceManager::FOResourceManager()
{
}


FOResourceManager::~FOResourceManager()
{
}

int FOResourceManager::getOrAddArchiveID(const std::string &archive)
{
	for (map<int, std::string>::const_iterator it = m_archives.cbegin();
		it != m_archives.cend(); ++it) {
		if (it->second.compare(archive) == 0) {
			return it->first;
		}
	}
	m_archives[m_archiveID++] = archive;
	return m_archiveID - 1;
}

void FOResourceManager::load(std::string path)
{
	IDirectoryIterator archiveIterator(path.c_str(), "*.bsa");
	while (!archiveIterator.Done()) {
		std::string archive = archiveIterator.GetFullPath();
		loadArchive(archive);
		archiveIterator.Next();
	}

	_MESSAGE("Loading data folder: %s", path.c_str());
	
	for (IDirectoryIterator dirIterator(path.c_str()); !dirIterator.Done(); dirIterator.Next())
	{
		WIN32_FIND_DATA *item = dirIterator.Get();
		if ((item->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 || 
			strcmp(item->cFileName, ".") == 0 ||
			strcmp(item->cFileName, "..") == 0) 
		{
			continue;
		}
		string dir = dirIterator.GetFullPath();
		size_t idx = dir.find(path);
		if (idx != string::npos) {
			dir = dir.substr(idx + path.length());
			if (dir.length() > 0 && dir[0] == '\\') {
				dir = dir.substr(1);
			}
		}
		loadFolder(path, dir);		
	}
}

void FOResourceManager::loadArchive(std::string archive)
{
	archive = normalizePath(archive);

	_MESSAGE("Loading archive file: %s", archive.c_str());
	IFileStream archiveStream(archive.c_str());
	if (archiveStream.GetHandle() == NULL)
	{
		_WARNING("Could not open archive file");
		return;
	}
	// Read header
	UInt32 fileID = archiveStream.Read32();
	if (fileID != '\0ASB') {
		_WARNING("Invalid archive");
		return;
	}
	UInt32 version = archiveStream.Read32();
	UInt32 folderOffset = archiveStream.Read32();
	UInt32 archiveFlags = archiveStream.Read32();
	UInt32 folderCount = archiveStream.Read32();
	UInt32 fileCount = archiveStream.Read32();
	UInt32 totalFolderNameLength = archiveStream.Read32();
	UInt32 totalFileNameLength = archiveStream.Read32();
	UInt32 fileFlags = archiveStream.Read32();

	bool defaultCompressed = (archiveFlags & (UInt32)ArchiveFlags::ARCHIVE_COMPRESSED) != 0;
	int archiveID = getOrAddArchiveID(archive);

	// Read folders
	archiveStream.SetOffset(folderOffset);
	for (int i = 0; i < folderCount; i++) {
		UInt64 folderHash = archiveStream.Read64();
		UInt32 fileCount = archiveStream.Read32();
		UInt32 fileRecordBlockOffset = archiveStream.Read32();

		SInt64 curPos = archiveStream.GetOffset();
		archiveStream.SetOffset(fileRecordBlockOffset - totalFileNameLength);

		// Jump to FileBlock
		// Read folder path
		bzString folderName(archiveStream);

		FOResourceFolder *folder;
		if (m_folders.find(folderHash) == m_folders.end()) {
			folder = new FOResourceFolder(folderHash);
			folder->setPath(folderName.get());
			m_folders[folderHash] = folder;
		}
		else {
			folder = m_folders.at(folderHash);
		}


		// Read fileBlock
		for (int j = 0; j < fileCount; j++) {
			UInt64 fileHash = archiveStream.Read64();
			UInt32 fileSize = archiveStream.Read32();
			UInt32 fileOffset = archiveStream.Read32();
			bool notDefault = (fileSize & (1 << 30)) != 0;
			fileSize = fileSize & 0x3FFFFFFF;
			bool compressed = defaultCompressed ^ notDefault;

			SInt64 curPos = archiveStream.GetOffset();
			archiveStream.SetOffset(fileOffset);
			std::string name;
			if ((archiveFlags & (UInt32)ArchiveFlags::ARCHIVE_HASFILENAME) != 0) {
				bzString filePath(archiveStream);
				name = filePath.get();
				fileOffset += filePath.getSize();
				if (compressed)
					fileSize -= filePath.getSize();
			}

			FOResourceFile *file;
			if (compressed) {
				UInt32 originalSize = archiveStream.Read32();
				file = new FOResourceFile(fileHash, archiveID, fileOffset + 4,
					fileSize - 4, originalSize, true);
			}
			else {
				file = new FOResourceFile(fileHash, archiveID, fileOffset, fileSize);
			}
			file->setPath(name);
			folder->setFile(fileHash, file);

			archiveStream.SetOffset(curPos);
		}
		archiveStream.SetOffset(curPos);
	}
}


void FOResourceManager::loadFolder(std::string base, std::string path)
{
	uint64_t folderHash = hash(path, "");
	FOResourceFolder *folder;
	if (m_folders.find(folderHash) == m_folders.end()) {
		folder = new FOResourceFolder(folderHash);
		folder->setPath(path.c_str());
		m_folders[folderHash] = folder;
	}
	else {
		folder = m_folders.at(folderHash);
	}
	for (IDirectoryIterator fileIterator((base + '\\' + path).c_str()); !fileIterator.Done();
		fileIterator.Next()) {
		WIN32_FIND_DATA *item = fileIterator.Get();
		if (strcmp(item->cFileName, ".") == 0 || strcmp(item->cFileName, "..") == 0) {
			continue;
		}
		if ((item->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			loadFolder(base, path + '\\' + item->cFileName);
			continue;
		}
		uint64_t fileHash = hash("", item->cFileName);
		folder->setFile(fileHash, new FOResourceFile(fileHash, 
			base + '\\' + path + '\\' + item->cFileName, item->nFileSizeLow));		
	}
}

const FOResourceFolder * FOResourceManager::getFolder(std::string path) const
{
	path = normalizePath(path);
	uint64_t pathHash = hash(path, "");
	return getFolder(pathHash);
}

const FOResourceFolder * FOResourceManager::getFolder(uint64_t hash) const
{
	map<uint64_t, FOResourceFolder *>::const_iterator it = m_folders.find(hash);
	if (it == m_folders.end()) {
		return NULL;
	}
	else {
		return it->second;
	}
}

const FOResourceFile * FOResourceManager::getFile(std::string path) const
{
	path = normalizePath(path);
	
	size_t fileOffset = path.find_last_of('\\');
	if (fileOffset == string::npos) {
		return getFile("", path);
	}
	else {
		string fileName = path.substr(fileOffset + 1);
		path = path.substr(0, fileOffset);
		return getFile(path, fileName);
	}
}

const FOResourceFile * FOResourceManager::getFile(std::string path, 
	std::string fileName) const
{
	path = normalizePath(path);
	fileName = normalizePath(fileName);
	uint64_t pathHash = hash(path, "");
	uint64_t fileHash = hash("", fileName);
	return getFile(pathHash, fileHash);
}

const FOResourceFile * FOResourceManager::getFile(uint64_t pathHash, uint64_t fileHash) const
{
	const FOResourceFolder *folder = getFolder(pathHash);
	if (folder == NULL) {
		return NULL;
	}
	return folder->getFile(fileHash);
}

const std::string FOResourceManager::getArchive(int id)
{
	map<int, string>::const_iterator it = m_archives.find(id);
	if (it == m_archives.end()) {
		return "";
	}
	else {
		return it->second;
	}
}

std::string FOResourceManager::normalizePath(std::string path)
{
	transform(path.begin(), path.end(), path.begin(), ::tolower);
	replace(path.begin(), path.end(), '/', '\\');
	if (path.length() > 0 && path[0] == '\\') {
		path = path.substr(1);
	}
	return path;
}

uint32_t FOResourceManager::hashStr(std::string str)
{
	uint32_t hash = 0;

	for (size_t i = 0; i < str.length(); i++) {
		hash *= 0x1003F;
		hash += (uint8_t)str[i];
	}

	return hash;
}

uint64_t FOResourceManager::hashPair(std::string file, std::string ext)
{
	uint64_t hash = 0;

	if (file.length() > 0) {
		hash = (uint64_t)((((uint8_t)file[file.length() - 1]) * 0x1) +
			((file.length() > 2 ? (uint8_t)file[file.length() - 2] : (uint8_t)0) * 0x100) +
			(file.length() * 0x10000) +
			(((uint8_t)file[0]) * 0x1000000));

		if (file.length() > 3) {
			hash += (uint64_t)(hashStr(file.substr(1, file.length() - 3)) * 0x100000000);
		}
	}

	if (ext.length() > 0) {
		hash += (uint64_t)(hashStr(ext) * 0x100000000LL);

		uint8_t i = 0;
		if (ext == ".nif") i = 1;
		else if (ext == ".kf") i = 2;
		else if (ext == ".dds") i = 3;
		else if (ext == ".wav") i = 4;

		if (i != 0) {
			uint8_t a = (uint8_t)(((i & 0xfc) << 5) + (uint8_t)((hash & 0xff000000) >> 24));
			uint8_t b = (uint8_t)(((i & 0xfe) << 6) + (uint8_t)((hash & 0x000000ff)));
			uint8_t c = (uint8_t)((i << 7) + (uint8_t)((hash & 0x0000ff00) >> 8));

			hash -= hash & 0xFF00FFFF;
			hash += (uint32_t)((a << 24) + b + (c << 8));
		}
	}
	return hash;
}

uint64_t FOResourceManager::hash(std::string path, std::string filename)
{
	filename = normalizePath(filename);
	path = normalizePath(path);

	std::string ext;
	std::string name;

	// Get extension
	size_t extIndex = filename.find_last_of('.');
	if (extIndex != std::string::npos) {
		ext = filename.substr(extIndex);
		name = filename.substr(0, extIndex);
	}
	else {
		ext = "";
		name = filename;
	}

	if (path.length() > 0 && name.length() > 0) {
		return hashPair(path + '\\' + name, ext);
	}
	else {
		return hashPair(path + name, ext);
	}
}
