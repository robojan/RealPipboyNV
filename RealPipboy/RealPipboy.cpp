#include "RealPipboy.h"

#include "common/IPrefix.h"

#include "Communication/CommunicationManager.h"
#include "Packets/packets.h"

#include <chrono>

RealPipboy::RealPipboy(IDataManager *dataManager) :
	m_connected(false), m_dataManager(dataManager), m_keepAliveID(-1), m_inGame(false),
	m_transferringData(false), m_lastKeepAliveReceived(0)
{
}

RealPipboy::~RealPipboy()
{
}

void RealPipboy::init()
{
	CommunicationManager &comm = CommunicationManager::getInstance();
	comm.registerPacketHandler(MessageType::HELLO, this);
}

void RealPipboy::update(void)
{
	CommunicationManager &comm = CommunicationManager::getInstance();
	if (comm.isClientConnected() != m_connected) {
		m_connected = comm.isClientConnected();
		enableDataTransfer(m_inGame && m_connected);
		if (m_connected) {
			_MESSAGE("Client connected");
			m_keepAliveID = m_updateScheduler.addPeriodic(RealPipboy::sendKeepAlive, 3000, this);
			m_updateScheduler.start();
		}
		else {
			_MESSAGE("Client disconnected");
			if (m_keepAliveID > 0) {
				m_updateScheduler.removeTask(m_keepAliveID);
			}
		}
	}
	comm.update();
	m_dataManager->update();
	Sleep(16);
}

void RealPipboy::makeConnectable(bool connectable)
{
	if (connectable) {
		CommunicationManager::getInstance().startListening();
	}
	else {
		CommunicationManager::getInstance().stopListening();
		m_dataManager->deregisterUpdates(m_updateScheduler);
		m_updateScheduler.stop();
	}
}

void RealPipboy::setInGame(bool inGame)
{
	m_inGame = inGame;
	_MESSAGE("SetInGame: %s", inGame ? "True" : "False");
	CommunicationManager &comm = CommunicationManager::getInstance();
	enableDataTransfer(m_inGame && comm.isClientConnected());
}

IDataManager * RealPipboy::getDataManager() const
{
	return m_dataManager;
}

void RealPipboy::enableDataTransfer(bool enable)
{
	if (enable == m_transferringData) {
		return;
	}
	m_transferringData = enable;
	if (enable) {
		m_dataManager->registerUpdates(m_updateScheduler);
	}
	else {
		m_dataManager->deregisterUpdates(m_updateScheduler);
	}
	m_updateScheduler.start();
}

void RealPipboy::sendKeepAlive(void *user)
{
	RealPipboy *pipboy = (RealPipboy *)(user);
	CommunicationManager &comm = CommunicationManager::getInstance();
	if (comm.isClientConnected()) {
		comm.sendPacket(new HelloPacket(0x100, pipboy->m_dataManager->getSystemName()));
	}
}

void RealPipboy::handlePacket(DataPacket *packet)
{
	switch (packet->getType()) {
	case MessageType::HELLO:
		m_lastKeepAliveReceived = std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count();
		break;
	}
}