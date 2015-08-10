#pragma once

#include <string>
#include <atomic>
#include "Scheduler.h"
#include "DataTypes/IDataManager.h"
#include "Communication/CommunicationManager.h"

class RealPipboy : private IPacketHandler
{
public:
	RealPipboy(IDataManager *data, void (*showMessage)(bool error, const char *msg));
	~RealPipboy();

	void init();

	void setTCPSettings(const char *hostname, int port);

	void update(void);
	void makeConnectable(bool connectable);
	void setInGame(bool inGame);
	
	IDataManager *getDataManager() const;
private:
	void enableDataTransfer(bool enable);
	static void sendKeepAlive(void *user);
	virtual void handlePacket(DataPacket *packet) override;

	Scheduler m_updateScheduler;
	bool m_connected;	
	IDataManager *m_dataManager;
	int m_keepAliveID;
	std::atomic<bool> m_inGame;
	bool m_transferringData;

	long long m_lastKeepAliveReceived;
};

