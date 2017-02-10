#ifndef STORAGEENGINE_H
#define STORAGEENGINE_H

#include "changecontroller.h"
#include "datamerger.h"
#include "localstore.h"
#include "remoteconnector.h"
#include "stateholder.h"

#include <QFuture>
#include <QJsonSerializer>
#include <QObject>

namespace QtDataSync {

class StorageEngine : public QObject
{
	Q_OBJECT
	friend class Setup;

public:
	enum TaskType {
		Count,
		Keys,
		LoadAll,
		Load,
		Save,
		Remove
	};
	Q_ENUM(TaskType)

	explicit StorageEngine(QJsonSerializer *serializer,
						   LocalStore *localStore,
						   StateHolder *stateHolder,
						   RemoteConnector *remoteConnector,
						   DataMerger *dataMerger);

public slots:
	void beginTask(QFutureInterface<QVariant> futureInterface, QtDataSync::StorageEngine::TaskType taskType, int metaTypeId, const QVariant &value = {});

private slots:
	void initialize();
	void finalize();

	void requestCompleted(quint64 id, const QJsonValue &result);
	void requestFailed(quint64 id, const QString &errorString);
	void operationDone(const QJsonValue &result);
	void operationFailed(const QString &errorString);

	void loadLocalStatus();
	void beginRemoteOperation(const ChangeController::ChangeOperation &operation);
	void beginLocalOperation(const ChangeController::ChangeOperation &operation);

private:
	struct RequestInfo {
		//change controller
		bool isChangeControllerRequest;

		//store requests
		QFutureInterface<QVariant> futureInterface;
		int convertMetaTypeId;

		//changing operations
		bool changeAction;
		ObjectKey changeKey;
		StateHolder::ChangeState changeState;

		RequestInfo(bool isChangeControllerRequest = false);
		RequestInfo(QFutureInterface<QVariant> futureInterface, int convertMetaTypeId = QMetaType::UnknownType);
	};

	QJsonSerializer *serializer;
	LocalStore *localStore;
	StateHolder *stateHolder;
	RemoteConnector *remoteConnector;
	ChangeController *changeController;

	QHash<quint64, RequestInfo> requestCache;
	quint64 requestCounter;

	void count(QFutureInterface<QVariant> futureInterface, int metaTypeId);
	void keys(QFutureInterface<QVariant> futureInterface, int metaTypeId);
	void loadAll(QFutureInterface<QVariant> futureInterface, int dataMetaTypeId, int listMetaTypeId);
	void load(QFutureInterface<QVariant> futureInterface, int metaTypeId, const QByteArray &keyProperty, const QString &value);
	void save(QFutureInterface<QVariant> futureInterface, int metaTypeId, const QByteArray &keyProperty, QVariant value);
	void remove(QFutureInterface<QVariant> futureInterface, int metaTypeId, const QByteArray &keyProperty, const QString &value);
};

}

#endif // STORAGEENGINE_H
