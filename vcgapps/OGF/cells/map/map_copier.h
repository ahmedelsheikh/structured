/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000-2005 INRIA - Project ALICE
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
 *  Contact: Bruno Levy - levy@loria.fr
 *
 *     Project ALICE
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 *
 * As an exception to the GPL, Graphite can be linked with the following (non-GPL) libraries:
 *     Qt, SuperLU, WildMagic and CGAL
 */
 

#ifndef __OGF_CELLS_MAP_MAP_COPIER__
#define __OGF_CELLS_MAP_MAP_COPIER__

#include <OGF/cells/common/common.h>
#include <OGF/cells/map/map.h>
#include <OGF/cells/map/map_attributes.h>
#include <OGF/cells/map_algos/map_components.h>
#include <OGF/basic/attributes/attribute_copier.h>

#include <string>
#include <set>

namespace OGF {

    class MapBuilder ;

    /**
     * Copies from a Map or a MapComponent into a Map.
     * Named attributes can be also copied. Note: halfedge
     * attributes copy is not implemented yet.
     */
    class CELLS_API MapCopier {
    public:
        MapCopier() ;
        ~MapCopier() ;

        void set_copy_all_attributes(bool x) {
            copy_all_attributes_ = x ; 
        }

        void declare_vertex_attribute_to_copy(const std::string& x) {
            vertex_attributes_to_copy_.insert(x) ;
        }

        void declare_tex_vertex_attribute_to_copy(const std::string& x) {
            tex_vertex_attributes_to_copy_.insert(x) ;
        }

        void declare_halfedge_attribute_to_copy(const std::string& x) {
            halfedge_attributes_to_copy_.insert(x) ;
        }

        void declare_facet_attribute_to_copy(const std::string& x) {
            facet_attributes_to_copy_.insert(x) ;
        }

        void clear_attributes_to_copy() {
            vertex_attributes_to_copy_.clear() ;
            tex_vertex_attributes_to_copy_.clear() ;
            halfedge_attributes_to_copy_.clear() ;
            facet_attributes_to_copy_.clear() ;
        }

        void copy(
            MapBuilder& to, Map* from, 
            MapVertexAttribute<int>& vertex_id, MapTexVertexAttribute<int>& tex_vertex_id, 
            int& cur_vertex_id, int& cur_tex_vertex_id
        ) ;

        void copy(MapBuilder& to, Map* from) {
            MapVertexAttribute<int> vertex_id(from) ;
            MapTexVertexAttribute<int> tex_vertex_id(from) ;
            int cur_vertex_id = 0 ;
            int cur_tex_vertex_id = 0 ;
            copy(to, from, vertex_id, tex_vertex_id, cur_vertex_id, cur_tex_vertex_id) ;
        }

        void copy(
            MapBuilder& to, MapComponent* from,
            MapVertexAttribute<int>& vertex_id, MapTexVertexAttribute<int>& tex_vertex_id, 
            int& cur_vertex_id, int& cur_tex_vertex_id
        ) ;

        void copy(MapBuilder& to, MapComponent* from) {
            MapVertexAttribute<int> vertex_id(from->map()) ;
            MapTexVertexAttribute<int> tex_vertex_id(from->map()) ;
            int cur_vertex_id = 0 ;
            int cur_tex_vertex_id = 0 ;
            copy(to, from, vertex_id, tex_vertex_id, cur_vertex_id, cur_tex_vertex_id) ;
        }
        
    protected:

        // ------------------------------ copy attributes --------------------------------------------------------

        void copy_vertex_attributes(Map::Vertex* to, Map::Vertex* from) {
            copy_attributes(vertex_attribute_copiers_, to, from) ;
        }
        
        void copy_tex_vertex_attributes(Map::TexVertex* to, Map::TexVertex* from) {
            copy_attributes(tex_vertex_attribute_copiers_, to, from) ;
        }
        
        void copy_halfedge_attributes(Map::Halfedge* to, Map::Halfedge* from) {
            copy_attributes(halfedge_attribute_copiers_, to, from) ;
        }
        
        void copy_facet_attributes(Map::Facet* to, Map::Facet* from) {
            copy_attributes(facet_attribute_copiers_, to, from) ;
        }

        void bind_attribute_copiers(Map* destination, Map* source) ;

    private:
        std::set<std::string> vertex_attributes_to_copy_ ;
        std::set<std::string> tex_vertex_attributes_to_copy_ ;
        std::set<std::string> halfedge_attributes_to_copy_ ;
        std::set<std::string> facet_attributes_to_copy_ ;
        bool copy_all_attributes_ ;

        std::vector< AttributeCopier<Map::Vertex> > vertex_attribute_copiers_ ;
        std::vector< AttributeCopier<Map::Halfedge> > halfedge_attribute_copiers_ ;
        std::vector< AttributeCopier<Map::Facet> > facet_attribute_copiers_ ;
        std::vector< AttributeCopier<Map::TexVertex> > tex_vertex_attribute_copiers_ ;

        Map* current_source_ ;
        Map* current_destination_ ;
    } ;

}
#endif

