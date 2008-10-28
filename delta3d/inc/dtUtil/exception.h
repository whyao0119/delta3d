/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Matthew W. Campbell
 */

#ifndef DELTA_EXCEPTION
#define DELTA_EXCEPTION

#include <string>
#include <iostream>
#include <dtUtil/export.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/log.h>

namespace dtUtil
{
   class DT_UTIL_EXPORT BaseExceptionType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(BaseExceptionType);

      public:

         static BaseExceptionType GENERAL_EXCEPTION;

      protected:

         BaseExceptionType(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

   /*
    * This is the exception class used throughout the dtDAL module.
    * In most cases the DAL module gracefully handles exceptions and
    * logs them for you.  However, applications using the dtDAL module
    * should excercise proper exception handling.
    */
   class DT_UTIL_EXPORT Exception
   {
      public:

         /**
          * Constructor - Initializes the exception and logs it.
          *  @param type - the type of exception being thrown.
          *  @param message - Message to display about the exception.
          *  @param filename - File the exception was thrown from.
          *  @param linenum - Line number in the file from which the
          *  exception was thrown.
          */
         Exception(Enumeration& type, const std::string& message, const std::string& filename,
               unsigned int linenum);

         /**
          * Constructor - Initializes the exception and logs it.
          *  @param message - Message to display about the exception.
          *  @param filename - File the exception was thrown from.
          *  @param linenum - Line number in the file from which the
          *  exception was thrown.
          */
         Exception(const std::string& message, const std::string& filename,
               unsigned int linenum);

         /*
          * Destructor - Empty
          */
         virtual ~Exception() { }

         /*
          * @return the message to be displayed when this exception is thrown.
          */
         const std::string &What() const { return mMessage; }

         /*
          * @return the filename associated with this exception.
          */
         const std::string &File() const { return mFileName; }

         /**
          * @return the line number associated with this exception.
          */
         unsigned int Line() const { return mLineNum; }

         /**
          * @return the enumerated type of the exception.
          */
         const Enumeration& TypeEnum() const { return mType; }

         /**
          * Converts this exception to a string.  The string contains the reason,
          * line number and file the exception was thrown from.
          * @return The string version of this exception.
          */
         std::string ToString() const;

         /**
          * Prints the exception to the console.
          */
         void Print() const;

         /**
          * logs the exception to the default logger.
          * @param level  The level/type of logging
          */
         void LogException(dtUtil::Log::LogMessageType level) const;

         /**
          * logs the exception to the following log level using the logger.
          * @param level  The level/type of logging
          * @param loggerName the name passed to "getInstance" of the Logger.
          */
         void LogException(dtUtil::Log::LogMessageType level, const std::string& loggerName) const;

         /**
          * logs the exception to the following log level using the given logger.
          * @param level  The level/type of logging
          * @param logger the actual log instance used to log.
          */
         void LogException(dtUtil::Log::LogMessageType level, dtUtil::Log& logger) const;

      protected:
         Enumeration& mType;
         std::string mMessage, mFileName;
         unsigned int mLineNum;
   };

   inline std::ostream& operator << (std::ostream& o, const Exception& ex)
   {
      o << ex.ToString();
      return o;
   }

}

#endif
