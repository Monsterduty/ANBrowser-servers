#ifndef NETWORKING_HPP
#define NETWORKING_HPP
#include <QString>
#include <vector>
#include <QByteArray>

QString download( const char *link, std::vector<std::pair<QByteArray, QByteArray>> headers = {} );

//QString sendPOST( QString link, QByteArray data, std::vector<std::pair<QByteArray, QByteArray>> headers = {} )

#endif
