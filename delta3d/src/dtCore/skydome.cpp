#include <prefix/dtcoreprefix-src.h>
#include <dtCore/skydome.h>
#include <dtCore/moveearthtransform.h>
#include <dtCore/makeskydome.h>
#include <osg/Depth>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Node>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(SkyDome)

SkyDome::SkyDome( const std::string& name, bool createCapGeometry )
:EnvEffect(name),
mEnableCap(createCapGeometry)
{
   RegisterInstance(this);

   SetOSGNode( new osg::Group() );
   mBaseColor.set(0.5f, 0.5f, 0.2f);
   Config();
}

SkyDome::~SkyDome()
{
   DeregisterInstance(this);
}

// Build the sky dome
void dtCore::SkyDome::Config()
{
   osg::Group *group = new osg::Group();

    // use a transform to make the sky and base around with the eye point.
   mXform = new MoveEarthySkyWithEyePointTransformAzimuth();
   mXform->SetAzimuth( 0.0f ); //zero out the rotation of the dome

    // Transform's value isn't known until the cull traversal, so its bounding
    // volume can't be determined. Therefore culling will be invalid,
    // so switch it off. This will cause all of our parents to switch culling
    // off as well. But don't worry, culling will be back on once underneath
    // this node or any other branch above this transform.
   mXform->setCullingActive(false);
   mGeode = MakeSkyDome( *this ).Compute();
   mXform->addChild( mGeode.get() );
   group->addChild(mXform.get());
   group->setNodeMask(0xf0000000);

   GetOSGNode()->asGroup()->addChild(group);
}


void dtCore::SkyDome::SetBaseColor(const osg::Vec3& color)
{
   osg::Geometry *geom = mGeode->getDrawable(0)->asGeometry();
   osg::Array *array = geom->getColorArray();
   if (array && array->getType() == osg::Array::Vec4ArrayType)
   {
      mBaseColor.set(color);

      osg::Vec4Array *color = static_cast<osg::Vec4Array*>(array);
      int limit = mEnableCap?38:19;
      for (int i=0; i<limit; i++)
      {
         (*color)[i].set(mBaseColor[0], mBaseColor[1], mBaseColor[2], 1.f);
      }
      geom->dirtyDisplayList();
   }
}

/** 0 degrees = horizon
 *  90 degrees = high noon
 *  - degrees = below horizon
 */
void dtCore::SkyDome::Repaint(   const osg::Vec3& skyColor, 
                                 const osg::Vec3& fogColor,
                                 double sunAngle, 
                                 double sunAzimuth,
                                 double visibility )
{
   double diff;
   osg::Vec3 outer_param, outer_amt, outer_diff;
   osg::Vec3 middle_param, middle_amt, middle_diff;
   int i, j;

   //rotate the dome to line up with the sun's azimuth.
   mXform->SetAzimuth( sunAzimuth );

   // Check for sunrise/sunset condition
   if( sunAngle > -10.0 && sunAngle < 10.0 )
   {
      // 0.0 - 0.4
      outer_param.set(
         (10.0 - fabs(sunAngle)) / 20.0,
         (10.0 - fabs(sunAngle)) / 40.0,
         -(10.0 - fabs(sunAngle)) / 30.0 );

      middle_param.set(
         (10.0 - fabs(sunAngle)) / 40.0,
         (10.0 - fabs(sunAngle)) / 80.0,
         0.0 );

      outer_diff = outer_param / 9.0;

      middle_diff = middle_param / 9.0;
   } 
   else 
   {
      outer_param.set( 0.0, 0.0, 0.0 );
      middle_param.set( 0.0, 0.0, 0.0 );

      outer_diff.set( 0.0, 0.0, 0.0 );
      middle_diff.set( 0.0, 0.0, 0.0 );
   }

   outer_amt.set( outer_param );
   middle_amt.set( middle_param );

   // First, recaclulate the basic colors

   static osg::Vec4 center_color;
   static osg::Vec4 upper_color[19];
   static osg::Vec4 middle_color[19];
   static osg::Vec4 lower_color[19];
   static osg::Vec4 bottom_color[19];

   double vis_factor, cvf = visibility;
   if (cvf > 20000.f)
   {
      cvf = 20000.f;
   }

   if ( visibility < 3000.0 ) 
   {
      vis_factor = (visibility - 1000.0) / 2000.0;
      if ( vis_factor < 0.0 ) 
      {
         vis_factor = 0.0;
      }
   } 
   else 
   {
      vis_factor = 1.0;
   }

   for ( j = 0; j < 3; j++ ) 
   {
      diff = skyColor[j] - fogColor[j];
      center_color[j] = skyColor[j] - diff * ( 1.0 - vis_factor );
   }
   center_color[3] = 1.0;


   for ( i = 0; i < 9; i++ ) 
   {
      for ( j = 0; j < 3; j++ ) 
      {
         diff = skyColor[j] - fogColor[j];

         upper_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.7 + 0.3 * cvf/20000.f) );
         middle_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.1 + 0.85 * cvf/20000.f) )
            + middle_amt[j];
         lower_color[i][j] = fogColor[j] + outer_amt[j];

         if ( upper_color[i][j] > 1.0 ) { upper_color[i][j] = 1.0; }
         if ( upper_color[i][j] < 0.0 ) { upper_color[i][j] = 0.0; }
         if ( middle_color[i][j] > 1.0 ) { middle_color[i][j] = 1.0; }
         if ( middle_color[i][j] < 0.0 ) { middle_color[i][j] = 0.0; }
         if ( lower_color[i][j] > 1.0 ) { lower_color[i][j] = 1.0; }
         if ( lower_color[i][j] < 0.0 ) { lower_color[i][j] = 0.0; }
      }
      upper_color[i][3] = middle_color[i][3] = lower_color[i][3] = 1.0;

      for ( j = 0; j < 3; j++ ) 
      {
         outer_amt[j] -= outer_diff[j];
         middle_amt[j] -= middle_diff[j];
      }
   }

   outer_amt.set( 0.0, 0.0, 0.0 );
   middle_amt.set( 0.0, 0.0, 0.0 );

   for ( i = 9; i < 19; i++ ) 
   {
      for ( j = 0; j < 3; j++ ) 
      {
         diff = skyColor[j] - fogColor[j];

         upper_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.7 + 0.3 * cvf/20000.f) );
         middle_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.1 + 0.85 * cvf/20000.f) )
            + middle_amt[j];
         lower_color[i][j] = fogColor[j] + outer_amt[j];

         if ( upper_color[i][j] > 1.0 ) { upper_color[i][j] = 1.0; }
         if ( upper_color[i][j] < 0.0 ) { upper_color[i][j] = 0.0; }
         if ( middle_color[i][j] > 1.0 ) { middle_color[i][j] = 1.0; }
         if ( middle_color[i][j] < 0.0 ) { middle_color[i][j] = 0.0; }
         if ( lower_color[i][j] > 1.0 ) { lower_color[i][j] = 1.0; }
         if ( lower_color[i][j] < 0.0 ) { lower_color[i][j] = 0.0; }
      }
      upper_color[i][3] = middle_color[i][3] = lower_color[i][3] = 1.0;

      for ( j = 0; j < 3; j++ ) 
      {
         outer_amt[j] += outer_diff[j];
         middle_amt[j] += middle_diff[j];
      }
   }

   for ( i = 0; i < 19; i++ ) 
   {
      bottom_color[i].set(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
   }

   //repaint the lower ring
   osg::Geometry *geom = mGeode->getDrawable(0)->asGeometry();
   osg::Array *array = geom->getColorArray();
   if (array && array->getType()==osg::Array::Vec4ArrayType)
   {
      osg::Vec4Array *color = static_cast<osg::Vec4Array*>(array);
      // Set cap color
      if( mEnableCap )
      {
         for (i=0; i<19; i++)
         {
            (*color)[i].set(bottom_color[i][0], bottom_color[i][1], bottom_color[i][2], 1.f);
         }
      }
      // Set dome colors
      int c;
      for(i=0, c=mEnableCap?19:0; i<19; i++, c++ )
      {
         (*color)[c].set(bottom_color[i][0], bottom_color[i][1], bottom_color[i][2], 1.f);
         (*color)[c+19].set(lower_color[i][0], lower_color[i][1], lower_color[i][2], 1.f);
         (*color)[c+19+19].set(middle_color[i][0], middle_color[i][1], middle_color[i][2], 1.f);
         (*color)[c+19+19+19].set(upper_color[i][0], upper_color[i][1], upper_color[i][2], 1.f);
         (*color)[c+19+19+19+19].set(center_color[0], center_color[1], center_color[2], 1.f);
      }

   }

   geom->dirtyDisplayList();
}






}
