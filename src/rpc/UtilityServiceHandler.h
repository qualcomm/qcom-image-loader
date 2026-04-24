// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "function/Fwd.h"
#include "ImageManagementDefinitions.h"
#include "rpc/Fwd.h"
#include "rpc/Service.h"

// Forward declaration to avoid circular dependency
namespace QC {
class ClientCallbackHandler;
}

namespace Rpc {

class UtilityServiceHandler
: public Rpc::ServiceHandlerBase
, public std::enable_shared_from_this<UtilityServiceHandler>
{
public:
   UtilityServiceHandler();
   virtual ~UtilityServiceHandler();
   QC::ErrorCode::type initializeService();
   QC::ErrorCode::type destroyService();
   QC::ErrorCode::type createDigestsForVipDownload(
      const std::string& buildPath,
      const QC::DownloadBuildOptions& options,
      const std::string& outputPath
   );
   QC::ErrorCode::type createDigestsForBuildValidation(
      const std::string& buildPath,
      const QC::DownloadBuildOptions& options,
      const std::string& outputPath
   );

   /// Helper to get Util::CheckedPointer from this object
   inline Rpc::ServiceHandlerBasePtr getServiceHandlerPtr()
   {
      try
      {
         return std::static_pointer_cast<Rpc::ServiceHandlerBase>(shared_from_this());
      }
      catch(const std::bad_weak_ptr&)
      {
         return nullptr; // or handle error
      }
   }

protected:
   typedef int64_t EventId;
   void sendEvent(EventId eventId, const std::string& description = std::string());

private:
   static std::recursive_mutex g_utilityServiceMutex;

   // Store reference to ClientCallbackHandler to avoid singleton access during
   // destruction
   Util::CheckedPointer<QC::ClientCallbackHandler> m_pClientCallbackHandler;

   // Message publisher for async events
   std::shared_ptr<Util::IMessagePublisher> m_pPublisher;

   void onImageTransferEvent(Function::ImageTransferEvent* pEvent);
   void offlineFirehoseProcess(
      const int32_t process,
      const std::string& buildPath,
      const QC::DownloadBuildOptions& options,
      const std::string& outputPath
   );
};

} // namespace Rpc
