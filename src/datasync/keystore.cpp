#include "keystore.h"
#include "cryptocontroller_p.h"
using namespace QtDataSync;

namespace QtDataSync {
class Q_DATASYNC_EXPORT KeyStorePrivate
{
public:
	KeyStorePrivate(const Defaults &defaults);
	Defaults defaults;
};
}

KeyStore::KeyStore(const Defaults &defaults, QObject *parent) :
	QObject(parent),
	d(new KeyStorePrivate(defaults))
{}

KeyStore::~KeyStore() {}

QStringList KeyStore::listProviders()
{
	return CryptoController::allKeystoreKeys();
}

QString KeyStore::defaultProvider()
{
#ifdef Q_OS_LINUX
	auto kwallet = QStringLiteral("kwallet");
	if(CryptoController::allKeystoreKeys().contains(kwallet))
		return kwallet;
#endif
	return QStringLiteral("plain");
}

Defaults KeyStore::defaults() const
{
	return d->defaults;
}



KeyStorePrivate::KeyStorePrivate(const Defaults &defaults) :
	defaults(defaults)
{}



KeyStoreException::KeyStoreException(const KeyStore * const keyStore, const QString &what) :
	Exception(keyStore->defaults(), what),
	_keyStoreName(keyStore->providerName())
{}

KeyStoreException::KeyStoreException(const KeyStoreException * const other) :
	Exception(other),
	_keyStoreName(other->_keyStoreName)
{}

QString KeyStoreException::keyStoreName() const
{
	return _keyStoreName;
}

QByteArray KeyStoreException::className() const noexcept
{
	return QTDATASYNC_EXCEPTION_NAME(KeyStoreException);
}

QString KeyStoreException::qWhat() const
{
	return Exception::qWhat() +
			QStringLiteral("\n\tKeystore: %1")
			.arg(_keyStoreName);
}

void KeyStoreException::raise() const
{
	throw (*this);
}

QException *KeyStoreException::clone() const
{
	return new KeyStoreException(this);
}
