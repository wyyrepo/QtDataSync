#include "threadedserver_p.h"
using namespace QtDataSync;

const QString ThreadedServer::UrlScheme(QStringLiteral("threaded"));
QMutex ThreadedServer::_lock;
QHash<QString, ThreadedServer*> ThreadedServer::_servers;

bool ThreadedServer::connectTo(const QUrl &url, ExchangeBuffer *clientBuffer)
{
	if(url.scheme() != UrlScheme || !url.isValid()) {
		qCCritical(rothreadedbackend).noquote() << "Unsupported URL-Scheme:" << url.scheme();
		return false;
	}

	QMutexLocker _(&_lock);
	auto server = _servers.value(url.path());
	if(!server) {
		qCWarning(rothreadedbackend).noquote() << "No threaded server found for URL:" << url.toString(); //TODO prevent spamming when unable to connect...
		return false;
	} else {
		return QMetaObject::invokeMethod(server, "addConnection", Qt::QueuedConnection,
										 Q_ARG(ExchangeBuffer*, clientBuffer));
	}
}

ThreadedServer::ThreadedServer(QObject *parent) :
	QConnectionAbstractServer(parent),
	_listenAddress(),
	_pending(),
	_lastError(QAbstractSocket::UnknownSocketError)
{}

ThreadedServer::~ThreadedServer()
{
	close();
}

bool ThreadedServer::hasPendingConnections() const
{
	return !_pending.isEmpty();
}

QUrl ThreadedServer::address() const
{
	return _listenAddress;
}

bool ThreadedServer::listen(const QUrl &address)
{
	if(_listenAddress.isValid()) {
		_lastError = QAbstractSocket::SocketAddressNotAvailableError;
		return false;
	}

	if(address.scheme() != UrlScheme || !address.isValid()) {
		_lastError = QAbstractSocket::SocketAddressNotAvailableError;
		return false;
	} else {
		QMutexLocker _(&_lock);
		if(_servers.contains(address.path())) {
			_lastError = QAbstractSocket::AddressInUseError;
			return false;
		} else {
			_servers.insert(address.path(), this);
			_listenAddress = address;
			return true;
		}
	}
}

QAbstractSocket::SocketError ThreadedServer::serverError() const
{
	return _lastError;
}

void ThreadedServer::close()
{
	if(_listenAddress.isValid()) {
		QMutexLocker _(&_lock);
		_servers.remove(_listenAddress.path());
		_listenAddress.clear();
		_pending.clear();
		_lastError = QAbstractSocket::UnknownSocketError;
	}
}

ServerIoDevice *ThreadedServer::configureNewConnection()
{
	if(!_listenAddress.isValid() || !hasPendingConnections())
		return nullptr;

	auto buffer = new ExchangeBuffer();
	if(!buffer->connectTo(_pending.dequeue())) {
		_lastError = QAbstractSocket::ConnectionRefusedError;
		buffer->deleteLater();
		return nullptr;
	}

	return new ThreadedServerIoDevice(buffer, this);
}

void ThreadedServer::addConnection(ExchangeBuffer *buffer)
{
	if(_listenAddress.isValid()) {
		_pending.enqueue(buffer);
		emit newConnection();
	}
}



ThreadedServerIoDevice::ThreadedServerIoDevice(ExchangeBuffer *buffer, QObject *parent) :
	ServerIoDevice(parent),
	_buffer(buffer)
{
	_buffer->setParent(this);
	connect(_buffer, &ExchangeBuffer::readyRead,
			this, &ThreadedServerIoDevice::readyRead);
	connect(_buffer, &ExchangeBuffer::partnerDisconnected,
			this, &ThreadedServerIoDevice::disconnected);
}

ThreadedServerIoDevice::~ThreadedServerIoDevice()
{
	_buffer->disconnect();
	if(_buffer->isOpen())
		_buffer->close();
}

QIODevice *ThreadedServerIoDevice::connection() const
{
	return _buffer;
}

void ThreadedServerIoDevice::doClose()
{
	if(_buffer->isOpen())
		_buffer->close();
}