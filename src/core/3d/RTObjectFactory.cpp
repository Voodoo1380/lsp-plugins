/*
 * RTObjectFactory.cpp
 *
 *  Created on: 12 февр. 2019 г.
 *      Author: sadko
 */

#include <core/3d/RTObjectFactory.h>

namespace lsp
{
    
    RTObjectFactory::RTObjectFactory()
    {
    }
    
    RTObjectFactory::~RTObjectFactory()
    {
        sScene.destroy();
    }

#define ICO_X       0.525731112119133606f
#define ICO_N       0.0f
#define ICO_Z       0.850650808352039932f

    // Icosahedron vertex coordinates
    static const point3d_t ico_vertex[] =
    {
        { -ICO_X,  ICO_N,  ICO_Z,  1.0f },
        {  ICO_X,  ICO_N,  ICO_Z,  1.0f },
        { -ICO_X,  ICO_N, -ICO_Z,  1.0f },
        {  ICO_X,  ICO_N, -ICO_Z,  1.0f },
        {  ICO_N,  ICO_Z,  ICO_X,  1.0f },
        {  ICO_N,  ICO_Z, -ICO_X,  1.0f },
        {  ICO_N, -ICO_Z,  ICO_X,  1.0f },
        {  ICO_N, -ICO_Z, -ICO_X,  1.0f },
        {  ICO_Z,  ICO_X,  ICO_N,  1.0f },
        { -ICO_Z,  ICO_X,  ICO_N,  1.0f },
        {  ICO_Z, -ICO_X,  ICO_N,  1.0f },
        { -ICO_Z, -ICO_X,  ICO_N,  1.0f }
    };

    // Icosahedron triangle vertex indexes
    static const uint8_t ico_triangle[] =
    {
        0, 1, 4,
        0, 4, 9,
        9, 4, 5,
        4, 8, 5,

        4, 1, 8,
        8, 1, 10,
        8, 10, 3,
        5, 8, 3,

        5, 3, 2,
        2, 3, 7,
        7, 3, 10,
        7, 10, 6,

        7, 6, 11,
        11, 6, 0,
        0, 6, 1,
        6, 10, 1,

        9, 11, 0,
        9, 2, 11,
        9, 5, 2,
        7, 11, 2
    };

#undef ICO_X
#undef ICO_N
#undef ICO_Z

    Object3D *RTObjectFactory::generateIcosphere(size_t level)
    {
        typedef struct sp_triangle_t
        {
            size_t      vi[3];  // Vertex indexes
        } sp_triangle_t;

        // Generate object name
        LSPString obj_name;
        if (!obj_name.fmt_ascii("Icosphere level=%d", int(level)))
            return NULL;

        // Add initial vertexes (icosahedron has 12 vertexes)
        size_t ico_index[12];
        for (size_t i=0; i<12; ++i)
        {
            ssize_t idx     = sScene.add_vertex(&ico_vertex[i]);
            if (idx < 0)
                return NULL;
            ico_index[i]    = idx;
        }

        // Create initial triangles (icosahedron has 20 triangles)
        size_t total_triangles  = 20 << (level << 1);  // 20 * 4^level

        sp_triangle_t *vt   = reinterpret_cast<sp_triangle_t *>(::malloc(total_triangles * sizeof(sp_triangle_t))); // Temporary array
        if (vt == NULL)
            return NULL;

        const uint8_t *src_idx = ico_triangle;
        for (size_t i=0; i<20; ++i, src_idx += 3)
        {
            sp_triangle_t *t    = &vt[i];
            t->vi[0]        = ico_index[size_t(src_idx[0])];
            t->vi[1]        = ico_index[size_t(src_idx[1])];
            t->vi[2]        = ico_index[size_t(src_idx[2])];
        }

        // Generate additional triangles
        point3d_t *p[3];
        point3d_t sp[3];

        for (size_t step=0; step < level; ++step)
        {
            size_t nt           = 20 << (step << 1); // 20 * 4^step
            sp_triangle_t *st   = vt;       // source triangles
            sp_triangle_t *dt   = &vt[nt];  // newly-allocated triangles

            // Perform split of each triangle into 4 sub-triangles
            for (size_t i=0; i<nt; ++i, ++st, dt += 3)
            {
                p[0]        = sScene.vertex(st->vi[0]);
                p[1]        = sScene.vertex(st->vi[1]);
                p[2]        = sScene.vertex(st->vi[2]);

                sp[0].x     = (p[0]->x + p[1]->x) * 0.5f;
                sp[0].y     = (p[0]->y + p[1]->y) * 0.5f;
                sp[0].z     = (p[0]->z + p[1]->z) * 0.5f;
                sp[0].w     = 1.0f;

                sp[1].x     = (p[1]->x + p[2]->x) * 0.5f;
                sp[1].y     = (p[1]->y + p[2]->y) * 0.5f;
                sp[1].z     = (p[1]->z + p[2]->z) * 0.5f;
                sp[1].w     = 1.0f;

                sp[2].x     = (p[2]->x + p[0]->x) * 0.5f;
                sp[2].y     = (p[2]->y + p[0]->y) * 0.5f;
                sp[2].z     = (p[2]->z + p[0]->z) * 0.5f;
                sp[2].w     = 1.0f;

                dsp::normalize_point(&sp[0]);
                dsp::normalize_point(&sp[1]);
                dsp::normalize_point(&sp[2]);

                dt[0].vi[0] = sScene.add_vertex(&sp[0]);
                if (dt[0].vi[0] < 0)
                {
                    ::free(vt);
                    return NULL;
                }

                dt[0].vi[1] = sScene.add_vertex(&sp[1]);
                if (dt[0].vi[1] < 0)
                {
                    ::free(vt);
                    return NULL;
                }

                dt[0].vi[2] = sScene.add_vertex(&sp[2]);
                if (dt[0].vi[2] < 0)
                {
                    ::free(vt);
                    return NULL;
                }

                dt[1].vi[0] = dt[0].vi[0];
                dt[1].vi[1] = st->vi[1];
                dt[1].vi[2] = dt[0].vi[1];

                dt[2].vi[0] = dt[0].vi[1];
                dt[2].vi[1] = st->vi[2];
                dt[2].vi[2] = dt[0].vi[2];

                st->vi[1]   = dt[0].vi[0];
                st->vi[2]   = dt[0].vi[2];
            }
        }

        // Allocate icosphere object
        Object3D *obj = sScene.add_object(&obj_name);
        if (obj == NULL)
        {
            ::free(vt);
            return NULL;
        }

        // Add each triangle to the scene
        vector3d_t n;
        for (size_t i=0; i<total_triangles; ++i)
        {
            // Compute normal value
            p[0]    = sScene.vertex(vt[i].vi[0]);
            p[1]    = sScene.vertex(vt[i].vi[1]);
            p[2]    = sScene.vertex(vt[i].vi[2]);

            if ((p[0] == NULL) || (p[1] == NULL) || (p[2] == NULL))
            {
                ::free(vt);
                return NULL;
            }

            dsp::calc_normal3d_p3(&n, p[0], p[1], p[2]);

            size_t nid  = sScene.add_normal(&n);
            if (nid < 0)
            {
                ::free(vt);
                return NULL;
            }

            // Add triangle
            status_t res = obj->add_triangle(i, vt[i].vi[0], vt[i].vi[1], vt[i].vi[2], nid, nid, nid);
            if (res != STATUS_OK)
            {
                ::free(vt);
                return NULL;
            }
        }

        // Now all is OK, we can swap scene state and save spere identifier
        ::free(vt);
        return obj;
    }

    Object3D *RTObjectFactory::generateTriangle()
    {
        // Generate object name
        LSPString obj_name;
        if (!obj_name.set_ascii("Triangle"))
            return NULL;

        // Allocate icosphere object
        point3d_t p[3];
        vector3d_t n;
        size_t idx[3];
        ssize_t id;
        dsp::init_point_xyz(&p[0], 1.0f, 1.0f, 1.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 1.0f, 1.0f);
        dsp::init_point_xyz(&p[2], 0.0f, -1.0f, 1.0f);
        dsp::init_vector_dxyz(&n, 0.0f, 0.0f, 1.0f);

        for (size_t i=0; i<3; ++i)
        {
            id = sScene.add_vertex(&p[i]);
            if (id < 0)
                return NULL;
            idx[i] = id;
        }

        id = sScene.add_normal(&n);
        if (id < 0)
            return NULL;

        // Create object and add triangle
        Object3D *obj = sScene.add_object(&obj_name);
        if (obj == NULL)
            return NULL;
        status_t res = obj->add_triangle(0, idx[0], idx[1], idx[2], id, id, id);
        if (res < 0)
            return NULL;

        return obj;
    }

    Object3D *RTObjectFactory::buildIcosphere(size_t level)
    {
        // Lookup for object
        for (size_t i=0, n=sObjects.size(); i<n; ++i)
        {
            f_object_t *obj = sObjects.get(i);
            if ((obj != NULL) && (obj->type == OT_ICOSPHERE) && (obj->icosphere.level == level))
                return sScene.get_object(obj->obj_id);
        }

        // Object was not found, create new one
        f_object_t *obj = sObjects.add();
        if (obj == NULL)
            return NULL;

        Object3D *res       = generateIcosphere(level);
        if (res == NULL)
        {
            sObjects.remove(obj);
            return NULL;
        }

        obj->type               = OT_ICOSPHERE;
        obj->obj_id             = sScene.get_object_index(res);
        obj->icosphere.level    = level;

        return res;
    }

    Object3D *RTObjectFactory::buildTriangle()
    {
        // Lookup for object
        for (size_t i=0, n=sObjects.size(); i<n; ++i)
        {
            f_object_t *obj = sObjects.get(i);
            if ((obj != NULL) && (obj->type == OT_TRIANGLE))
                return sScene.get_object(obj->obj_id);
        }

        // Object was not found, create new one
        f_object_t *obj = sObjects.add();
        if (obj == NULL)
            return NULL;

        Object3D *res       = generateTriangle();
        if (res == NULL)
        {
            sObjects.remove(obj);
            return NULL;
        }

        obj->type               = OT_ICOSPHERE;
        obj->obj_id             = sScene.get_object_index(res);

        return res;
    }

    void RTObjectFactory::clear()
    {
        sScene.clear();
        sObjects.clear();
    }

} /* namespace mtest */
