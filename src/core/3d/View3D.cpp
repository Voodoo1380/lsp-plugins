/*
 * View.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <core/3d/View3D.h>

namespace lsp
{
    View3D::View3D()
    {
    }
    
    View3D::~View3D()
    {
    }

    void View3D::clear(size_t flags)
    {
        if (flags & V3D_VERTEXES)
            vVertexes.flush();
        if (flags & V3D_RAYS)
            vRays.flush();
        if (flags & V3D_SEGMENTS)
            vSegments.flush();
        if (flags & V3D_POINTS)
            vPoints.flush();
    }

    bool View3D::add_ray(const v_ray3d_t *r)
    {
        return vRays.append(r);
    }

    bool View3D::add_point(const v_point3d_t *p)
    {
        return vPoints.append(p);
    }

    bool View3D::add_segment(const v_segment3d_t *s)
    {
        return vSegments.append(s);
    }

    bool View3D::add_segment(const rt_edge_t *s, const color3d_t *c)
    {
        v_segment3d_t xs;
        xs.p[0]     = *(s->v[0]);
        xs.p[1]     = *(s->v[1]);
        xs.c[0]     = *c;
        xs.c[1]     = *c;

        return vSegments.append(&xs);
    }

    bool View3D::add_segment(const rt_edge_t *s, const color3d_t *c1, const color3d_t *c2)
    {
        v_segment3d_t xs;
        xs.p[0]     = *(s->v[0]);
        xs.p[1]     = *(s->v[1]);
        xs.c[0]     = *c1;
        xs.c[1]     = *c2;

        return vSegments.append(&xs);
    }

    bool View3D::add_segment(const rt_vertex_t *p1, const rt_vertex_t *p2, const color3d_t *c)
    {
        v_segment3d_t xs;
        xs.p[0]     = *p1;
        xs.p[1]     = *p2;
        xs.c[0]     = *c;
        xs.c[1]     = *c;

        return vSegments.append(&xs);
    }

    bool View3D::add_triangle(const v_vertex3d_t *vi)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0] = vi[0];
        v[1] = vi[1];
        v[2] = vi[2];

        return true;
    }

    bool View3D::add_triangle_1c(const v_triangle3d_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t->p[0];
        v[0].n      = t->n[0];
        v[0].c      = *c;

        v[1].p      = t->p[1];
        v[1].n      = t->n[1];
        v[1].c      = *c;

        v[2].p      = t->p[2];
        v[2].n      = t->n[2];
        v[2].c      = *c;

        return true;
    }

    bool View3D::add_triangle_pvnc1(const point3d_t *t, const vector3d_t *n, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t[0];
        v[0].n      = *n;
        v[0].c      = *c;

        v[1].p      = t[1];
        v[1].n      = *n;
        v[1].c      = *c;

        v[2].p      = t[2];
        v[2].n      = *n;
        v[2].c      = *c;

        return true;
    }

    bool View3D::add_triangle_pvnc3(const point3d_t *t, const vector3d_t *n, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t[0];
        v[0].n      = *n;
        v[0].c      = *c0;

        v[1].p      = t[1];
        v[1].n      = *n;
        v[1].c      = *c1;

        v[2].p      = t[2];
        v[2].n      = *n;
        v[2].c      = *c2;

        return true;
    }

    bool View3D::add_triangle_3c(const v_triangle3d_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t->p[0];
        v[0].n      = t->n[0];
        v[0].c      = *c0;

        v[1].p      = t->p[1];
        v[1].n      = t->n[1];
        v[1].c      = *c1;

        v[2].p      = t->p[2];
        v[2].n      = t->n[2];
        v[2].c      = *c2;

        return true;
    }

    bool View3D::add_triangle_3c(const obj_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = *(t->n[0]);
        v[0].c          = *c0;

        v[1].p          = *(t->v[1]);
        v[1].n          = *(t->n[1]);
        v[1].c          = *c1;

        v[2].p          = *(t->v[2]);
        v[2].n          = *(t->n[2]);
        v[2].c          = *c2;

        return true;
    }

    bool View3D::add_triangle_1c(const obj_triangle_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = *(t->n[0]);
        v[0].c          = *c;

        v[1].p          = *(t->v[1]);
        v[1].n          = *(t->n[1]);
        v[1].c          = *c;

        v[2].p          = *(t->v[2]);
        v[2].n          = *(t->n[2]);
        v[2].c          = *c;

        return true;
    }

    bool View3D::add_triangle_3c(const rt_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = t->n;
        v[0].c          = *c0;

        v[1].p          = *(t->v[1]);
        v[1].n          = t->n;
        v[1].c          = *c1;

        v[2].p          = *(t->v[2]);
        v[2].n          = t->n;
        v[2].c          = *c2;

        return true;
    }

    bool View3D::add_triangle_1c(const rt_triangle_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = t->n;
        v[0].c          = *c;

        v[1].p          = *(t->v[1]);
        v[1].n          = t->n;
        v[1].c          = *c;

        v[2].p          = *(t->v[2]);
        v[2].n          = t->n;
        v[2].c          = *c;

        return true;
    }

    bool View3D::add_plane_pv1c(const point3d_t *t, const color3d_t *c)
    {
        v_ray3d_t *r = vRays.append();
        if (r == NULL)
            return false;
        v_segment3d_t *v = vSegments.append_n(6);
        if (v == NULL)
        {
            vRays.remove_last();
            return false;
        }

        v[0].p[0]   = t[0];
        v[1].p[0]   = t[1];
        v[2].p[0]   = t[2];

        v[0].p[1]   = t[1];
        v[1].p[1]   = t[2];
        v[2].p[1]   = t[0];

        v[0].c[0]   = *c;
        v[0].c[1]   = *c;
        v[1].c[0]   = *c;
        v[1].c[1]   = *c;
        v[2].c[0]   = *c;
        v[2].c[1]   = *c;

        point3d_t mp[3];
        mp[0].x     = 0.5f * (t[1].x + t[2].x);
        mp[0].y     = 0.5f * (t[1].y + t[2].y);
        mp[0].z     = 0.5f * (t[1].z + t[2].z);

        mp[1].x     = 0.5f * (t[2].x + t[0].x);
        mp[1].y     = 0.5f * (t[2].y + t[0].y);
        mp[1].z     = 0.5f * (t[2].z + t[0].z);

        mp[2].x     = 0.5f * (t[0].x + t[1].x);
        mp[2].y     = 0.5f * (t[0].y + t[1].y);
        mp[2].z     = 0.5f * (t[0].z + t[1].z);

        v[3].p[0]   = t[0];
        v[4].p[0]   = t[1];
        v[5].p[0]   = t[2];

        v[3].p[1]   = mp[0];
        v[4].p[1]   = mp[1];
        v[5].p[1]   = mp[2];

        v[3].c[0]   = *c;
        v[3].c[1]   = *c;
        v[4].c[0]   = *c;
        v[4].c[1]   = *c;
        v[5].c[0]   = *c;
        v[5].c[1]   = *c;

        r->p.x      = (t[0].x + t[1].x + t[2].x)/ 3.0f;
        r->p.y      = (t[0].y + t[1].y + t[2].y)/ 3.0f;
        r->p.z      = (t[0].z + t[1].z + t[2].z)/ 3.0f;
        r->p.w      = 1.0f;

        r->c        = *c;

        dsp::calc_normal3d_pv(&r->v, t);

        return true;
    }

    bool View3D::add_triangle_pv1c(const point3d_t *pv, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        vector3d_t n;
        dsp::calc_normal3d_pv(&n, pv);

        v[0].p      = pv[0];
        v[0].n      = n;
        v[0].c      = *c;

        v[1].p      = pv[1];
        v[1].n      = n;
        v[1].c      = *c;

        v[2].p      = pv[2];
        v[2].n      = n;
        v[2].c      = *c;

        return true;
    }

    bool View3D::add_triangle(const v_vertex3d_t *v1, const v_vertex3d_t *v2, const v_vertex3d_t *v3)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0] = *v1;
        v[1] = *v2;
        v[2] = *v3;
        return true;
    }

    v_ray3d_t *View3D::get_ray(size_t index)
    {
        return vRays.get(index);
    }

    v_point3d_t *View3D::get_point(size_t index)
    {
        return vPoints.get(index);
    }

    v_segment3d_t *View3D::get_segment(size_t index)
    {
        return vSegments.get(index);
    }

    v_vertex3d_t *View3D::get_vertex(size_t index)
    {
        return vVertexes.get(index);
    }

} /* namespace mtest */
