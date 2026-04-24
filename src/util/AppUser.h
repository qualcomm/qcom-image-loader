// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

namespace Util {

class User
{
public:
   // ── Answer types ─────────────────────────────────────────────────────
   enum class YesOrNo
   {
      NO = 0,
      YES = 1
   };
   enum class OkOrCancel
   {
      CANCEL = 0,
      OK = 1
   };

   using FilePath = std::filesystem::path;

   // ── Callback types (std::function) ────────
   using YesOrNoHandler = std::function<YesOrNo(const std::string&, YesOrNo)>;
   using OkOrCancelHandler = std::function<OkOrCancel(const std::string&, OkOrCancel)>;
   using WarningHandler = std::function<void(const std::string&)>;
   using FileOperationHandler = std::function<FilePath(const std::string&, const FilePath&, const std::string&)>;

   // ── Default (no-op / pass-through) handlers ───────────────────────────
   static YesOrNo defaultYesOrNoHandler(const std::string&, YesOrNo def)
   {
      return def;
   }
   static OkOrCancel defaultOkOrCancelHandler(const std::string&, OkOrCancel def)
   {
      return def;
   }
   static void defaultWarningHandler(const std::string&)
   {
   }
   static FilePath defaultFileOpenHandler(const std::string&, const FilePath& dir, const std::string&)
   {
      return dir;
   }
   static FilePath defaultFileSaveHandler(const std::string&, const FilePath& dir, const std::string&)
   {
      return dir;
   }

   // ── Constructor ───────────────────────────────────────────────────────
   explicit User(
      YesOrNoHandler yesOrNo = &defaultYesOrNoHandler,
      OkOrCancelHandler okCancel = &defaultOkOrCancelHandler,
      WarningHandler warning = &defaultWarningHandler,
      FileOperationHandler fileOpen = &defaultFileOpenHandler,
      FileOperationHandler fileSave = &defaultFileSaveHandler
   )
   : m_yesOrNoHandler(std::move(yesOrNo))
   , m_okOrCancelHandler(std::move(okCancel))
   , m_warningHandler(std::move(warning))
   , m_fileOpenHandler(std::move(fileOpen))
   , m_fileSaveHandler(std::move(fileSave))
   {
   }

   // ── Setters ───────────────────────────────────────────────────────────
   void setYesOrNoHandler(YesOrNoHandler h)
   {
      m_yesOrNoHandler = std::move(h);
   }
   void setOkOrCancelHandler(OkOrCancelHandler h)
   {
      m_okOrCancelHandler = std::move(h);
   }
   void setWarningHandler(WarningHandler h)
   {
      m_warningHandler = std::move(h);
   }
   void setFileOpenHandler(FileOperationHandler h)
   {
      m_fileOpenHandler = std::move(h);
   }
   void setFileSaveHandler(FileOperationHandler h)
   {
      m_fileSaveHandler = std::move(h);
   }

   // ── Interaction methods ───────────────────────────────────────────────
   YesOrNo ask(const std::string& q, YesOrNo def) const
   {
      return m_yesOrNoHandler ? m_yesOrNoHandler(q, def) : def;
   }

   OkOrCancel ask(const std::string& q, OkOrCancel def) const
   {
      return m_okOrCancelHandler ? m_okOrCancelHandler(q, def) : def;
   }

   void warn(const std::string& w) const
   {
      if(m_warningHandler) m_warningHandler(w);
   }

   FilePath openFile(const std::string& caption, const FilePath& dir, const std::string& filter)
   {
      return m_fileOpenHandler ? m_fileOpenHandler(caption, dir, filter) : dir;
   }

   FilePath saveFile(const std::string& caption, const FilePath& dir, const std::string& filter)
   {
      return m_fileSaveHandler ? m_fileSaveHandler(caption, dir, filter) : dir;
   }

private:
   YesOrNoHandler m_yesOrNoHandler;
   OkOrCancelHandler m_okOrCancelHandler;
   WarningHandler m_warningHandler;
   FileOperationHandler m_fileOpenHandler;
   FileOperationHandler m_fileSaveHandler;
};

/// std::shared_ptr<User> replaces Mem::CheckedPointer<User> / Util::UserPtr
using UserPtr = std::shared_ptr<User>;

} // namespace Util
