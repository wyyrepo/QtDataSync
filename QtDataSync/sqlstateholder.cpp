#include "defaultsqlkeeper.h"
#include "sqlstateholder.h"

#include <QDebug>

#include <QtSql>

using namespace QtDataSync;

SqlStateHolder::SqlStateHolder(QObject *parent) :
	StateHolder(parent)
{}

void SqlStateHolder::initialize()
{
	database = DefaultSqlKeeper::aquireDatabase();

	//create table
	if(!database.tables().contains(QStringLiteral("SyncState"))) {
		QSqlQuery createQuery(database);
		createQuery.prepare(QStringLiteral("CREATE TABLE SyncState ("
												"Type		TEXT NOT NULL,"
												"Key		TEXT NOT NULL,"
												"Changed	INTEGER NOT NULL CHECK(Changed >= 0 AND Changed <= 2),"
												"PRIMARY KEY(Type, Key)"
										   ");"));
		if(!createQuery.exec()) {
			qCritical() << "Failed to create SyncState table with error:"
						<< createQuery.lastError().text();
		}
	}
}

void SqlStateHolder::finalize()
{
	database = QSqlDatabase();
	DefaultSqlKeeper::releaseDatabase();
}

QList<StateHolder::ChangedInfo> SqlStateHolder::listLocalChanges()
{
	QSqlQuery listQuery(database);
	listQuery.prepare(QStringLiteral("SELECT Type, Key, Changed FROM SyncState WHERE Changed != 0"));
	if(!listQuery.exec()) {
		qCritical() << "Failed to load current state with error:"
					<< listQuery.lastError().text();
		return {};
	}

	QList<ChangedInfo> stateList;
	while(listQuery.next()) {
		ChangedInfo info;
		info.typeName = listQuery.value(0).toByteArray();
		info.key = listQuery.value(1).toString();
		info.state = (ChangeState)listQuery.value(2).toInt();
		stateList.append(info);
	}

	return stateList;
}

void SqlStateHolder::markLocalChanged(const QByteArray &typeName, const QString &key, ChangeState changed)
{
	QSqlQuery updateQuery(database);

	if(changed == Unchanged) {
		updateQuery.prepare(QStringLiteral("DELETE FROM SyncState WHERE Type = ? AND Key = ?"));
		updateQuery.addBindValue(typeName);
		updateQuery.addBindValue(key);
	} else {
		updateQuery.prepare(QStringLiteral("INSERT OR REPLACE INTO SyncState (Type, Key, Changed) VALUES(?, ?, ?)"));
		updateQuery.addBindValue(typeName);
		updateQuery.addBindValue(key);
		updateQuery.addBindValue((int)changed);
	}

	if(!updateQuery.exec()) {
		qCritical() << "Failed to update current state for type"
					<< typeName
					<< "and key"
					<< key
					<< "with error:"
					<< updateQuery.lastError().text();
	}
}

void SqlStateHolder::markAllLocalChanged(const QByteArray &typeName, StateHolder::ChangeState changed)
{
	QSqlQuery updateQuery(database);

	if(changed == Unchanged) {
		updateQuery.prepare(QStringLiteral("DELETE FROM SyncState WHERE Type = ?"));
		updateQuery.addBindValue(typeName);
	} else {
		updateQuery.prepare(QStringLiteral("UPDATE SyncState SET Changed = ? WHERE Type = ?"));
		updateQuery.addBindValue((int)changed);
		updateQuery.addBindValue(typeName);
	}

	if(!updateQuery.exec()) {
		qCritical() << "Failed to update current state for all of type"
					<< typeName
					<< "with error:"
					<< updateQuery.lastError().text();
	}
}