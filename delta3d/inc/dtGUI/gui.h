/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2010 MOVES Institute 
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
 */
#ifndef GUI_h__
#define GUI_h__

#include <dtGUI/export.h>
#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtGUI/ceguimouselistener.h>
#include <dtGUI/ceguikeyboardlistener.h>
#include <CEGUI/CEGUISubscriberSlot.h>
#include <CEGUI/CEGUIEvent.h>


/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Camera;
}
/// @endcond

namespace dtCore
{
   class Camera;
   class Keyboard;
   class Mouse;
}

namespace dtGUI
{
   typedef CEGUI::Window Widget;
   typedef CEGUI::ScriptModule BaseScriptModule;

   class ScriptModule;

   /** 
    * Used for rendering and managing on-screen, graphical user interfaces.
    */
   class DT_GUI_EXPORT GUI : public dtCore::Base
   {
   public:
      DECLARE_MANAGEMENT_LAYER(GUI)

      /** 
       * Create the GUI instance.
       * @param camera The Camera the UI will be rendered to
       * @param keyboard The Keyboard to listen to for events
       * @param mouse The Mouse to listen to for events
       */
      GUI(dtCore::Camera* camera,
          dtCore::Keyboard* keyboard = NULL,
          dtCore::Mouse* mouse = NULL);

      /** 
       * Sets the Camera to use for rendering to
       * @param camera If NULL, no GUI rendering will take place
       */
      void SetCamera(dtCore::Camera* camera);

      /** 
       *  Set the Mouse from which the gui receives events
       *  @param mouse If NULL, no Mouse events will be read
       */
      void SetMouse(dtCore::Mouse* mouse);

      /** 
       *  Set the Keyboard from which the GUI receives events.
       *  @param keyboard If NULL, no Keyboard events will be read
       */
      void SetKeyboard(dtCore::Keyboard* keyboard);

      ///returns default sheet (autogenerated root-window)
      Widget* GetRootSheet();

      ///returns default sheet (autogenerated root-window)
      const Widget* GetRootSheet() const;

      /** 
       * Load a Layout file
       * @param fileName The filename of the .layout file
       * @param prefix Optional string prefix that is to be used when creating 
       *        the windows in the layout file, this function allows a layout to be
       *        loaded multiple times without having name clashes.
       * @param resourceGroup Optional string of the resource group this layout belongs to
       * @return The root Widget loaded from the .layout
       */
      Widget* LoadLayout(const std::string& fileName, 
                         const std::string& prefix="",
                         const std::string& resourceGroup = "");

     /** 
       * Load a Layout file and automatically add the loaded root Widget to the supplied parent widget
       * @param parent The parent widget to add the loaded layout to
       * @param fileName The filename of the .layout file
       * @param prefix Optional string prefix that is to be used when creating 
       *        the windows in the layout file, this function allows a layout to be
       *        loaded multiple times without having name clashes.
       * @param resourceGroup Optional string of the resource group this layout belongs to
       * @return The root Widget loaded from the .layout
       */
      Widget* LoadLayout(Widget* parent, const std::string& fileName, 
                         const std::string& prefix="", const std::string& resourceGroup = "" );

      /** 
        * Create a Widget and add it to the internal root Widget
        * @param typeName The type of widget to create (e.g., "WindowsLook/FrameWindow")
        * @param name The name to apply to the widget
        * @return The created Widget (could be NULL if an error occurred)
        */
      Widget* CreateWidget(const std::string& typeName, const std::string& name="");

      /** 
        * Create a Widget and add it to the supplied parent Widget.
        * @param parent The parent widget to add newly created widget to
        * @param typeName The type of widget to create (e.g., "WindowsLook/FrameWindow")
        * @param name The name to apply to the widget
        * @return The created Widget (could be NULL if an error occurred)
        */
      Widget* CreateWidget(Widget* parent, const std::string& typeName, const std::string& name="");

      /** 
        * Remove widget from its parent and destroy it
        * @param widget the widget to destroy
        */
      void DestroyWidget(Widget* widget);

      /** 
        * Find and return the widget by widget name.
        * @param name the name of the widget to find
        * @return The found widget (or NULL, if not found)
        */
      Widget* GetWidget(const std::string& name);

      /** 
        * Searches and return all Widgets that have the supplied text as part
        * of their name.
        * @param subName Part of the name of the Widgets to find
        * @param toFill The found Widgets with subName in their names (or empty, if none found)
        */
      void FindWidgets(const std::string& subName, std::vector<Widget*>& toFill);

      /** 
        * Searches and return the first Widget that has the supplied text as part
        * of its  name.
        * @param subName Part of the name of the Widget to find
        * @return The first found Widget (or NULL, if not found)
        */
      Widget* FindWidget(const std::string& subName);

      /** 
        * Set the directory for the supplied resource type.
        * @code
        * dtGUI::GUI::SetResourceGroupDirectory("layouts", "c:\temp\layouts");
        * @endcode
        * @param resourceType The type of resource to set the directory for.  Could be
        * "imagesets", "looknfeels", "layouts", "lua_scripts", "schemes", or "fonts"
        * @param directory The directory containing the resource types
        */
      static void SetResourceGroupDirectory(const std::string& resourceType, const std::string& directory);

      typedef CEGUI::SubscriberSlot Subscriber;
      /** 
        * Subscribe a callback to the supplied widget's event.  
        * @param widgetName The name of an existing widget
        * @param event The name of an event generated by the widget
        * @param subscriber The callback functor to receive the event
        * @return The Connection object, used this to manage the connection
        */
      CEGUI::Event::Connection SubscribeEvent(const std::string& widgetName, 
                                              const std::string& event,
                                              GUI::Subscriber subscriber);

      /** 
        * Subscribe a callback to the supplied widget's event.  
        * @param window An existing widget 
        * @param event The name of an event generated by the widget
        * @param subscriber The callback functor to receive the event
        * @return The Connection object, used this to manage the connection
        */
      CEGUI::Event::Connection SubscribeEvent(Widget& window, 
                                              const std::string& event, 
                                              GUI::Subscriber subscriber);

      /** 
        * Is the supplied widget present in the system?
        * @param widgetName The name of widget
        * @return true if the widget exists, false otherwise
        */
      static bool IsWindowPresent(const std::string& widgetName);

      /** 
        * Is the supplied Imageset present in the system?
        * @param imagesetName The name of ImageSet
        * @return true if the ImageSet exists, false otherwise
        */
      static bool IsImagesetPresent(const std::string& imagesetName);

      /** 
        * Create an ImageSet based on the supplied Image file. The Imageset will 
        * initially have a single image defined named "full_image" which is an image 
        * that represents the entire area of the loaded image. 
        * @param imagesetName 
        * @param fileName The name of the Image file to load
        * @param resourceGroup Optional resource group this imageset belongs to
        */
      static void CreateImageset(const std::string& imagesetName,
                                 const std::string& fileName,
                                 const std::string& resourceGroup = "");

      /** 
        * Destroy the supplied imageset if it exists
        * @param imagesetName The name of ImageSet
        */
      static void DestroyImageset(const std::string& imagesetName);

      /**
       * Shortcut to the CEGUI::SchemeManager
       * @param fileName The filename of the .scheme file to load
       * @param resourceGroup The name of the optional resource group this belongs to
       */
      static void LoadScheme(const std::string& fileName, const std::string& resourceGroup = "");

      /**
       * Set the image for the rendered mouse cursor.
       * @param imagesetName The ImageSet name
       * @param imageName The name of the Image defined in the ImageSet
       */
      static void SetMouseCursor(const std::string& imagesetName, const std::string& imageName);

      /**
       * Set the ScriptModule to use with CEGUI.  This is typically used to 
       * map callbacks to CEGUI Events when loading from a .layout file.
       * @param scriptModule : The script module to set on the CEGUI::System
       */
      static void SetScriptModule(BaseScriptModule* scriptModule);

      /** 
       * Get the currently assigned ScriptModule used by CEGUI.
       * @return The currently assigned ScriptModule, could be NULL
       */
      static BaseScriptModule* GetScriptModule();

   protected:
      virtual ~GUI();
   	
   private:
      void _SetupInternalGraph();
      void _SetupDefaultUI();

      ///used to receive "preframe"-delta-messages
      void OnMessage(dtCore::Base::MessageData *data);

      static void _SetupSystemAndRenderer();
      static bool SystemAndRendererCreatedByHUD;

      dtCore::RefPtr<dtGUI::CEGUIKeyboardListener> mKeyboardListener; ///needed for injection mouse-events to the cegui
      dtCore::RefPtr<dtGUI::CEGUIMouseListener>    mMouseListener; ///needed for injection keyboard-events to the cegui
      osg::Geode*                                  mInternalGraph; ///osg graph used to render the gui
      CEGUI::Window*                               mRootSheet; ///auto-generated panel-window

      dtCore::RefPtr<osg::Camera>           mCamera; ///camera, whose viewport is used to draw the gui
      dtCore::RefPtr<dtCore::Keyboard>      mKeyboard; ///observed keyboard
      dtCore::RefPtr<dtCore::Mouse>         mMouse; ///observed mouse

   };
}
#endif // GUI_h__
