#include "FileTransferHandler.h"
#include "RealPipboy/Communication/CommunicationManager.h"
#include "FOResourceManager.h"
#include "FOResourceStream.h"

#include "squish-1.11/squish.h"
#include <png.h>

#define PACKET_DATA_SIZE (48*1024)

using namespace std;

FileTransferHandler::FileTransferHandler() : m_fileTransferRunning(false), m_id(0)
{
	CommunicationManager::getInstance().registerPacketHandler(MessageType::GETFILE, this);
}

FileTransferHandler::~FileTransferHandler()
{
	CommunicationManager::getInstance().deregisterPacketHandler(MessageType::GETFILE, this);
}

void FileTransferHandler::handlePacket(DataPacket *packet)
{
	if (packet->getType() != MessageType::GETFILE) {
		return;
	}
	GetFilePacket *gfPacket = dynamic_cast<GetFilePacket *>(packet);
	addFileForTransfer(gfPacket->getFileName(), gfPacket->getID(), gfPacket->getFlags());
}

void FileTransferHandler::fileTransferThreadFunc(FileTransferHandler *fth)
{
	uint8_t *fileBuffer = NULL;
	uint32_t bufLen = 0;
	while (fth->m_fileTransferRunning) {
		struct fileTransferInfo m_currentTransfer;
		m_currentTransfer.fileName = "";
		// Get file name
		fth->m_fileTransferMutex.lock();
		if (fth->m_fileTransferList.size() > 0) {
			m_currentTransfer = fth->m_fileTransferList.front();
			fth->m_fileTransferList.pop_front();
		}
		fth->m_fileTransferMutex.unlock();
		if (m_currentTransfer.fileName.empty()) {
			fth->m_fileTransferRunning = false;
			continue;
		}
		// Get File 
		FOResourceStream fileStream(m_currentTransfer.fileName.c_str());
		if (!fileStream.isOpen()) {
			_ERROR("Could not transmit file \"%s\" to the client: Could not open the file", 
				m_currentTransfer.fileName.c_str());
			fth->transmitFileFailed(m_currentTransfer);
			continue;
		}

		// Read the file
		fileBuffer = new uint8_t[fileStream.GetLength()];
		bufLen = fileStream.GetLength();
		fileStream.ReadBuf(fileBuffer, fileStream.GetLength());

		// Do file conversion
		if ((m_currentTransfer.flags & GETFILEFLAG_NOCONVERSION) == 0) {
			convertFile(m_currentTransfer.fileName, &fileBuffer, &bufLen);
		}

		// transmit the file
		fth->transmitFile(m_currentTransfer, fileBuffer, bufLen);

		// Free the buffer
		delete[] fileBuffer;
		fileBuffer = NULL;
		bufLen = 0;
	}
}

void FileTransferHandler::addFileForTransfer(const std::string &file, int8_t id, int8_t flags)
{
	m_fileTransferMutex.lock();
	struct fileTransferInfo info;
	info.fileName = file;
	info.id = id;
	info.flags = flags;
	m_fileTransferList.push_back(info);
	if (m_fileTransferRunning == false) {
		m_fileTransferRunning = true;
		thread workThread(FileTransferHandler::fileTransferThreadFunc, this);
		workThread.detach();
	}
	m_fileTransferMutex.unlock();
}

void FileTransferHandler::transmitFileFailed(struct fileTransferInfo &info)
{
	CommunicationManager::getInstance().sendPacket(new SetFilePacket(info.fileName, info.id));
}

void FileTransferHandler::transmitFile(struct fileTransferInfo &info, const uint8_t *buffer, unsigned int len)
{
	
	int numberOfPackets = (len+PACKET_DATA_SIZE-1) / PACKET_DATA_SIZE;

	CommunicationManager &comm = CommunicationManager::getInstance();
	comm.sendPacket(new SetFilePacket(info.fileName, info.id, 
		len, numberOfPackets));
	
	for (int i = 0; i < numberOfPackets; i++) {
		uint32_t offset = i * PACKET_DATA_SIZE;
		uint32_t dataSize = len - offset;
		if (dataSize > PACKET_DATA_SIZE) {
			dataSize = PACKET_DATA_SIZE;
		}

		comm.sendPacket(new SendFileDataPacket(info.id, offset, i, buffer + offset, dataSize));
	}

}

void FileTransferHandler::convertFile(std::string &filename, uint8_t **buffer, unsigned int *bufLen)
{
	// Get file extension
	size_t extPos = filename.find_last_of('.');
	if (extPos == string::npos) {
		return;
	}
	string ext = filename.substr(extPos + 1);
	if (ext == "dds") {
		if (convertDDSToPNG(buffer, bufLen)) {
			filename.append(".png");
		}

	}
}

typedef struct {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
} DDS_PIXELFORMAT;

typedef struct {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwPitchOrLinearSize;
	DWORD dwDepth;
	DWORD dwMipMapCount;
	DWORD dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD dwCaps;
	DWORD dwCaps2;
	DWORD dwCaps3;
	DWORD dwCaps4;
	DWORD dwReserved2;
} DDS_HEADER;

bool createPNGFile(uint8_t *rgba, int width, int height);

bool FileTransferHandler::convertDDSToPNG(uint8_t **buffer, unsigned int *bufLen)
{
	uint8_t *input = *buffer;
	if (*bufLen <= (4 + sizeof(DDS_HEADER))) {
		_ERROR("Tried to read an DDS file which is to small to contain an DDS image");
		return false;
	}
	DWORD magic = *((DWORD *)(input + 0));
	if (magic != ' SDD') {
		_ERROR("Tried to read invalid DDS file");
		return false;
	}
	DDS_HEADER *header = (DDS_HEADER *)(input + 4);
	input += 4 + sizeof(DDS_HEADER);
	if ((header->ddspf.dwFlags & 0x4) != 0 && header->ddspf.dwFourCC == '01XD') {
		_ERROR("DXT10 file not supported");
		return false;
	}
	uint8_t *decompressed;
	bool decompressedMustBeFreed = false;
	size_t decompressedSize = header->dwHeight * header->dwWidth * 4;
	if ((header->ddspf.dwFlags & 0x4) == 0) {
		// Uncompressed
		decompressed = input;
	}
	else {
		int decompressFlags = 0;
		if (header->ddspf.dwFourCC == '1TXD') {
			decompressFlags |= squish::kDxt1;
		}
		else if (header->ddspf.dwFourCC == '2TXD' || header->ddspf.dwFourCC == '3TXD') {
			decompressFlags |= squish::kDxt3;
		}
		else if(header->ddspf.dwFourCC == '4TXD' || header->ddspf.dwFourCC == '5TXD') {
			decompressFlags |= squish::kDxt5;
		}
		else {
			_ERROR("Unknown compression method in DDS file");
			return false;
		}
		decompressed = new uint8_t[decompressedSize];
		decompressedMustBeFreed = true;
		squish::DecompressImage(decompressed, header->dwWidth, header->dwHeight, 
			input, decompressFlags);
	}

	// Now we got the decompressed image, recompress it into PNG
	std::vector<uint8_t> pngBuffer;
	bool ret = createPNGFile(decompressed, header->dwWidth, header->dwHeight, &pngBuffer);

	if (decompressedMustBeFreed) {
		delete[] decompressed;
		decompressed = NULL;
	}
		
	if (!ret) {
		_ERROR("Could not create PNG file");
		return false;
	}

	// replace dds file with png file
	delete[] *buffer;
	*buffer = new uint8_t[pngBuffer.size()];
	*bufLen = pngBuffer.size();
	memcpy(*buffer, pngBuffer.data(), pngBuffer.size());

	return true;
}

void pngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t len) {
	std::vector<uint8_t> *out = (std::vector<uint8_t> *)png_get_io_ptr(png_ptr);
	out->insert(out->end(), data, data + len);
}

bool FileTransferHandler::createPNGFile(uint8_t *rgba, int width, int height, std::vector<uint8_t> *out) {
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	bool result = NULL;

	out->clear();

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		_ERROR("Could not allocate png write struct");
		goto finalise;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		_ERROR("Could not allocate png info struct");
		goto finalise;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		_ERROR("Error during png creation");
		goto finalise;
	}

	png_set_write_fn(png_ptr, out, pngWriteCallback, NULL);

	// Write header
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);

	// write image data
	for (int y = 0; y < height; y++) {
		png_write_row(png_ptr, rgba + width * y * 4);
	}

	// end write
	png_write_end(png_ptr, NULL);
	result = true;

finalise:
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	return result;
}
