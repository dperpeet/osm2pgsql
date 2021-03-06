/*
#-----------------------------------------------------------------------------
# Part of osm2pgsql utility
#-----------------------------------------------------------------------------
# By Artem Pavlenko, Copyright 2007
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#-----------------------------------------------------------------------------
*/

#ifndef GEOMETRY_BUILDER_H
#define GEOMETRY_BUILDER_H

#include "osmtypes.hpp"

#include <vector>
#include <string>
#include <memory>

namespace geos { namespace geom {
class Geometry;
class GeometryFactory;
class CoordinateSequence;
}}

struct reprojection;

class geometry_builder
{
public:
    struct wkt_t
    {
        wkt_t(const geos::geom::Geometry *geom, bool poly, reprojection *p = nullptr);

        wkt_t(const std::string &geom_str, bool poly, double geom_area = 0)
        : geom(geom_str), area(geom_area), polygon(poly)
        {}

        bool is_polygon() const
        { return polygon; }

        std::string geom;
        double area;
        bool polygon;
    };

    // type to represent an optional return of WKT-encoded geometry
    typedef std::shared_ptr<geometry_builder::wkt_t> maybe_wkt_t;
    typedef std::shared_ptr<std::vector<geometry_builder::wkt_t> > maybe_wkts_t;

    static int parse_wkb(const char *wkb, multinodelist_t &nodes, bool *polygon);
    maybe_wkt_t get_wkt_simple(const nodelist_t &nodes, int polygon) const;
    maybe_wkts_t get_wkt_split(const nodelist_t &nodes, int polygon, double split_at) const;
    maybe_wkts_t build_both(const multinodelist_t &xnodes, int make_polygon,
                            int enable_multi, double split_at, osmid_t osm_id = -1) const;
    maybe_wkts_t build_polygons(const multinodelist_t &xnodes, bool enable_multi, osmid_t osm_id = -1) const;
    // Used by gazetteer. Outputting a multiline, it only ever returns one WKT
    maybe_wkt_t build_multilines(const multinodelist_t &xnodes, osmid_t osm_id) const;

    void set_exclude_broken_polygon(bool exclude)
    {
        excludepoly = exclude;
    }

    void set_reprojection(reprojection *r)
    {
        projection = r;
    }


private:
    std::unique_ptr<geos::geom::Geometry>
    create_simple_poly(geos::geom::GeometryFactory &gf,
                       std::unique_ptr<geos::geom::CoordinateSequence> coords) const;

    bool excludepoly = false;
    reprojection *projection = nullptr;
};

#endif
