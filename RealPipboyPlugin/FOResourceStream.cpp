#include "FOResourceStream.h"

#include "FOResourceManager.h"

#include "zlib.h"

using namespace std;

#define CHUNK (32 * 1024)

FOResourceStream::FOResourceStream() :
	m_fileInfo(NULL), m_open(false), m_zstream(NULL), m_inBuffer(NULL), m_outBuffer(NULL),
	m_inAvailable(0), m_outAvailable(0)
{

}

FOResourceStream::FOResourceStream(const FOResourceFile *file) :
	m_fileInfo(NULL), m_open(false), m_zstream(NULL), m_inBuffer(NULL), m_outBuffer(NULL),
	m_inAvailable(0), m_outAvailable(0)

{
	open(file);
}

FOResourceStream::FOResourceStream(const char *path) :
	m_fileInfo(NULL), m_open(false), m_zstream(NULL), m_inBuffer(NULL), m_outBuffer(NULL),
	m_inAvailable(0), m_outAvailable(0)
{
	open(path);
}

FOResourceStream::~FOResourceStream()
{
	close();
}

bool FOResourceStream::open(const FOResourceFile *file)
{
	close();
	if (file == NULL)
		return false;
	m_fileInfo = file;
	streamLength = 0;
	streamOffset = 0;

	if (m_fileInfo->isInArchive()) {
		return openArchived();
	}
	else {
		return openNormal();
	}
}

bool FOResourceStream::open(const char *path)
{
	return open(FOResourceManager::getSingleton()->getFile(path));
}

void FOResourceStream::close()
{
	if (!m_open)
		return;
	IDataStream *parentStream = m_rawStream.GetParent();
	if (parentStream != NULL) {
		delete parentStream;
	}
	m_rawStream = IDataSubStream();

	if (m_zstream != NULL) {
		inflateEnd(m_zstream);
		delete m_zstream;
		m_zstream = NULL;
	}
	if (m_inBuffer != NULL) {
		delete[] m_inBuffer;
		m_inBuffer = NULL;
		m_inAvailable = 0;
	}
	if (m_outBuffer != NULL) {
		delete[] m_outBuffer;
		m_outBuffer = NULL;
		m_outAvailable = 0;
	}
}

bool FOResourceStream::openArchived()
{
	FOResourceManager *manager = FOResourceManager::getSingleton();
	std::string archivePath = manager->getArchive(m_fileInfo->getArchive());
	if (archivePath.empty())
		return false;
	IFileStream *archiveStream = new IFileStream(archivePath.c_str());
	if (archiveStream->GetHandle() == NULL) {
		delete archiveStream;
		return false;
	}
	m_rawStream.Attach(archiveStream, m_fileInfo->getOffset(), m_fileInfo->getRawFileSize());
	if (m_fileInfo->isCompressed()) {
		// Create buffers
		ASSERT(m_inBuffer == NULL && m_outBuffer == NULL);
		m_inBuffer = new UInt8[CHUNK];
		m_outBuffer = new UInt8[CHUNK];

		// Create zlib state
		m_zstream = new z_stream;
		m_zstream->next_in = m_inBuffer;
		m_zstream->avail_in = 0;
		m_zstream->zalloc = Z_NULL;
		m_zstream->zfree = Z_NULL;
		m_zstream->opaque = Z_NULL;
		m_zstream->next_out = m_outBuffer;
		m_zstream->avail_out = CHUNK;

		// Fill the input buffers
		fillInBuffer();
		
		// Init zlib
		int result = inflateInit(m_zstream);
		if (result != Z_OK) {
			close();
			_ERROR("Could not init ZLIB inflate: %s(%d)", m_zstream->msg, result);
			return false;
		}


	}
	else {

	}
	streamLength = m_fileInfo->getFileSize();
	streamOffset = 0;
	m_open = true;
	return true;
}

bool FOResourceStream::openNormal()
{
	if (m_fileInfo->getPath().empty())
		return false;
	IFileStream *fileStream = new IFileStream(m_fileInfo->getPath().c_str());
	if (fileStream->GetHandle() == NULL) {
		delete fileStream;
		return false;
	}
	m_rawStream.Attach(fileStream, 0, fileStream->GetLength());
	streamLength = fileStream->GetLength();
	streamOffset = 0;
	m_open = true;
	return true;
}

void FOResourceStream::fillInBuffer()
{
	ASSERT_STR(m_inBuffer != NULL, "FOResourceStream::fillInBuffer: m_inBuffer == NULL");
	ASSERT_STR(m_zstream != NULL, "FOResourceStream::fillInBuffer: m_zstream == NULL");
	if (m_zstream->next_in == Z_NULL) {
		m_zstream->next_in = m_inBuffer;
	}
	UInt8 *readPtr = m_inBuffer;
	UInt32 readSize = CHUNK;
	if (m_zstream->avail_in > 0) {
		memmove(m_inBuffer, m_zstream->next_in, m_zstream->avail_in);
		m_zstream->next_in = m_inBuffer;
		readPtr += m_zstream->avail_in;
		readSize -= m_zstream->avail_in;
	}

	if (readSize > m_rawStream.GetRemain()) {
		readSize = m_rawStream.GetRemain();
	}
	m_rawStream.ReadBuf(readPtr, readSize);
	m_zstream->avail_in += readSize;
	
}

void FOResourceStream::SetOffset(SInt64 inOffset)
{
	if (m_fileInfo->isCompressed()) {
		ASSERT_STR(false, "FOResourceStream::SetOffset: Not implemented in case of compression");
		if (streamOffset > inOffset) {

		}
		else {

		}
	}
	else {
		IDataStream::SetOffset(inOffset);
	}
}

void FOResourceStream::ReadBuf(void *buf, UInt32 inLength)
{
	if (m_fileInfo->isCompressed()) {
		ASSERT_STR(m_zstream != NULL, "FOResourceStream::ReadBuf: zlib not initialized");
		UInt32 read = 0;
		while (read < inLength) {
			UInt32 toRead = inLength - read;
			if (toRead > CHUNK - m_zstream->avail_out) {
				toRead = CHUNK - m_zstream->avail_out;
			}
			if (toRead == 0) {
				// Refill buffer
				fillInBuffer();
				m_zstream->next_out = m_outBuffer;
				m_zstream->avail_out = CHUNK;

				// Decompress the file
				int result = inflate(m_zstream, Z_NO_FLUSH);
				if (result != Z_OK) {
					// Some error
					switch (result) {
					case Z_STREAM_END:
						streamLength = streamOffset + (CHUNK - m_zstream->avail_out);
						break;
					case Z_STREAM_ERROR:
						_ERROR("Error in decompressing file", m_zstream->msg);
						return;
					case Z_BUF_ERROR:
						break;
					case Z_DATA_ERROR:
						_ERROR("ERROR in decompressing file: %s", m_zstream->msg);
						return;
					}
				}
				continue;
			}
			memcpy(buf, m_outBuffer, toRead);
			buf = ((UInt8 *)buf) + toRead;
			streamOffset += toRead;
			m_zstream->avail_out += toRead;
			memmove(m_outBuffer, m_zstream->next_out, CHUNK - m_zstream->avail_out);
			read += toRead;
		}
	}
	else {		
		m_rawStream.ReadBuf(buf, inLength);
		streamOffset += inLength;
	}
}

void FOResourceStream::WriteBuf(const void * buf, UInt32 inLength)
{
	// Read only
	(void)buf;
	(void)inLength;
}
