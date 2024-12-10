//*****************************************************************
/*
  JackTrip: A System for High-Quality Audio Network Performance
  over the Internet

  Copyright (c) 2022-2024 JackTrip Labs, Inc.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/
//*****************************************************************

/**
 * \file VsDeeplink.h
 * \author Mike Dickey, based on code by Aaron Wyatt and Matt Horton
 * \date December 2024
 */

#ifndef __VSDEEPLINK_H__
#define __VSDEEPLINK_H__

#include <QLocalSocket>
#include <QString>
#include <QUrl>

class VsDeeplink : public QObject
{
    Q_OBJECT

   public:
    // construct with an instance of the application, to parse command line args
    VsDeeplink();

    // virtual destructor since it inherits from QObject
    // this is used to unregister url handler
    virtual ~VsDeeplink();

   public slots:
    // handleUrl is called to trigger processing of a VsDeeplink
    void handleUrl(const QUrl& url);

    // called by local socket server to process VsDeeplink requests
    void handleVsDeeplinkRequest(QLocalSocket& socket);

   signals:
    // signalVsDeeplink is emitted when we want the local instance to process a VsDeeplink
    void signalVsDeeplink(const QUrl& url);

   private:
    // sets url scheme for windows machines; does nothing on other platforms
    static void setUrlScheme();
};

#endif  // __VSDEEPLINK_H__
