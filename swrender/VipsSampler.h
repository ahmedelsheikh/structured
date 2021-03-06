/* * structured - Tools for the Generation and Visualization of Large-scale
 * Three-dimensional Reconstructions from Image Data. This software includes
 * source code from other projects, which is subject to different licensing,
 * see COPYING for details. If this project is used for research see COPYING
 * for making the appropriate citations.
 * Copyright (C) 2013 Matthew Johnson-Roberson <mattkjr@gmail.com>
 *
 * This file is part of structured.
 *
 * structured is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * structured is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with structured.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VIPSSAMPLER_H
#define VIPSSAMPLER_H
#include <vips/vips>
#include <vips/vips.h>
#include <osg/Vec3>
#include <osg/Vec2>
#include "render_utils.h"
#include <map>

/// Sampler that captures positions and normals.
class VipsSampler
{
    typedef std::map<std::pair<int,int>,int > dcm_t;

public:
    typedef unsigned int uint;

    VipsSampler(vips::VImage &img) :m_img(img){mipmapL[0]=0;
                                               mipmapL[1]=2;mipmapL[2]=4;};
    static void renderDepthTriCallback(void * param, int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    static void renderBlendedTriCallback(void * param, int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    static void sampleQuadCallback(void * param, int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    static void renderVarTriCallback(void * param, int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    static void renderTriCallback(void * param, int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);

    void setCurrentFace(uint vertexCount, const osg::Vec3 * positions, const osg::Vec3 * normals);
    void depthPassTri(int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    void blendVarTri(int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    void renderTri(int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);

    void blendPassTri(int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    void sampleQuad(int x, int y, const osg::Vec3& bar, const osg::Vec3& dx, const osg::Vec3& dy, float coverage);
    REGION *regOutput;
    REGION *regRange;
    dcm_t *doublecountmapPtr;
    int triIdx;
    TextureMipMap *texture;
    TextureMipMap **inCoreTextureArr;
    Texture *texture_no_mm;

    int idx;

private:
    static const float rmax=0.70710678;
    int mipmapL[3];
    vips::VImage & m_img;

    uint m_currentVertexCount;
    const osg::Vec3 * m_normals;
    const osg::Vec3 * m_positions;
    osg::Vec3 m_midedgenormals[5];
};
extern Rect gRect;

#define NV_EPSILON			(0.0001f)
inline bool isZero(const float f, const float epsilon = NV_EPSILON)
{
    return fabs(f) <= epsilon;
}
inline osg::Vec3 normalizeSafe(const osg::Vec3& v, const osg::Vec3& fallback, float epsilon = NV_EPSILON)
{
    float l = v.length();
    if (isZero(l, epsilon)) {
        return fallback;
    }
    return (v* (1.0f / l));
}
#endif // VIPSSAMPLER_H
