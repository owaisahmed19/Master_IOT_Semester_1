/**
 * @author Benoît Piranda
 * @date 7/11/2024
 * @todo Make more tests of the triangulation algorithm with complex shapes.
 **/

#ifndef POLYGON_H
#define POLYGON_H
#include "vector2d.h"
#include <QPainter>
#include <QDebug>

/**
 * @brief The Triangle class store 3 pointers to existing Vector2D vertices.
 * It is used by the MyPolygon class to create a set of internal triangles.
 */
class Triangle {
    Vector2D *tabPts[3]; ///< array of 3 pointers to point
public:
    /**
     * @brief Constuctor of triangle with pointers to vertices,
     * @warning The order of vertices must be CCW
     * @param p_p0 P0
     * @param p_p1 P1
     * @param p_p2 P2
     */
    Triangle(Vector2D *p_p0,Vector2D *p_p1,Vector2D *p_p2) {
        tabPts[0]=p_p0;
        tabPts[1]=p_p1;
        tabPts[2]=p_p2;
    }
    /**
     * @brief operator []
     * @param i
     * @return vertex #i coordinates.
     */
    const Vector2D* operator[](int i) const { return tabPts[i]; }
};

/**
 *  @brief MyPolygon class allow to create, draw and manipulate
 *  polygons, especially check if a point is inside and compute
 *  the surface of the polygon.
*/
class Polygon {
private:
    ///< @warning Store N+1 vertices in tabPts array, first is duplicated in last.
    QVector<Vector2D> tabPts; ///< array of vertex positions
    QVector<Triangle> triangles; ///< array of triangles for the triangulation process
    QColor currentColor; ///< current drawing color of the polygon
public:
    /**
     * @brief Constructor of a polygon.
     */
    Polygon():currentColor(Qt::green) {}
    /**
     * @brief nbVertices
     * @return the number of vertices of the polygon
     */
    int nbVertices() const { return tabPts.size()==0?0:tabPts.size()-1; }
    /**
     * @brief Add a new vertex at the end of the list
     * @warning The number of vertices added to the polygon must be lower than Nmax.
     * @param x abscissa of the new vertex
     * @param y ordinate of the new vertex
     */
    void addVertex(float x,float y);
    /**
     * @brief return the boundig box coordinates of the polygon
     * @return The first vector is the left bottom corner and the second vector is the right top corner of the box.
     */
    QPair<Vector2D,Vector2D> getBoundingBox() const;
    /**
     * @brief Set the color of the polygon to c
     * @param c The new color.
     */
    void setColor(const QColor c) { currentColor=c; }

    /**
     * @brief Turns the color of the polygon to green if the point pt is inside the polygon,
     * and turns it to red otherwise.
     * @warning The triangulation of the polygon must be up to date.
     * @param pt point to check.
     * @todo Complete the code
     */

  void changeColor(const Vector2D &pt);

    /**
     * @brief A way to read the list of vertices of the polygon.
     * @warning The returned array is read-only!
     * @param n number of vertices in the array.
     * @return a read-only version of the array of vertices.
     */

    Vector2D operator[](int i) const { return tabPts[i]; }
    /**
     * @brief Draw the polygon.
     * @param painter Current painter context.
     * @param showTriangles Shows the internal triangle if true.
     */
    void draw(QPainter &painter,bool showTriangles) const;
    /**
     * @brief triangulate the polygon and store triangles in "triangles" array.
     */
    void triangulate();


    bool isOnTheLeft(const Vector2D &p, int i) const;

   // bool isConvex() const {
     //   // \todo write the code
       // return false;
    //}

    bool isConvex() const;
    /**
     * @brief area
     * @return
     */

    float area() const;
};

#endif // POLYGON_H
