/*
   Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
*/

#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <qnetworkcookie.h>

class CookieJarPrivate;
class CookieJar : public QNetworkCookieJar {
    Q_OBJECT
public:
    CookieJar(QObject *parent = 0);
    ~CookieJar();

    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl & url) const;
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

protected:
    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);
    void endSession();

    QList<QNetworkCookie> allCookies() const;
    void setAllCookies(const QList<QNetworkCookie> &cookieList);
    void setSecondLevelDomains(const QStringList &secondLevelDomains);

private:
    CookieJarPrivate *d;
};

#endif

