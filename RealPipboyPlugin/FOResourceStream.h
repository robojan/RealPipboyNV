#pragma once

#include "common/IPrefix.h"
#include "common/IFileStream.h"

class FOResourceFile;

class FOResourceStream : public IDataStream
{
public:
	FOResourceStream();
	FOResourceStream(const FOResourceFile *file);
	FOResourceStream(const char *path);
	~FOResourceStream();

	bool open(const FOResourceFile *file);
	bool open(const char *path);
	void close();

	inline bool isOpen() { return m_open; }

	virtual void SetOffset(SInt64 inOffset);

	virtual void ReadBuf(void *buf, UInt32 inLength);
	// Read only
	virtual void WriteBuf(const void * buf, UInt32 inLength);

protected:
	bool m_open;
	const FOResourceFile *m_fileInfo;
	IDataSubStream m_rawStream;
	struct z_stream_s *m_zstream;
	UInt32 *m_inAvailable;
	UInt8 *m_inBuffer;
	UInt8 *m_outBuffer;
	UInt32 *m_outAvailable;


private:
	bool openArchived();
	bool openNormal();

	void fillInBuffer();
};
