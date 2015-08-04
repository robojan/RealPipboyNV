#pragma once

#include "RealPipboy/Communication/CommunicationManager.h"
#include "RealPipboy/Packets/packets.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <deque>
#include <cstdint>

class FileTransferHandler : private IPacketHandler {
public:
	FileTransferHandler();
	~FileTransferHandler();

private:

	struct fileTransferInfo {
		std::string fileName;
		int8_t id;
		int8_t flags;
	};

	virtual void handlePacket(DataPacket *packet) override;
	static void fileTransferThreadFunc(FileTransferHandler *pipboy);
	void addFileForTransfer(const std::string &file, int8_t id, int8_t flags);
	void transmitFileFailed(struct fileTransferInfo &info);
	void transmitFile(struct fileTransferInfo &info, const uint8_t *buffer, unsigned int len);
	static void convertFile(std::string &filename, uint8_t **buffer, unsigned int *bufLen);
	static bool convertDDSToPNG(uint8_t **buffer, unsigned int *bufLen);
	static bool createPNGFile(uint8_t *rgba, int width, int height, std::vector<uint8_t> *out);


	std::mutex m_fileTransferMutex;
	std::deque<fileTransferInfo> m_fileTransferList;
	std::atomic<bool> m_fileTransferRunning;
	int8_t m_id;
};