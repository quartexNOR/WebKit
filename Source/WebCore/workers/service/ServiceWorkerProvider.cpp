/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ServiceWorkerProvider.h"

#if ENABLE(SERVICE_WORKER)

#include "Document.h"
#include "SWClientConnection.h"
#include "SchemeRegistry.h"
#include <pal/SessionID.h>

namespace WebCore {

static ServiceWorkerProvider* sharedProvider;

ServiceWorkerProvider::~ServiceWorkerProvider()
{
}

ServiceWorkerProvider& ServiceWorkerProvider::singleton()
{
    RELEASE_ASSERT(sharedProvider);
    return *sharedProvider;
}

void ServiceWorkerProvider::setSharedProvider(ServiceWorkerProvider& newProvider)
{
    sharedProvider = &newProvider;
}

bool ServiceWorkerProvider::mayHaveServiceWorkerRegisteredForOrigin(PAL::SessionID sessionID, const SecurityOriginData& origin)
{
    auto* connection = existingServiceWorkerConnectionForSession(sessionID);
    if (!connection)
        return m_mayHaveRegisteredServiceWorkers;

    return connection->mayHaveServiceWorkerRegisteredForOrigin(origin);
}

void ServiceWorkerProvider::registerServiceWorkerClients()
{
    setMayHaveRegisteredServiceWorkers();
    for (auto* document : Document::allDocuments()) {
        auto sessionID = document->sessionID();
        if (!sessionID.isValid())
            continue;

        if (SchemeRegistry::canServiceWorkersHandleURLScheme(document->url().protocol().toStringWithoutCopying()))
            document->setServiceWorkerConnection(&serviceWorkerConnectionForSession(sessionID));
    }
}

} // namespace WebCore

#endif // ENABLE(SERVICE_WORKER)
