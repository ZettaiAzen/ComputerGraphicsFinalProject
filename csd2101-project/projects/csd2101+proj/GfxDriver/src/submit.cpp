/*!
@file    submit.cpp
@author  Prasanna Ghali       (pghali@digipen.edu)
@co-author  Parminder Singh   (parminder.singh@digipen.edu)

Consolidates the implementations you must complete -- bounding spheres
(gfxModel::ComputeModelBVSphere, gfxSphere::Transform), culling/clipping
(YourClipper), rasterization (YourRasterizer), and the first-person camera
(YourCamera) -- previously YourBoundingSphere.cpp, YourCullerClipper.cpp,
YourRasterizer.cpp, and YourCamera.cpp -- so that all your work is
submitted in a single file pair: submit.h / submit.cpp.

All content (c) 2002 DigiPen Institute of Technology, all rights reserved.
*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */

#include "submit.h"

/*                                                           bounding spheres
                                             (was YourBoundingSphere.cpp)
----------------------------------------------------------------------------- */

/*  _________________________________________________________________________ */
gfxSphere gfxModel::
ComputeModelBVSphere(std::vector<gfxVector3> const& verts)
/*! Compute model's bounding sphere (in model frame) using Ritter's method.

    @param verts	-->	Model-frame vertices of scene object. This function will be
                      called once for each model as it is loaded. You should compute
                      the bounding sphere containing all the input vertices
                      stored in the verts vector using Ritter's method.
                      See GfxLib/Model.h, GfxLib/Sphere.h, GfxLib/Vertex.h for more
                      information regarding gfxModel, gfxSphere, and gfxVertex class
                      declarations.

    @return
    Object of type gfxSphere defining the bounding sphere containing the model using
    Ritter's method.
*/
{
  //@todo Implement me.

    // find pairs of vertices containing minmax x, y, z values
    // lets start by initialising all of them with the first vertex
    gfxVector3 maxX{verts[0]}, minX{ verts[0] }, maxY{ verts[0] }, minY{ verts[0] }, maxZ{ verts[0] }, minZ{ verts[0] };

    // loop through the vertices
    for (const gfxVector3& vec : verts) {
        if (vec.x > maxX.x) {
            maxX = vec;
        }
        if (vec.x < minX.x) {
            minX = vec;
        }

        if (vec.y > maxY.y) {
            maxY = vec;
        }
        if (vec.y < minY.y) {
            minY = vec;
        }

        if (vec.z > maxZ.z) {
            maxZ = vec;
        }
        if (vec.z < minZ.z) {
            minZ = vec;
        }
    }

    // find the pair with the greatest distance
    gfxVector3 xVec = maxX - minX;
    gfxVector3 yVec = maxY - minY;
    gfxVector3 zVec = maxZ - minZ;

    float xDist = xVec.Length();
    float yDist = yVec.Length();
    float zDist = zVec.Length();

    float greatestDist = (xDist > yDist ? (xDist > zDist ? xDist : zDist) : (yDist > zDist ? yDist : zDist));

    // SECOND PASS
    // 
    // depending on which axis it is, calculate center
    gfxVector3 center{};

    if (greatestDist == xDist) {
        center = minX + (0.5)*xVec;
    }
    else if (greatestDist == yDist) {
        center = minY + (0.5) * yVec;
    }
    else {
        // its z
        center = minZ + (0.5) * zVec;
    }

    float radius = greatestDist / 2.0f;

    // check for all points again, if any of them is outside
    for (const gfxVector3 point : verts) {
        gfxVector3 toPoint = point - center;
        float dist = toPoint.Length();
        if (dist > radius) {
            toPoint.Normalize();
            gfxVector3 toPointOpp = center - radius * toPoint;
            
            // update center
            center = (toPointOpp + point);
            center /= 2.f;

            gfxVector3 centerToEdge = point - center;
            radius = centerToEdge.Length();
        }
    }

  return (gfxSphere(center.x, center.y, center.z, radius));
}

/*  _________________________________________________________________________ */
gfxSphere gfxSphere::
Transform(gfxMatrix4 const& xform) const
/*! Transform a bounding sphere to a destination reference frame using
    the matrix manifestation of the transform from model to destination
    frame. Note that the only valid affine transforms for this assignment are:
    scale (if any), followed by rotation (if any), followed by translation.

    @param xform -->  The matrix manifestation of transform from model to
                      destination frame.

    @return
    The model bounding sphere transformed to destination frame.
*/
{
  //@todo Implement me.

    gfxVector4 newCenter = xform * gfxVector4(center.x, center.y, center.z, 1.0);

    // using lengths of each axis edge to check for the greatest scale...
    gfxVector4 edgeX = gfxVector4(center.x + radius, center.y, center.z, 1.0);
    gfxVector4 newEdgeX = xform * edgeX;

    gfxVector4 edgeY = gfxVector4(center.x, center.y + radius, center.z, 1.0);
    gfxVector4 newEdgeY = xform * edgeY;

    gfxVector4 edgeZ = gfxVector4(center.x, center.y, center.z +radius, 1.0);
    gfxVector4 newEdgeZ = xform * edgeZ;

    gfxVector4 xVec = newEdgeX - newCenter;
    gfxVector4 yVec = newEdgeY - newCenter;
    gfxVector4 zVec = newEdgeZ - newCenter;

    float xDist = xVec.Length();
    float yDist = yVec.Length();
    float zDist = zVec.Length();

    // the greatest distance is the new radius
    float newRadius = (xDist > yDist ? (xDist > zDist ? xDist : zDist) : (yDist > zDist ? yDist : zDist));

    return (gfxSphere(newCenter.x, newCenter.y, newCenter.z, newRadius));
}

/*                                                           culling/clipping
                                              (was YourCullerClipper.cpp)
----------------------------------------------------------------------------- */

/*  _________________________________________________________________________ */
gfxFrustum YourClipper::
/*! Get view frame frustum plane equations

  @param perspective_mtx	--> Matrix manifestation of perspective (or, orthographic)
  transform.

  @return	--> gfxFrustum
  Plane equations of the six surfaces that specify the view volume in view frame.
*/
ComputeFrustum(gfxMatrix4 const&	perspective_mtx) {
  //@todo Implement me.
  return (gfxFrustum());
}

/*  _________________________________________________________________________ */
bool YourClipper::
/*! Performing culling.

@param bs		--> View-frame definition of the bounding sphere of object
which is being tested for inclusion, exclusion, or
intersection with view frustum.
@param f		--> View-frame frustum plane equations.
@param oc		--> Six-bit flag specifying the frustum planes intersected
by bounding sphere of object. A given bit of the outcode
is set if the sphere crosses the appropriate plane for that
outcode bit - otherwise the bit is cleared.

@return
True if the vertices bounded by the sphere should be culled.
False otherwise.

If the return value is false, the outcode oc indicates which planes
the sphere intersects with. A given bit of the outcode is set if the
sphere crosses the appropriate plane for that outcode bit.
*/
Cull(gfxSphere const& bounding_sphere, gfxFrustum const&	frustum, gfxOutCode *ptr_outcode) {
  //@todo Implement me.
  *ptr_outcode = 0; // for now, assume bounding sphere completely inside frustum
  return (false);	  // for now, assume object is not trivially rejected
}

/*  _________________________________________________________________________ */
gfxVertexBuffer YourClipper::
/*!
Perform clipping.

@param outcode	--> Outcode of view-frame of bounding sphere of object specifying
the view-frame frustum planes that the sphere is straddling.

@param vertex_buffer	--> The input vertex buffer contains three points
forming a triangle.
Each vertex has x_c, y_c, z_c, and w_c fields that describe the position
of the vertex in clip space. Additionally, each vertex contains an array
of floats (bs[6]) that contains the boundary condition for each clip plane,
and an outcode value specifying which planes the vertex is inside.

The gfxClipPlane enum in GraphicsPipe.h contains the indices into bs
array. gfxClipCode contains bit values for each clip plane code.

If an object's bounding sphere could not be trivially accepted nor rejected,
it is reasonable to expect that the object is straddling only a
subset of the six frustum planes. This means that the object's triangles
need not be clipped against all the six frustum planes but only against
the subset of planes that the bounding sphere is straddling.
Furthermore, even if the bounding sphere is straddling a subset of planes,
the triangles themselves can be trivially accepted or rejected.
To implement the above two insights, use argument outcode - the object bounding
sphere's outcode which was previously returned by Cull().

Notes:
When computing clip frame intersection points, ensure that all necessary information
required to project and rasterize the vertex is computed using linear interpolation
between the inside and outside vertices. This includes:
clip frame coordinates: (c_x, c_y, c_z, c_w),
texture coordinates: (u, v),
vertex color coordinates: (r, g, b, a),
boundary conditions: bc[GFX_CPLEFT], bc[GFX_CPRIGHT], ...
outcode: oc

As explained in class, consistency in computing computing the parameter t
using t = 0 for inside point and t = 1 for outside point helps in preventing
tears and other artifacts.

Although the input primitive is a triangle, after clipping, the output
primitive may be a convex polygon with more than 3 vertices. In that
case, you must produce as output an ordered list of clipped vertices
that form triangles when taken in groups of three.

@return None
*/
Clip(gfxOutCode outcode, gfxVertexBuffer const& vertex_buffer) {
  //@todo Implement me.
  return (gfxVertexBuffer());
}

/*                                                              rasterization
                                                (was YourRasterizer.cpp)
----------------------------------------------------------------------------- */

/*  _________________________________________________________________________ */
void YourRasterizer::
DrawFilled(gfxGraphicsPipe *dev, gfxVertex const&	v0,
           gfxVertex const&	v1, gfxVertex const&	v2)
/*! Render a triangle in filled mode using 3 device-frame points.
  @param dev <->  Pointer to the pipe to render to.
  @param v0  -->  First vertex.
  @param v1  -->  Second vertex.
  @param v2  -->  Third vertex.

READ THIS FUNCTION HEADER CAREFULLY:

Begin by reading the declaration of type gfxVertex (in Vertex.h) so that
you're aware of the data encapsulated by a gfxVertex object.

-------------------------------------------------------------------------------
Triangles can be rendered in one of three ways:
1) Texture mapped (but not lit)
2) Lit (but not texture mapped)
3) Lit and texture mapped

To reduce computational overheads, this software pipe doesn't implement both
lighting and texture mapping on objects - it is one or the other.
The current rendering state of the triangle (or object) is determined by
the following call:

  unsigned int  *tex = dev->GetCurrentTexture();

If the function returns 0 (a null pointer), there is no texture state bound
to the currently-drawing triangle. This means that the object is lit but not
texture mapped. Otherwise if the function returns an address, the object
is texture mapped but not lit. The texture image is buffered in memory as
a sequence of 32-bit unsigned integral values in xxRRGGBB format which
can be directly written to the colorbuffer without any transformations.
The width and height of the texture image can be determined by the calls:

 unsigned int  tW = dev->GetCurrentTextureWidth();
 unsigned int  tH = dev->GetCurrentTextureHeight();

NOTE: Assume that GL_REPEAT is the default wrapping mode for both s and t
texture coordinates.
-------------------------------------------------------------------------------

Testing depth buffer state of the pipe:
Users can choose to turn on or off the depth buffering algorithm. The current
state of the pipe can be determined by the following call:

  bool          wdepth    = dev->DepthTestEnabled();

If DepthTestEnabled() returns 0, depth buffering is disabled, otherwise
it is enabled.
-------------------------------------------------------------------------------

Which interpolation scheme is to be used by the rasterizer to interpolate
vertex color and texture coordinates?
Since users can set the graphics pipe's to render scenes using linear or
hyperbolic interpolation, your rasterizer must ideally implement both these
interpolation schemes. However, since hyperbolic interpolation was not covered
in the lectures, you're only required to implement linear interpolation of
vertex attributes using the barycentric interpolation scheme discussed in lectures.
The current interpolation state of the graphics pipe
can be determined by the call:

  gfxInterpMode im = dev->GetInterpMode();

Enumeration type gfxInterpMode is declared in GraphicsPipe.h with
enumeration constants: GFX_LINEAR_INTERP or GFX_HYPERBOLIC_INTERP.
-------------------------------------------------------------------------------

How is the front colorbuffer accessed?
The dimensions of the colorbuffer are specified by the user and have values
determined by the read-only sWindowWidth and sWindowHeight variables defined
at file-scope in main.cpp. The calls

  unsigned int  *frameBuf = dev->GetFrameBuffer();
  size_t        w         = dev->GetWidth();
  size_t        h         = dev->GetHeight();

returns the address of the first element, the width, and the height of the
of the colorbuffer. The colorbuffer is a linear array of 32-bit unsigned
integral values. The pixel form is xxRRGGBB - the most significant 8 bits
are unused. To write to this colorbuffer, you must convert the normalized
color components in range [0.f, 1.f] from the interpolator into values in
range [0, 255] and pack the components into a 32-bit integral value with
format xxRRGGBB.
-------------------------------------------------------------------------------

How is the depthbuffer accessed?
The depthbuffer's dimensions are exactly the same as the colorbuffer. The call

  float         *depthBuf = dev->GetDepthBuffer();

returns the address of the first element of the depthbuffer. As can be seen the
depthbuffer stores depthvalues in the range [0.f, 1.f]. The depthbuffer
has the same dimensions as the colorbuffer.
-------------------------------------------------------------------------------

What are the coordinate conventions of the colorbuffer, depthbuffer and
texels in the texture image?
Memory for these buffer is allocated by Windows which is also used to blit
the contents of the colorbuffer to the display device's video memory.
Windows defines the colorbuffer with the convention that the origin is at
the upper-left corner. This means that assigning the value 0x00ff0000 to
frameBuf[0] will paint the upper-left corner with red color. Reading a
depth value from the first location of the depthbuffer provides the depth
value of the upper-left corner. Simiarly, reading a texel from the first
location of the texture image will return the texel associated with the
upper-left corner. However, the graphics pipe simulates OpenGL and therefore
generates device coordinates with the bottom-left corner as the origin. All of
this means that it is your responsibility to map the OpenGL device (or viewport
or window) coordinates from the bottom-left corner to upper-left corner.
*/
{
  //@todo Implement me.
  return;
}


/*  _________________________________________________________________________ */
void YourRasterizer::
DrawPoint(gfxGraphicsPipe *dev, gfxVertex const&	v0)
  /*! Render a device frame point.

      @param dev -->  Pointer to the pipe to render to.
      @param v0  -->  Vertex with device frame coordinates previously computed.

      The framebuffer is a linear array of 32-bit pixels.
      fb[y * width + x] will access a pixel at (x, y). The
      pixel format is xxRRGGBB (thus the shifting that's
      going on to pack the color components into a single
      32-bit pixel value.
  */
{
  //@todo Implement me.
  return;
}

/*  _________________________________________________________________________ */
void YourRasterizer::
DrawLine(gfxGraphicsPipe *dev, gfxVertex const&	v0, gfxVertex const&	v1)
/*! Render a line segment between 2 device frame points.

    @param dev -->  Pointer to the pipe to render to.
    @param v0  -->  First vertex.
    @param v1  -->  Second vertex.
*/
{
  //@todo Implement me.
  return;
}

/*  _________________________________________________________________________ */
void YourRasterizer::
DrawWireframe(gfxGraphicsPipe* dev,
              gfxVertex const& v0,
              gfxVertex const& v1,
              gfxVertex const& v2)
/*! Render a triangle in wireframe mode using 3 device-frame points.

    @param dev -->  Pointer to the pipe to render to.
    @param v0  -->  First vertex.
    @param v1  -->  Second vertex.
    @param v2  -->  Third vertex.
*/
{
  DrawLine(dev, v0, v1);
  DrawLine(dev, v1, v2);
  DrawLine(dev, v2, v0);
}

/*                                                        first-person camera
                                                    (was YourCamera.cpp)
----------------------------------------------------------------------------- */

/*  _________________________________________________________________________ */
void YourCamera::
Move(gfxVector3 const& p)
/*! Displace the camera along its basis vectors.

    @param p -->  Displacement vector such that p.x, p.y, and p.z are
									displacements along the camera's side, up, and viewing
									basis vectors.
*/
{
	//@todo
/*
	Displace the camera so that it is always following the planar floor but
	can look up or down. This behavior is unlike a flight simulator where the
	camera moves in the direction of the view vector.
	Check out the sample application to make sure you understand the basic mechanics
	of the required camera.
*/
   //@todo Implement me.
}

/*  _________________________________________________________________________ */
void YourCamera::
Move(float x, float y, float z)
/*! Displace the camera along its basis vectors.

    @param x -->  Displacement along camera's side (X) axis.
    @param y -->  Displacement along camera's up (Y) axis.
    @param z -->  Displacement along camera's view (-Z) axis.
*/
{
/*
	Displace the camera such it is always following the planar floor but
	can look up or down. This behavior is unlike a flight simulator where the
	camera moves in the direction of the view vector.
	Check out the sample application to make sure you understand the basic mechanics
	of the required camera.
*/
  //@todo Implement me.
}

/*  _________________________________________________________________________ */
void YourCamera::
UpdateSphericalFromPoints()
/*! Updates camera's spherical coordinates using updated
		camera position mFrom and camera target mAt.
*/
{
  //@todo Implement me.
}

/*  _________________________________________________________________________ */
void YourCamera::
UpdatePointsFromSpherical()
/*! Updates camera's target position mAt using camera's
		updated spherical coordinates.
*/
{
  //@todo Implement me.
}
