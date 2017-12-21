HEADERS += \
	$$PWD/message_p.h \
	$$PWD/identifymessage_p.h \
	$$PWD/registermessage_p.h \
	$$PWD/asymmetriccrypto_p.h \
	$$PWD/accountmessage_p.h \
	$$PWD/loginmessage_p.h \
	$$PWD/welcomemessage_p.h \
    $$PWD/errormessage_p.h \
    $$PWD/syncmessage_p.h \
    $$PWD/changemessage_p.h

SOURCES += \
	$$PWD/message.cpp \
	$$PWD/identifymessage.cpp \
	$$PWD/registermessage.cpp \
	$$PWD/asymmetriccrypto.cpp \
	$$PWD/accountmessage.cpp \
	$$PWD/loginmessage.cpp \
	$$PWD/welcomemessage.cpp \
    $$PWD/errormessage.cpp \
    $$PWD/syncmessage.cpp \
    $$PWD/changemessage.cpp

INCLUDEPATH += $$PWD

DEFINES += "DS_PROTO_VERSION=\\\"$$MODULE_VERSION\\\""
