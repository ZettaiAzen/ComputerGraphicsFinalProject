/*!
@file    submit.cpp
@author  Prasanna Ghali       (pghali@digipen.edu)
@co-author  Parminder Singh   (parminder.singh@digipen.edu)
            Jolin Wong Xinhui (jolinxinhui.w@digipen.edu)

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

    // we need to send in 6 planes...

    /*
    On the other hand, OpenGL and this framework store 4 x 4 matrix elements
        using an array of 16 elements which are stored in memory using column - major
        order.Therefore, the elements of the 4 x 4 matrix M are stored in array
        mMat[16] as follows :

    | mMat[0]		mMat[4]		mMat[8]		mMat[12]|
        |																				|
        | mMat[1]		mMat[5]		mMat[9]		mMat[13]|
        |																				|
        | mMat[2]		mMat[6]		mMat[10]	mMat[14]|
        |																				|
        | mMat[3]		mMat[7]		mMat[11]	mMat[15] |

    */

    gfxVector4 r0 = perspective_mtx.GetRow4(0);
    gfxVector4 r1 = perspective_mtx.GetRow4(1);
    gfxVector4 r2 = perspective_mtx.GetRow4(2);
    gfxVector4 r3 = perspective_mtx.GetRow4(3);

    gfxFrustum mFrustum{};

    // might have to double check these? unsure
    gfxVector4 left = (-1)*r0 - r3;
    gfxVector4 right = r0 - r3;
    gfxVector4 bottom = (-1) * r1 - r3;
    gfxVector4 top = r1 - r3;
    gfxVector4 nnear = (-1) * r2 - r3;
    gfxVector4 ffar = r2 - r3;


    auto normalizePlane = [](gfxVector4& vec) {
        gfxVector3 temp{ vec.x, vec.y, vec.z };
        float length = temp.Length();
        
        vec.x /= length;
        vec.y /= length;
        vec.z /= length;
        vec.w /= length;
        };


    // normalise all of them before sending it in
    normalizePlane(left);
    normalizePlane(right);
    normalizePlane(bottom);
    normalizePlane(top);
    normalizePlane(nnear);
    normalizePlane(ffar);

    mFrustum.l = gfxPlane{ left.x, left.y, left.z, left.w };
    mFrustum.r = gfxPlane{ right.x, right.y, right.z, right.w };
    mFrustum.b = gfxPlane{ bottom.x, bottom.y, bottom.z, bottom.w };
    mFrustum.t = gfxPlane{ top.x, top.y, top.z, top.w };
    mFrustum.n = gfxPlane{ nnear.x, nnear.y, nnear.z, nnear.w };
    mFrustum.f = gfxPlane{ ffar.x, ffar.y, ffar.z, ffar.w };

    return (mFrustum);
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

    // check for each side in this order: LRBTNF
    *ptr_outcode = (int)0; // negate everything first

    // we can use a for loop
    for (int i = 0; i < 6; ++i) {
        float dist = (frustum.mPlanes[i].a * bounding_sphere.center.x) + (frustum.mPlanes[i].b * bounding_sphere.center.y) + (frustum.mPlanes[i].c * bounding_sphere.center.z) + (frustum.mPlanes[i].d * 1);
        if (dist > bounding_sphere.radius) { 
            // cull it as it is too far
            return true;
        }
        else { // it is straddling the line
            if (std::abs(dist) < bounding_sphere.radius) {
                *ptr_outcode |= ((int)(1) << (i)); // adjust outcode accordingly
            }
        }
    }

    return false;

  //*ptr_outcode = 0; // for now, assume bounding sphere completely inside frustum
  /*return (false);	*/  // for now, assume object is not trivially rejected
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
  //@todo Implement me. :elephant:
   
    // i dont know if the outcode being 0 will come here??? but ill check incase
    // trivially accepted for bounding sphere
    if (outcode == 0) {
        // pass it as it is
        return vertex_buffer;
    }

    // now we check for trivially rejected / trivially accepted vertexes
    gfxOutCode resultReject{ vertex_buffer[0].oc }; // initialise first
    gfxOutCode resultAccept{ vertex_buffer[0].oc }; 

    for (int i = 1; i < vertex_buffer.size(); ++i) {
        resultReject &= vertex_buffer[i].oc;
        resultAccept |= vertex_buffer[i].oc;
    }

    if (resultReject != 0) {
        return gfxVertexBuffer{}; // empty!!! reject
    }
    
    if (resultAccept == 0) {
        return vertex_buffer; // pass it as it is
    }

    // now we go through clipper stage
    gfxVertexBuffer m_vertexBuffer{vertex_buffer};
    for (int y = 0; y < 6; ++y) {
        // see if the outcode is out for the respective stage
        if (outcode & ((unsigned int)1 << (y))) { // this means it needs to be clipped
            // loop through the vertices

            gfxVertexBuffer side_buffer{};
            for (size_t i = 0; i < m_vertexBuffer.size(); ++i) {
                gfxVertex vec0 = m_vertexBuffer[i];
                gfxVertex vec1 = m_vertexBuffer[(i + 1)%m_vertexBuffer.size()];

                // four different kinds, IN IN, OUT OUT, IN OUT, OUT IN
                // using only IN and OUT vertices not OUT and IN

                // checking for out out in which we skip
                if (vec0.bc[y] > 0 && vec1.bc[y] > 0) { // they are out
                    continue;
                }

                if (vec0.bc[y] <= 0 && vec1.bc[y] <= 0) { // they are inside
                    side_buffer.push_back(vec1);
                    continue;
                } 
                
                bool INOUT_bool = (vec0.bc[y] <= 0 && vec1.bc[y] > 0);

                gfxVertex in{};
                gfxVertex out{};

                if (INOUT_bool) {
                    in = vec0;
                    out = vec1;
                }
                else {
                    in = vec1;
                    out = vec0;
                }

                float t = in.bc[y] / (in.bc[y] - out.bc[y]);
                gfxVertex midpoint{ in };

                midpoint.x_c += t * (out.x_c - in.x_c); midpoint.y_c += t * (out.y_c - in.y_c); midpoint.z_c += t * (out.z_c - in.z_c); midpoint.w_c += t * (out.w_c - in.w_c);
                midpoint.s += t * (out.s - in.s); midpoint.t += t * (out.t - in.t);
                midpoint.r += t * (out.r - in.r); midpoint.g += t * (out.g - in.g); midpoint.b += t* (out.b - in.b); midpoint.a += t* (out.a - in.a);

                midpoint.oc = (unsigned int)0;
                for (int z = 0; z < 6; ++z) {
                    midpoint.bc[z] += t * (out.bc[z] - in.bc[z]);
                    midpoint.oc |= midpoint.bc[z] > 0 ? ((unsigned int)1 << z) : 0;
                }

                if (INOUT_bool) {
                    // check for the size of the side buffer first, if its empty then u put in two if not just put in the new point (+1)
                    side_buffer.push_back(midpoint);
                }
                else {
                    // out to in
                    side_buffer.push_back(midpoint);
                    side_buffer.push_back(in);
                }
            }
            m_vertexBuffer = side_buffer;
        }
    }


    if (m_vertexBuffer.size() < 3) { // invalid ?
        return gfxVertexBuffer{};
    }

    gfxVertexBuffer modifiedVertex{};
    // change the quad into triangles, using the first vertex as the pivot/source
    for (size_t i = 0; i < m_vertexBuffer.size() - 2; ++i) {
        modifiedVertex.push_back(m_vertexBuffer[0]);
        // ^ pivot
        // add the next two vertices for triangle
        modifiedVertex.push_back(m_vertexBuffer[i + 1]);
        modifiedVertex.push_back(m_vertexBuffer[i + 2]);
    }

  return (modifiedVertex);
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
  
  // check for valid triangle via signed area via cross product
  
  // area = (x1-x0)(y2-y0) - (x2-x0)(y1-y0)
  float area = (v1.x_d - v0.x_d) * (v2.y_d - v0.y_d) - (v2.x_d - v0.x_d)*(v1.y_d - v0.y_d);
  if (area <= 0) { // if area is 0 or -ve, reject
      return;
  }

  // if it is fine, calculate all the edge equations

  // bounding box, get min and max xy values
  int xmin = static_cast<int>(std::floor(std::min({ v0.x_d, v1.x_d, v2.x_d })));
  int xmax = static_cast<int>(std::ceil(std::max({ v0.x_d, v1.x_d, v2.x_d })));

  int ymin = static_cast<int>(std::floor(std::min({ v0.y_d, v1.y_d, v2.y_d })));
  int ymax = static_cast<int>(std::ceil(std::max({ v0.y_d, v1.y_d, v2.y_d })));

  // c -> P0.x*P1.y - P1.x*P0.y
  gfxVector3 l0{(v1.y_d - v2.y_d), (v2.x_d - v1.x_d),(v1.x_d * v2.y_d - v2.x_d * v1.y_d) };  // it is opposite v0, v2-v1
  gfxVector3 l1{ (v2.y_d - v0.y_d), (v0.x_d - v2.x_d),(v2.x_d * v0.y_d - v0.x_d * v2.y_d) }; // opposite v1,  v0-v2
  gfxVector3 l2{ (v0.y_d - v1.y_d), (v1.x_d - v0.x_d), (v0.x_d*v1.y_d - v1.x_d*v0.y_d)}; // opposite v2,  v1-v0


  // checking for top left

  // if a != 0, if a is +ve it means y coords are moving down (left edge)
  // if a = 0, horizontal line, if b is neg it means moving right to left (x coord), top edge
  bool tl0 = (l0.x != 0) ? (l0.x > 0.0 ? true : false) : (l0.y < 0.0 ? true : false);
  bool tl1 = (l1.x != 0) ? (l1.x > 0.0 ? true : false) : (l1.y < 0.0 ? true : false); 
  bool tl2 = (l2.x != 0) ? (l2.x > 0.0 ? true : false) : (l2.y < 0.0 ? true : false);

  auto dotProduct = [](gfxVector3 const& v0, gfxVector3 const& v1) -> float{
      return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
  };


  // precompute the a, b ,c
  float eval0{dotProduct(l0, gfxVector3{xmin+0.5f, ymin+0.5f, 1})};
  float eval1{ dotProduct(l1, gfxVector3{xmin + 0.5f, ymin + 0.5f, 1}) };
  float eval2{ dotProduct(l2, gfxVector3{xmin + 0.5f, ymin + 0.5f, 1}) };

  // checking for texture
  unsigned int* tex = dev->GetCurrentTexture();

  // checking for depth test
  bool wdepth = dev->DepthTestEnabled();
  float* depthBuf{};
  size_t width{};
  size_t height{};

  if (wdepth) {
      depthBuf = dev->GetDepthBuffer();
      width = dev->GetWidth(); // it has the same dimensions as the colour buffer
      height = dev->GetHeight();
  }

  // now we start the loop
  for(int y{ymin} ; y < ymax; ++y){
    float heval0{ eval0 };
    float heval1{ eval1 };
    float heval2{ eval2 };

    for (int x{ xmin }; x < xmax; ++x) {

        // check for valid point in line
        // this is not a valid line

        bool in_l0 = (heval0 > 0 || (heval0 == 0 && tl0));
        bool in_l1 = (heval1 > 0 || (heval1 == 0 && tl1));
        bool in_l2 = (heval2 > 0 || (heval2 == 0 && tl2));

        if (!(in_l0 && in_l1 && in_l2)){
            
            // move to the next point

            heval0 += l0.x;
            heval1 += l1.x;
            heval2 += l2.x;
            continue;
        }

        // weights
        float a = heval0 / area;
        float b = heval1 / area;
        float c = heval2 / area;

        gfxVertex m_vertex{};

        m_vertex.x_d = static_cast<float>(x);
        m_vertex.y_d = static_cast<float>(y);

        // if there is texture, calculate that 
        if (tex) {
            float s = (a * (v0.s) + b * (v1.s) + c * (v2.s));
            float t = (a * (v0.t) + b * (v1.t) + c * (v2.t));
            m_vertex.s = s;
            m_vertex.t = t;
        }
        else {
            // calculate color
            float red = (a * (v0.r) + b * (v1.r) + c * (v2.r));
            float green = (a * (v0.g) + b * (v1.g) + c * (v2.g));
            float blue = (a * (v0.b) + b * (v1.b) + c * (v2.b));
            float alpha = (a * (v0.a) + b * (v1.a) + c * (v2.a));

            m_vertex.r = red;
            m_vertex.g = green;
            m_vertex.b = blue;
            m_vertex.a = alpha;
        }

        // send that to be drawn

        if (wdepth) {
            
            float depth = (a * (v0.z_d) + b * (v1.z_d) + c * (v2.z_d));
            int flipped_y = static_cast<int>(height - 1 - y);
            if (depthBuf[flipped_y * width + x] > depth) {
                DrawPoint(dev, m_vertex);
                depthBuf[flipped_y * width + x] = depth;
            }
        }
        else {
            DrawPoint(dev, m_vertex);
        }

        heval0 += l0.x;
        heval1 += l1.x;
        heval2 += l2.x;
    }

    eval0 += l0.y;
    eval1 += l1.y;
    eval2 += l2.y;
  }



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

    unsigned int* framebuffer = dev->GetFrameBuffer();
    if (!framebuffer) { // just incase??
        return;
    }

    int width = static_cast<int>(dev->GetWidth()); // it has the same dimensions as the colour buffer
    int height = static_cast<int>(dev->GetHeight());

    // check if x y coords are even in the range

   /* int x = static_cast<int>(v0.x_d);
    int y = static_cast<int>(v0.y_d);*/

    float flipped_y = height - v0.y_d - 1.f;

    if ((flipped_y >= height || flipped_y < 0) || (v0.x_d >= width || v0.x_d < 0)) {
        return; // skip this point
    }

    // i need to check if im using colours or texture here
    // set framebuffer to colour

    // check if im doing colour or texture
    unsigned int* tex = dev->GetCurrentTexture();

    if (tex) {
        // doing texture
        // wrapping because of GL_REPEAT
        float floor_s = v0.s - std::floor(v0.s);
        float floor_t = v0.t - std::floor(v0.t);

        // we need to scale to the texture coords now
        int  tW = static_cast<int>(dev->GetCurrentTextureWidth());
        int  tH = static_cast<int>(dev->GetCurrentTextureHeight());

        float scaled_s = (tW * floor_s);
        float scaled_t = (tH * floor_t);

        // clamping just in case of rounding errors
        scaled_s = std::clamp(scaled_s, 0.f, (tW - 1.f));
        scaled_t = std::clamp(scaled_t, 0.f, (tH - 1.f));

        float flipped_t = tH - 1 - scaled_t;

        int y = static_cast<int>(std::floor(flipped_y));
        int x = static_cast<int>(std::floor(v0.x_d));
        int s = static_cast<int>(scaled_s);
        int t = static_cast<int>(scaled_t);

        // check for valid
        if ((flipped_t < tH && flipped_t >= 0) && (scaled_s < tW && scaled_s >= 0)) {
            framebuffer[y * width + x] = tex[t * tW + s];
        }

    }
    else {
        // we are doing colours
        unsigned int m_r = static_cast<int>(v0.r * 255);
        unsigned int m_g = static_cast<int>(v0.g * 255);
        unsigned int m_b = static_cast<int>(v0.b * 255);

        // clamping just in case
        m_r = std::clamp(m_r, (unsigned int)0, (unsigned int)255);
        m_g = std::clamp(m_g, (unsigned int)0, (unsigned int)255);
        m_b = std::clamp(m_b, (unsigned int)0, (unsigned int)255);

        uint32_t clr{};
        clr |= m_r << 16;
        clr |= m_g << 8;
        clr |= m_b;

        int y = static_cast<int>(std::floor(flipped_y));
        int x = static_cast<int>(std::floor(v0.x_d));

        framebuffer[y * width + x] = clr;

    }

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

    // need to see which direction we are going and the line dydx
    int dx = static_cast<int>(v1.x_d - v0.x_d);
    int dy = static_cast<int>(v1.y_d - v0.y_d);

    bool isSwapped{ std::abs(dy) > std::abs(dx) };

    int x, y, xstep, ystep, d, dmaj, dmin;

    x = static_cast<int>(v0.x_d);
    y = static_cast<int>(v0.y_d);

    dx = static_cast<int>(v1.x_d - v0.x_d);
    dy = static_cast<int>(v1.y_d - v0.y_d);

    xstep = (dx < 0) ? -1 : 1;
    ystep = (dy < 0) ? -1 : 1;

    dx = (dx < 0) ? -dx : dx;
    dy = (dy < 0) ? -dy : dy;

    if (isSwapped) {
        std::swap(x, y);
        std::swap(xstep, ystep);
        std::swap(dx, dy);
    }

    d = (2 * dy) - dx;
    dmaj = (2 * dy) - (2 * dx);
    dmin = (2 * dy);

    // checking for depth test
    bool wdepth = dev->DepthTestEnabled();

    size_t width = dev->GetWidth(); // it has the same dimensions as the colour buffer
    size_t height = dev->GetHeight();

    int flipped_y{ static_cast<int>(height - 1 - v0.y_d) };
    int real_x{ static_cast<int>(v0.x_d) };

    if (flipped_y < height && real_x < width) {
        if (wdepth) {
            float* depthBuf = dev->GetDepthBuffer();

            if (depthBuf[flipped_y * width + real_x] > v0.z_d) {
                DrawPoint(dev, v0);
                depthBuf[flipped_y * width + real_x] = v0.z_d;
            }
        }
        else {
            DrawPoint(dev, v0);
        }
    }

    // i guess we have to interpolate ? ? for z value i guess..

    for (int i = 1; i < dx; ++i) {
        if (d > 0) { // we move!
            d += dmaj;
            y += ystep;
        }
        else {
            d += dmin;
        }
        x += xstep;

        gfxVertex m_vertex{v0};
        float t = i / (float)dx;

        // checking for colour or depth

        m_vertex.r = 0.f;
        m_vertex.g = 0.f;
        m_vertex.b = 0.f;
        m_vertex.a = 0.f;

        if (wdepth) {
            float* depthBuf = dev->GetDepthBuffer();
            size_t width = dev->GetWidth(); // it has the same dimensions as the colour buffer
            size_t height = dev->GetHeight();

            m_vertex.z_d += t * (v1.z_d - v0.z_d);
            
            int flipped_y{};
            int real_x{};

            if (isSwapped) {
                flipped_y = static_cast<int>(height - 1 - x);
                real_x = y;
            }
            else {
                flipped_y = static_cast<int>(height - 1 - y);
                real_x = x;
            }
            if (flipped_y < height && real_x < width) {
                if (depthBuf[flipped_y * width + real_x] > m_vertex.z_d) {
                    if (isSwapped) {
                        m_vertex.x_d = static_cast<float>(y);
                        m_vertex.y_d = static_cast<float>(x);
                        DrawPoint(dev, m_vertex); // to look at
                    }
                    else {

                        m_vertex.x_d = static_cast<float>(x);
                        m_vertex.y_d = static_cast<float>(y);
                        DrawPoint(dev, m_vertex); // to look at
                    }
                    depthBuf[flipped_y * width + real_x] = m_vertex.z_d;
                }
            }
            
        }
        else {
            if (isSwapped) {

                m_vertex.x_d = static_cast<float>(y);
                m_vertex.y_d = static_cast<float>(x);
                DrawPoint(dev, m_vertex); // to look at
            }
            else {

                m_vertex.x_d = static_cast<float>(x);
                m_vertex.y_d = static_cast<float>(y);
                DrawPoint(dev, m_vertex); // to look at
            }
        }
    }
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
    gfxVector3 bx, by, bz;

    // get the basis vectors
    GetBasisVectors(&bx, &by, &bz);

    gfxVector3 xDisp = -p.x * bx;
    gfxVector3 yDisp = p.y * by;

    // for z, which is the look at axis, we have to ignore the y value because moving is parallel to the floor
    bz.y = 0.f;
    bz.Normalize();
    gfxVector3 zDisp = -p.z * bz;

    // adjust the position of the camera with these displacement vectors
    mFrom += (xDisp + yDisp + zDisp);
    mAt += (xDisp + yDisp + zDisp);

    
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

    Move(gfxVector3(x, y, z));

}

/*  _________________________________________________________________________ */
void YourCamera::
UpdateSphericalFromPoints()
/*! Updates camera's spherical coordinates using updated
		camera position mFrom and camera target mAt.
*/
{
  //@todo Implement me.

    //!< Distance between camera at and target positions.
                                                        //!< Here, we are specifying the radius of a sphere
                                                        //!< centered at mFrom and mAt is a point on the surface
                                                        //!< of the sphere.
    gfxVector3 radiusVec = mAt - mFrom;
    mRadius = radiusVec.Length();

    //!< Latitude angle alpha [-pi/2..pi/2] where 
                                                        //!< (pi/2 - alpha) radians is the angle between the view-vector
                                                        //!< and up axis (0, 1, 0). When alpha is pi/2, the view-vector
                                                        //!< is oriented along (0, 1, 0) and when alpha is -pi/2, the
                                                        //!< view-vector is oriented along (0, -1, 0).
                                                        //!< See class notes for pictures.
    mLatitude = std::asin(radiusVec.y / mRadius);

    //!< Azimuth angle beta [0..2pi] where beta is the angle between
                                                        //!< the view vector projected onto the ZX-plane and z-axis (0, 0, 1).
                                                        //!< beta increases from 0 to 2pi radians in counter-clockwise direction
                                                        //!< when viewed from a point on up axis looking down along negative up axis.
                                                        //!< See class presentations for pictures.
    auto arctan = [](float pi, float pk) -> float {
        if (pk != 0 && pi >= 0) {
            return std::atan2(pi, pk);
        }

        if (pk == 0 && pi > 0) {
            return PI / 2.f;
        }

        if(pk != 0 && pi < 0){
            return (2 * PI + std::atan2(pi, pk));
        }

        if (pk == 0 && pi < 0) {
            return (3 * PI) / 2.f;
        }

        return 0.f;
    };

    mAzimuth = arctan(radiusVec.x, radiusVec.z);


}

/*  _________________________________________________________________________ */
void YourCamera::
UpdatePointsFromSpherical()
/*! Updates camera's target position mAt using camera's
		updated spherical coordinates.
*/
{
  //@todo Implement me.

    gfxVector3 vec{};
    vec.x = mRadius * std::cos(mLatitude) * std::sin(mAzimuth);
    vec.y = mRadius * std::sin(mLatitude);
    vec.z = mRadius * std::cos(mLatitude) * std::cos(mAzimuth);

    vec += mFrom;

    mAt = vec;
}
