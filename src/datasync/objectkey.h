#ifndef OBJECTKEY_H
#define OBJECTKEY_H

#include <QtCore/qmetatype.h>

#include "QtDataSync/qtdatasync_global.h"

namespace QtDataSync {

//! The default setup name
extern Q_DATASYNC_EXPORT const QString DefaultSetup;

struct Q_DATASYNC_EXPORT ObjectKey
{
	QByteArray typeName;
	QString id;

	ObjectKey(const QByteArray &typeName = {}, const QString &id = {});

	bool operator==(const ObjectKey &other) const;
	bool operator!=(const ObjectKey &other) const;
};

uint Q_DATASYNC_EXPORT qHash(const ObjectKey &key, uint seed = 0);
QDataStream Q_DATASYNC_EXPORT &operator<<(QDataStream &out, const ObjectKey &key);
QDataStream Q_DATASYNC_EXPORT &operator>>(QDataStream &in, ObjectKey &key);
QDebug Q_DATASYNC_EXPORT operator<<(QDebug debug, const ObjectKey &key);

}

Q_DECLARE_METATYPE(QtDataSync::ObjectKey)

#endif // OBJECTKEY_H