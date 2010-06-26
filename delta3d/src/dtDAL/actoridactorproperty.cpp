/* -*-c++-*-
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
 * Eric Heine
 */
#include <prefix/dtdalprefix.h>
#include <dtDAL/actoridactorproperty.h>

#include <dtCore/deltadrawable.h>
#include <dtDAL/datatype.h>
#include <dtDAL/map.h>
#include <dtDAL/project.h>

namespace dtDAL
{
   ////////////////////////////////////////////////////////////////////////////
   ActorIDActorProperty::ActorIDActorProperty(ActorProxy& actorProxy,
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      SetFuncType Set,
      GetFuncType Get,
      const dtUtil::RefString& desiredActorClass,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : ActorProperty(DataType::ACTOR, name, label, desc, groupName)
      , mProxy(&actorProxy)
      , SetIdFunctor(Set)
      , GetIdFunctor(Get)
      , mDesiredActorClass(desiredActorClass)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   ActorIDActorProperty::ActorIDActorProperty(
      const dtUtil::RefString& name,
      const dtUtil::RefString& label,
      SetFuncType Set,
      GetFuncType Get,
      const dtUtil::RefString& desiredActorClass,
      const dtUtil::RefString& desc,
      const dtUtil::RefString& groupName)
      : ActorProperty(DataType::ACTOR, name, label, desc, groupName)
      , mProxy(NULL)
      , SetIdFunctor(Set)
      , GetIdFunctor(Get)
      , mDesiredActorClass(desiredActorClass)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void ActorIDActorProperty::CopyFrom(const ActorProperty& otherProp)
   {
      if (GetDataType() != otherProp.GetDataType())
      {
         LOG_ERROR("Property types are incompatible. Cannot make copy.");
         return;
      }

      const ActorIDActorProperty& prop =
         static_cast<const ActorIDActorProperty& >(otherProp);

      SetValue(prop.GetValue());
   }

   ////////////////////////////////////////////////////////////////////////////
   void ActorIDActorProperty::SetValue(const dtCore::UniqueId& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("SetValue has been called on a property that is read only.");
         return;
      }

      SetIdFunctor(value);
   }

   ////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId ActorIDActorProperty::GetValue() const
   {
      return GetIdFunctor();
   }

   ////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* ActorIDActorProperty::GetRealActor()
   {
      dtDAL::ActorProxy* proxy = GetActorProxy();
      if (proxy != NULL)
      {
         return proxy->GetActor();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* ActorIDActorProperty::GetRealActor() const
   {
      const dtDAL::ActorProxy* proxy = GetActorProxy();
      if (proxy != NULL)
      {
         return proxy->GetActor();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxy* ActorIDActorProperty::GetActorProxy()
   {
      dtCore::UniqueId idValue = GetValue();
      if (idValue.ToString() == "") return NULL;

      try
      {
         // TODO: Retrieve the actor proxy from the game manager, or global
         // actor manager, instead of one that is loaded from a map file.
         dtDAL::Map* map = Project::GetInstance().GetMapForActorProxy(idValue);
         if (map == NULL)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_INFO,
               __FUNCTION__, __LINE__, "Map not found.  Setting property %s with string value failed.",
               GetName().c_str());
            return false;
         }

         ActorProxy* proxyValue = map->GetProxyById(idValue);
         if (proxyValue == NULL)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_INFO,
               __FUNCTION__, __LINE__, "Actor with ID %s not found.  Setting property %s with string value failed.",
               idValue.ToString().c_str(), GetName().c_str());
            return false;
         }

         return proxyValue;
      }
      catch (const dtDAL::ProjectInvalidContextException& ex)
      {
         dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
            __FUNCTION__, __LINE__, "Project context is not set, unable to lookup actors.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());
      }
      catch (const dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
            __FUNCTION__, __LINE__, "Error setting ActorIDActorProperty.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtDAL::ActorProxy* ActorIDActorProperty::GetActorProxy() const
   {
      dtCore::UniqueId idValue = GetValue();
      if (idValue.ToString() == "") return NULL;

      try
      {
         // TODO: Retrieve the actor proxy from the game manager, or global
         // actor manager, instead of one that is loaded from a map file.
         dtDAL::Map* map = Project::GetInstance().GetMapForActorProxy(idValue);
         if (map == NULL)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_INFO,
               __FUNCTION__, __LINE__, "Map not found.  Setting property %s with string value failed.",
               GetName().c_str());
            return false;
         }

         ActorProxy* proxyValue = map->GetProxyById(idValue);
         if (proxyValue == NULL)
         {
            dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_INFO,
               __FUNCTION__, __LINE__, "Actor with ID %s not found.  Setting property %s with string value failed.",
               idValue.ToString().c_str(), GetName().c_str());
            return false;
         }

         return proxyValue;
      }
      catch (const dtDAL::ProjectInvalidContextException& ex)
      {
         dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
            __FUNCTION__, __LINE__, "Project context is not set, unable to lookup actors.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());
      }
      catch (const dtUtil::Exception& ex)
      {
         dtUtil::Log::GetInstance("enginepropertytypes.cpp").LogMessage(dtUtil::Log::LOG_WARNING,
            __FUNCTION__, __LINE__, "Error setting ActorIDActorProperty.  Setting property %s with string value failed. Error Message %s.",
            GetName().c_str(), ex.What().c_str());
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool ActorIDActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
         return false;

      if (value.empty() || value == "NULL")
      {
         dtCore::UniqueId id;
         id = "";
         SetValue(id);
         return true;
      }

      dtCore::UniqueId newIdValue(value);
      SetValue(newIdValue);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string ActorIDActorProperty::ToString() const
   {
      dtCore::UniqueId id = GetValue();
      return id.ToString();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ActorIDActorProperty::GetValueString()
   {
      dtCore::DeltaDrawable* drawable = GetRealActor();
      if (drawable)
      {
         return drawable->GetName();
      }

      return "None";
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& ActorIDActorProperty::GetDesiredActorClass() const
   {
      return mDesiredActorClass;
   }
   ////////////////////////////////////////////////////////////////////////////
}

