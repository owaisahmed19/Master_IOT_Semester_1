#include "canvas.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QDebug>

/*
Canvas::Canvas(QWidget *parent) : QWidget(parent) {
    setMouseTracking(true);
    drawTriangleBorders=false;
    Polygon *S1 = new Polygon();
    S1->addVertex(120,40);
    S1->addVertex(400,160);
    S1->addVertex(320,400);
    S1->addVertex(40,80);
    polygons.push_back(S1);
    S1->setColor(S1->isConvex()?Qt::blue:Qt::cyan);
    S1->triangulate();
    qDebug() << "S1.area = " << S1->area() << " u²";

    Polygon *S2 = new Polygon();
    S2->addVertex(620,40);
    S2->addVertex(820,60);
    S2->addVertex(900,160);
    S2->addVertex(820,400);
    polygons.push_back(S2);
    S2->setColor(S1->isConvex()?Qt::blue:Qt::cyan);
    S2->triangulate();
    qDebug() << "S2.area = " << S2->area() << " u²";

}
*/

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
    setMouseTracking(true);
    drawTriangleBorders=false;

    // --- S1 (Quadrilateral) ---
    Polygon *S1 = new Polygon();
    // Vertices: (120, 40),(400, 160),(320, 400),(40, 80)
    S1->addVertex(120,40);
    S1->addVertex(400,160);
    S1->addVertex(320,400);
    S1->addVertex(40,80);
    polygons.push_back(S1);

    // --- S2 (Non-Convex Hexagon) ---
    Polygon *S2 = new Polygon();
    // Vertices: (620, 40),(820, 60),(900, 160),(820, 400),(680, 240),(540, 80)
    S2->addVertex(620,40);
    S2->addVertex(820,60);
    S2->addVertex(900,160);
    S2->addVertex(820,400);
    S2->addVertex(680,240); // New vertex for non-convex shape
    S2->addVertex(540,80);  // New vertex for non-convex shape
    polygons.push_back(S2);

    // --- S3 (Non-Convex Heptagon) ---
    Polygon *S3 = new Polygon();
    // Vertices: (500, 500),(800, 600),(900, 900),(400, 900),(680, 800),(640, 650),(240, 680)
    S3->addVertex(500, 500);
    S3->addVertex(800, 600);
    S3->addVertex(900, 900);
    S3->addVertex(400, 900);
    S3->addVertex(680, 800);
    S3->addVertex(640, 650);
    S3->addVertex(240, 680);
    polygons.push_back(S3);


    // Initial setup (Move this section down for better flow)
    /*
    for (auto &poly : polygons) {
        poly->setColor(poly->isConvex() ? Qt::blue : Qt::cyan); // This will use the unimplemented isConvex
        poly->triangulate(); // This is the unimplemented triangulation
        qDebug() << "Area = " << poly->area() << " u²"; // This will use the unimplemented area()
    }

*/
    // ... after creating S1, S2, S3 ...

    for (auto &poly : polygons) {
        bool convex = poly->isConvex();

        // Question 4: Print to console (qDebug is the Qt console output)
        qDebug() << "Polygon is Convex: " << (convex ? "Yes" : "No");

        // Question 3 (Status Bar update):
        // Send a message to the status bar (This should probably be done once
        // when the program starts, or only for a specific check).
        // Let's add the check to the Status Bar, and update the color based on convexity.

        poly->setColor(convex ? Qt::blue : Qt::cyan);
        poly->triangulate(); // placeholder
        qDebug() << "Area = " << poly->area() << " u²"; // placeholder
    }

    // Send a final status bar message showing the results for all polygons
    QString statusMsg = "S1: " + QString(S1->isConvex() ? "Convex" : "Concave") +
                        ", S2: " + QString(S2->isConvex() ? "Convex" : "Concave") +
                        ", S3: " + QString(S3->isConvex() ? "Convex" : "Concave");
    emit updateSB(statusMsg);
    // The original code only had S1 and S2 setup logic, so I moved it into the loop:
    // S1->setColor(S1->isConvex()?Qt::blue:Qt::cyan);
    // S1->triangulate();
    // qDebug() << "S1.area = " << S1->area() << " u²";

    // S2->setColor(S1->isConvex()?Qt::blue:Qt::cyan);
    // S2->triangulate();
    // qDebug() << "S2.area = " << S2->area() << " u²";
}

Canvas::~Canvas() {
    for (auto &poly : polygons) {
        delete poly;
    }
    polygons.clear();
}

void Canvas::resizeEvent(QResizeEvent *event) {
    int newWidth = event->size().width();
    int newHeight = event->size().height();
    // search max x and y of the polygons
    auto globalBB=QPair<Vector2D,Vector2D>(Vector2D(width(),height()),Vector2D(0,0));
    for (auto &poly:polygons) {
        auto bb = poly->getBoundingBox();
        if (globalBB.first.x>bb.first.x) globalBB.first.x=bb.first.x;
        if (globalBB.first.y>bb.first.y) globalBB.first.y=bb.first.y;
        if (globalBB.second.x<bb.second.x) globalBB.second.x=bb.second.x;
        if (globalBB.second.y<bb.second.y) globalBB.second.y=bb.second.y;
    }
    float maxx = globalBB.second.x-fmin(globalBB.first.x,0);
    float maxy = globalBB.second.y-fmin(globalBB.first.y,0);
    float maxs = maxx>maxy?maxx:maxy;
    if (newWidth>newHeight) {
        scale=newHeight/(maxs+40);
        origin.setY(10);
        origin.setX((newWidth-newHeight)/2);
    } else {
        scale=newWidth/(maxs+40);
        origin.setX(10);
        origin.setY((newHeight-newWidth)/2);
    }
}

void Canvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QBrush whiteBrush(Qt::SolidPattern);
    whiteBrush.setColor(Qt::white);
    painter.fillRect(0,0,width(),height(),whiteBrush);

    QPointF points[]={{0,-2},{80,-2},{80,-10},{100,0},{80,10},{80,2},{0,2}};
    painter.save();
    painter.translate(origin.x(),height()-origin.y());
    painter.scale(scale,-scale);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawPolygon(points,7);
    painter.save();
    painter.setBrush(Qt::green);
    painter.rotate(90);
    painter.drawPolygon(points ,7);
    painter.restore ();

    // drawing of the polygons
    for (auto &poly: polygons) {
        poly->draw(painter,drawTriangleBorders);
    }

    if (drawDistance) {
        painter.setPen(QPen(Qt::blue,5));
        painter.drawLine(distanceVector.first,distanceVector.second);
    }
    painter.restore();

    // draw points in a standard referential
    QFont font("Arial") ;
    font.setPointSize(18);
    painter.setBrush(Qt::black);
    painter.setFont(font);
    Vector2D v;
    for (auto &poly:polygons) {
        int n=poly->nbVertices();
        for (int i=0; i<n; i++) {
            v = ((*poly)[i+1]-(*poly)[i]).normed().ortho();
            painter.drawText(QPointF(origin.x()+(*poly)[i].x*scale-20*v.x,height()-origin.y()-(*poly)[i].y*scale+20*v.y),QString::number(i));
        }
    }
}

/*
void Canvas::mouseMoveEvent(QMouseEvent*e) {
    // convert event->pos() to canvas coordinates
    float mouseX=float(e->pos().x()-origin.x())/scale;
    float mouseY=-float(e->pos().y()-height()+origin.y())/scale;
    Vector2D pt(mouseX,mouseY);
    // check if point is in the polygon
    for (auto &poly:polygons) {
        poly->changeColor(pt);
    }
    if (drawDistance) {
        // to complete
    } else {
        emit updateSB(QString("Mouse position= (") + QString::number(mouseX,'f',1) + "," + QString::number(mouseY,'f',1) + ")");
    }
    update();
}

*/

// ... inside canvas.cpp, add a helper function (or define static)
// Function to compute the closest point on segment AB to point P.
// Returns the distance squared and sets closestPoint to the closest point's coordinates.
static float distanceToSegmentSquared(const Vector2D& A, const Vector2D& B, const Vector2D& P, Vector2D& closestPoint) {
    Vector2D AB = B - A;
    Vector2D AP = P - A;

    // Projection t = (AP . AB) / |AB|^2
    float magSq = AB.x * AB.x + AB.y * AB.y;
    float t = (AP.x * AB.x + AP.y * AB.y) / magSq;

    if (t < 0.0f) {
        // Closest point is A (before the segment)
        closestPoint = A;
    } else if (t > 1.0f) {
        // Closest point is B (after the segment)
        closestPoint = B;
    } else {
        // Closest point is on the segment
        closestPoint = A + AB * t;
    }

    // Return distance squared
    return (P - closestPoint).length() * (P - closestPoint).length();
}

void Canvas::mouseMoveEvent(QMouseEvent*e) {
    float mouseX=float(e->pos().x()-origin.x())/scale;
    float mouseY=-float(e->pos().y()-height()+origin.y())/scale;
    Vector2D pt(mouseX,mouseY);

    // 1. Point Picking
    for (auto &poly:polygons) {
        poly->changeColor(pt);
    }

    // 2. Distance Calculation and Status Bar Update (Q7)
    if (drawDistance) {
        float min_dist_sq = 1e38f;
        Vector2D closest_pt_on_poly;

        for (auto &poly : polygons) {
            int N = poly->nbVertices();
            for (int i = 0; i < N; ++i) {
                const Vector2D& A = (*poly)[i];
                const Vector2D& B = (*poly)[i+1];

                Vector2D closest_on_edge;
                float dist_sq = distanceToSegmentSquared(A, B, pt, closest_on_edge);

                if (dist_sq < min_dist_sq) {
                    min_dist_sq = dist_sq;
                    closest_pt_on_poly = closest_on_edge;
                }
            }
        }

        float min_dist = sqrt(min_dist_sq);

        // Set the vector to be drawn (mouse point to closest point on the polygon)
        distanceVector.first.setX(pt.x);
        distanceVector.first.setY(pt.y);
        distanceVector.second.setX(closest_pt_on_poly.x);
        distanceVector.second.setY(closest_pt_on_poly.y);

        // Update Status Bar with distance (u = 10 pixels requirement is usually handled by coordinate system,
        // but for safety, the distance is typically reported in world units)
        emit updateSB(QString("Mouse position= (") + QString::number(mouseX,'f',1) + "," +
                      QString::number(mouseY,'f',1) +
                      ") | Min Distance: " + QString::number(min_dist, 'f', 2) + " u");

    } else {
        // Normal status bar update
        emit updateSB(QString("Mouse position= (") + QString::number(mouseX,'f',1) + "," + QString::number(mouseY,'f',1) + ")");
    }
    update();
}

