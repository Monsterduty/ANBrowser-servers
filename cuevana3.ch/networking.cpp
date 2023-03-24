#include "qabstractsocket.h"
#include "qeventloop.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkreply.h"
#include "qnetworkrequest.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAbstractSocket>
#include <QEventLoop>
#include <QByteArray>
#include "networking.hpp"

QString download( const char *link, std::vector<std::pair<QByteArray, QByteArray>> headers ){
	QNetworkAccessManager manager;
	QNetworkRequest request;
	QNetworkReply *reply = nullptr;

	request.setUrl(QUrl(link));

	if ( headers.size() > 0 )
		for ( auto item : headers )
			request.setRawHeader(item.first, item.second);

	reply = manager.get( request );

	QEventLoop loop;

	QAbstractSocket::connect( reply, &QNetworkReply::finished, &loop, &QEventLoop::quit );
	loop.exec();

	QByteArray data = reply->readAll();

	return QString( data );
}
