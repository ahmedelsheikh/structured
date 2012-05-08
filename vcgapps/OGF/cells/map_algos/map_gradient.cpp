/*
 *  GXML/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000 Bruno Levy
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: Bruno Levy
 *
 *     levy@loria.fr
 *
 *     ISA Project
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */

#include <OGF/cells/map_algos/map_gradient.h>

namespace OGF {

    Vector3d MapGradient::compute_gradient(Map::Vertex* v, const Vector2d& w) {
        Vector3d result(0,0,0) ;
        Map::Halfedge* h = v->halfedge() ;
        int count = 0 ;
        do {
            result = result + compute_gradient(h->prev(), h, h->next(), w) ;
            h = h->next() ;
            count++ ;
        } while(h != v->halfedge()) ;
        return 1.0 / double(count) * result ;
    }


    void MapGradient::compute_gradient(
        const Point3d& p1, const Point3d& p2, const Point3d& p3,
        const Point2d& q1, const Point2d& q2, const Point2d& q3,
        double* TU,
        double* TV
    ) {
        double u1 = q1.x() ;
        double v1 = q1.y() ;
        double u2 = q2.x() ;
        double v2 = q2.y() ;
        double u3 = q3.x() ;
        double v3 = q3.y() ;

        double delta = 
            (u2*v3 - u3*v2) +
            (u3*v1 - u1*v3) +
            (u1*v2 - u2*v1) ;

        if(::fabs(delta) < 1e-50) {
//            std::cerr << "Flat triangle in (u,v) space"
//                      << std::endl ;
            TU[0] = 1e-6 ;
            TU[1] = 1e-6 ;
            TU[2] = 1e-6 ;

            TV[0] = 1e-6 ;
            TV[1] = 1e-6 ;
            TV[2] = 1e-6 ;
        } else {
            TU[0] = (v2 - v3) / delta ;
            TU[1] = (v3 - v1) / delta ;
            TU[2] = (v1 - v2) / delta ;
            
            TV[0] = (u3 - u2) / delta ;
            TV[1] = (u1 - u3) / delta ; 
            TV[2] = (u2 - u1) / delta ;
        }
    }

    Vector3d MapGradient::compute_gradient(
        Map::Halfedge* h1,
        Map::Halfedge* h2,
        Map::Halfedge* h3,
        const Vector2d& W
    ) {
        double TU[3] ;
        double TV[3] ;
        compute_gradient(h1,h2,h3,TU,TV) ;
        const Point3d& p1 = h1->vertex()->point() ;
        const Point3d& p2 = h2->vertex()->point() ;
        const Point3d& p3 = h3->vertex()->point() ;
        Vector3d grad_u(
            TU[0] * p1.x() + TU[1] * p2.x() + TU[2] * p3.x(),
            TU[0] * p1.y() + TU[1] * p2.y() + TU[2] * p3.y(),
            TU[0] * p1.z() + TU[1] * p2.z() + TU[2] * p3.z()
        ) ; 
        Vector3d grad_v(
            TV[0] * p1.x() + TV[1] * p2.x() + TV[2] * p3.x(),
            TV[0] * p1.y() + TV[1] * p2.y() + TV[2] * p3.y(),
            TV[0] * p1.z() + TV[1] * p2.z() + TV[2] * p3.z()
        ) ; 
        return W.x() * grad_u + W.y() * grad_v ;
    }

    void MapGradient::compute_gradient(
        Map::Halfedge* h1,
        Map::Halfedge* h2,
        Map::Halfedge* h3,
        double* TU,
        double* TV
    ) {
        compute_gradient(
            h1->vertex()->point(),
            h2->vertex()->point(),
            h3->vertex()->point(),
            h1->tex_coord(),
            h2->tex_coord(),
            h3->tex_coord(),
            TU, TV
        ) ;
    }

}
