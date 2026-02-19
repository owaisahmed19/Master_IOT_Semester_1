#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <canvas.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <trianglemesh.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // load initial simple case
    loadJson("../../json/simple.json");
    //loadJson("../../json/arcane.json");
    //loadJson("../../json/university_campus.json");
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Loads environmental and entity data from a JSON configuration file.
 * Handles window dimensions, server locations, and drone objectives.
 */
bool MainWindow::loadJson(const QString& title) {
    // 1. Attempt to open the file for reading
    QFile file(title);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Impossible d'ouvrir le fichier:" << title;
        return false;
    }

    // 2. Read raw data and close the file handle
    QByteArray data = file.readAll();
    file.close();

    // 3. Parse JSON data and ensure it is a valid object
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) return false;

    QJsonObject root = doc.object();

    // --- Window Safe Parsing ---
    // Sets the coordinate system and canvas size
    if (root.contains("window") && root["window"].isObject()) {
        QJsonObject win = root["window"].toObject();
        // Split strings like "-100,-80" into separate coordinate parts
        QStringList origin = win.value("origine").toString().split(",");
        QStringList size   = win.value("size").toString().split(",");

        // Robustness check: Ensure split resulted in both X and Y components
        // This prevents "index out of range" ASSERT failures
        if (origin.size() >= 2 && size.size() >= 2) {
            QPoint wOrigin(origin[0].toInt(), origin[1].toInt());
            QSize wSize(size[0].toInt(), size[1].toInt());
            ui->canvas->setWindow(wOrigin, wSize); // Configure canvas viewport
        }
    }

    // --- Servers Safe Parsing ---
    // Clear existing servers to allow for a fresh load
    ui->canvas->servers.clear();
    if (root.contains("servers") && root["servers"].isArray()) {
        QJsonArray arr = root["servers"].toArray();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            Server s;
            s.name = obj.value("name").toString();

            // Extract position coordinates from "x,y" string
            QStringList parts = obj.value("position").toString().split(',');
            if (parts.size() >= 2) {
                s.position = QPointF(parts[0].toDouble(), parts[1].toDouble());
            }

            s.color = QColor(obj.value("color").toString());
            s.id = i; // Assign ID based on array index for pathfinding
            ui->canvas->servers.append(s);
        }
    }

    // --- Drones Safe Parsing ---
    // Clear existing drones before loading new entities
    ui->canvas->drones.clear();
    if (root.contains("drones") && root["drones"].isArray()) {
        QJsonArray arr = root["drones"].toArray();
        for (const QJsonValue &v : arr) {
            QJsonObject obj = v.toObject();
            Drone d;
            d.name = obj.value("name").toString();

            // Convert coordinate string to Vector2D for physics calculations
            QStringList parts = obj.value("position").toString().split(',');
            if (parts.size() >= 2) {
                d.position = Vector2D(parts[0].toDouble(), parts[1].toDouble());
            }

            // Link the drone to its target server by name
            QString targetName = obj.value("target").toString();
            d.target = nullptr;
            for (auto &s : ui->canvas->servers) {
                if (s.name == targetName) {
                    d.target = &s;
                    break;
                }
            }
            ui->canvas->drones.append(d);
        }
    }

    // 4. Run Geometric and Algorithmic initializations
    createVoronoiMap();   // Generate Voronoi areas for each server [cite: 37]
    createServersLinks(); // Establish connections between neighboring areas [cite: 72]
    fillDistanceArray();  // Pre-calculate shortest paths using Dijkstra [cite: 78-79]

    // 5. Final Setup: Initial Drone Logic
    for (auto &d : ui->canvas->drones) {
        // Find which server room the drone is currently occupying [cite: 100]
        d.overflownArea(ui->canvas->servers);

        if (d.target) {
            // Requirement 2: The drone's first task is to reach its local server [cite: 101]
            Server* current = d.overflownArea(ui->canvas->servers);
            if (current) {
                // Set navigation destination to the server's center position
                d.destination = Vector2D(current->position.x(), current->position.y());
            }
        }
    }

    return true; // Successfully initialized environment
}

void MainWindow::createVoronoiMap() {
    TriangleMesh mesh(ui->canvas->servers);
    mesh.setBox(ui->canvas->getOrigin(),ui->canvas->getSize());

    auto triangles = mesh.getTriangles();
    auto m_servor = ui->canvas->servers.begin();
    QVector<const Triangle*> tabTri;
    while (m_servor!=ui->canvas->servers.end()) {
        // for all vertices of the mesh
        const Vector2D vert((*m_servor).position.x(),(*m_servor).position.y());
        auto mt_it = triangles->begin();
        tabTri.clear(); // tabTri: list of triangles containing m_vert
        while (mt_it!=triangles->end()) {
            if ((*mt_it).hasVertex(vert)) {
                tabTri.push_back(&(*mt_it));
            }
            mt_it++;
        }
        // find left border
        auto first = tabTri.begin();
        auto tt_it = tabTri.begin();
        bool found=false;
        while (tt_it!=tabTri.end() && !found) {
            auto comp_it = tabTri.begin();
            while (comp_it!=tabTri.end() && (*tt_it)->getNextVertex(vert)!=(*comp_it)->getPrevVertex(vert)) {
                comp_it++;
            }
            if (comp_it==tabTri.end()) {
                first=tt_it;
                found=true;
            }
            tt_it++;
        }
        // create polygon

        //poly->setColor((*m_servor)->color);
        tt_it=first;
        if (found && mesh.isInWindow((*tt_it)->getCenter().x,(*tt_it)->getCenter().y)) { // add a point for the left border
            Vector2D V = (*first)->nextEdgeNormal(vert);
            float k;
            if (V.x > 0) { // (circumCenter+k V).x=width
                k = (mesh.getWindowXmax() - (*first)->getCenter().x) / float(V.x);
            } else {
                k = (mesh.getWindowXmin()-(*first)->getCenter().x) / float(V.x);
            }
            if (V.y > 0) { // (circumCenter+k V).y=height
                k = fmin(k, (mesh.getWindowYmax() - (*first)->getCenter().y) / float(V.y));
            } else {
                k = fmin(k, (mesh.getWindowYmin()-(*first)->getCenter().y) / float(V.y));
            }
            m_servor->area.addVertex(Vector2D((*first)->getCenter() + k * V));
            Vector2D pt = (*first)->getCenter() + k * V;
        }
        auto comp_it = first;
        do {
            m_servor->area.addVertex((*tt_it)->getCenter());
            // search triangle on right of tt_it
            comp_it = tabTri.begin();
            while (comp_it!=tabTri.end() && (*tt_it)->getPrevVertex(vert)!=(*comp_it)->getNextVertex(vert)) {
                comp_it++;
            }
            if (comp_it!=tabTri.end()) tt_it = comp_it;
        } while (tt_it!=first && comp_it!=tabTri.end());
        if (found && mesh.isInWindow((*tt_it)->getCenter())) { // add a point for the right border
            Vector2D V = (*tt_it)->previousEdgeNormal(vert);
            float k;
            if (V.x > 0) { // (circumCenter+k V).x=width
                k = (mesh.getWindowXmax() - (*tt_it)->getCenter().x) / float(V.x);
            } else {
                k = (mesh.getWindowXmin()-(*tt_it)->getCenter().x) / float(V.x);
            }
            if (V.y > 0) { // (circumCenter+k V).y=height
                k = fmin(k, (mesh.getWindowYmax() - (*tt_it)->getCenter().y) / float(V.y));
            } else {
                k = fmin(k, (mesh.getWindowYmin()-(*tt_it)->getCenter().y) / float(V.y));
            }
            m_servor->area.addVertex(Vector2D((*tt_it)->getCenter() + k * V));
            Vector2D pt = (*tt_it)->getCenter() + k * V;
        }
        qDebug() << m_servor->name;
        m_servor->area.clip(mesh.getWindowXmin(),mesh.getWindowYmin(),mesh.getWindowXmax(),mesh.getWindowYmax());
        m_servor->area.triangulate();

        m_servor++;
    }
}

/**
 * @brief Establishes communication links between servers by identifying shared polygon edges.
 * This method fulfills Exercise 2, Question 1 by creating a graph where nodes are servers
 * and edges are the common boundaries (doors) between their areas. [cite: 68, 69, 72]
 */
void MainWindow::createServersLinks() {
    // 1. Iterate through every unique pair of servers (O(N^2) complexity) [cite: 72, 75]
    for (int i = 0; i < ui->canvas->servers.size(); ++i) {
        for (int j = i + 1; j < ui->canvas->servers.size(); ++j) {
            Server &s1 = ui->canvas->servers[i]; // First server in the pair [cite: 17]
            Server &s2 = ui->canvas->servers[j]; // Second server in the pair [cite: 17]

            // 2. Compare every edge of s1's polygon with every edge of s2's polygon [cite: 73]
            for (int e1 = 0; e1 < s1.area.nbVertices(); ++e1) {
                // Get the start and end vertices of the current edge for s1
                auto edge1 = s1.area.getEdge(e1);

                for (int e2 = 0; e2 < s2.area.nbVertices(); ++e2) {
                    // Get the start and end vertices of the current edge for s2
                    auto edge2 = s2.area.getEdge(e2);

                    // 3. Check if the edges are identical (shared boundary) [cite: 73]
                    // Since polygons are oriented CCW, a shared edge will have vertices
                    // in opposite order (e.g., s1: A->B, s2: B->A).
                    if ((edge1.first == edge2.second && edge1.second == edge2.first)) {

                        // 4. Create a new Link object representing the path through the door [cite: 70, 72]
                        // The Link stores references to both servers and the edge center.
                        Link *link = new Link(&s1, &s2, edge1);

                        // 5. Register the link in the canvas and both servers for pathfinding [cite: 72, 78]
                        ui->canvas->links.append(link); // For drawing the graph [cite: 42]
                        s1.links.append(link);          // For Dijkstra from s1 [cite: 79]
                        s2.links.append(link);          // For Dijkstra from s2 [cite: 79]
                    }
                }
            }
        }
    }
}
/**
 * @brief Prints a formatted table of all computed minimum distances between servers.
 * Fulfills Exercise 2, Question 5.
 */
void MainWindow::printDistanceTable() {
    int nServers = ui->canvas->servers.size();
    if (nServers == 0) return;

    qDebug() << "\n--- Computed Minimum Distances Table (Exercise 2, Q5) ---";

    // 1. Print Header Row with destination names
    QString header = "Source \\ Dest |";
    for (const auto &s : ui->canvas->servers) {
        header += QString(" %1 |").arg(s.name.left(8), -8);
    }
    qDebug() << header;
    qDebug() << QString(header.length(), '-');

    // 2. Print each row (Source Server)
    for (const auto &src : ui->canvas->servers) {
        QString row = QString("%1 |").arg(src.name.left(12), -12);

        for (int j = 0; j < nServers; ++j) {
            // Retrieve the distance computed in fillDistanceArray [cite: 78]
            double d = src.bestDistance[j].second;

            if (d >= 1e9) {
                row += "  INF   |"; // unreachable
            } else {
                row += QString(" %1 |").arg(d, 6, 'f', 1); // 1 decimal place
            }
        }
        qDebug() << row;
    }
    qDebug() << "---------------------------------------------------------\n";
}

/**
 * Computes the shortest paths between all servers in the graph.
 * This method implements Dijkstra's algorithm to fill the bestDistance array
 * for every server, allowing drones to know which link to follow to reach
 * any target server. [cite: 78, 79]
 */
void MainWindow::fillDistanceArray() {
    // 1. Get the total number of servers to define array sizes
    int nServers = ui->canvas->servers.size();
    if (nServers == 0) return;

    // 2. Initialize the bestDistance structure for every server
    for (auto &s : ui->canvas->servers) {
        // Resize to match the number of possible destination servers
        s.bestDistance.resize(nServers);
        for (int i = 0; i < nServers; i++) {
            // Initialize with a null link and a very large distance (infinity)
            s.bestDistance[i] = {nullptr, 1e9};
        }
        // The distance from a server to itself is always 0
        s.bestDistance[s.id] = {nullptr, 0};
    }

    // 3. Run Dijkstra's algorithm starting from each server
    // Each iteration treats 'startNode' as the source of the shortest path tree.
    for (int startNode = 0; startNode < nServers; ++startNode) {
        // Temporary array to store minimum distances found so far for this specific startNode
        QVector<qreal> minDist(nServers, 1e9);
        minDist[startNode] = 0;

        // Track which servers have been fully processed
        QSet<int> visited;

        // Process all servers in the graph
        for (int count = 0; count < nServers; ++count) {
            // Find the unvisited server with the smallest known distance
            int u = -1;
            for (int i = 0; i < nServers; ++i) {
                if (!visited.contains(i) && (u == -1 || minDist[i] < minDist[u]))
                    u = i;
            }

            // If no reachable server is found, stop processing this startNode
            if (u == -1 || minDist[u] >= 1e9) break;
            visited.insert(u);

            // Explore all neighbors of the current server 'u' [cite: 72]
            for (Link *link : ui->canvas->servers[u].links) {
                // Determine the neighbor server on the other side of the link [cite: 70]
                Server *neighbor = (link->getNode1()->id == u) ? link->getNode2() : link->getNode1();
                int v = neighbor->id;

                // Get the physical length of the link (distance through the door)
                qreal weight = link->getDistance();

                // 4. Relaxation Step: Check if a shorter path to 'v' exists via 'u'
                if (minDist[u] + weight < minDist[v]) {
                    minDist[v] = minDist[u] + weight;

                    // 5. Path Tracking (Exercise 2, Question 3)
                    // We need to store the FIRST link to follow from 'startNode' to reach 'v'.
                    if (u == startNode) {
                        // If 'v' is a direct neighbor, the first step is the link itself
                        ui->canvas->servers[startNode].bestDistance[v] = {link, minDist[v]};
                    } else {
                        // If 'v' is reached through 'u', the first step to 'v' is the same
                        // as the first step we took to get to 'u'.
                        Link* firstStep = ui->canvas->servers[startNode].bestDistance[u].first;
                        ui->canvas->servers[startNode].bestDistance[v] = {firstStep, minDist[v]};
                    }
                }
            }
        }
    }
    printDistanceTable();
}
void MainWindow::update() {
    static int last=elapsedTimer.elapsed();
    int current=elapsedTimer.elapsed();
    int dt=current-last;
    // update positions of drones
    for (auto &drone:ui->canvas->drones) {
        drone.move(dt/1000.0);
    }
    ui->canvas->repaint();
}


void MainWindow::on_actionShow_graph_triggered(bool checked) {
    ui->canvas->showGraph=checked;
    ui->canvas->repaint();
}


void MainWindow::on_actionMove_drones_triggered() {
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start();

    elapsedTimer.start();
}


void MainWindow::on_actionQuit_triggered() {
    QApplication::quit();
}


void MainWindow::on_actionCredits_triggered() {
    QMessageBox::information(this,"About DroneAndRooms program",
                             "My tiny project.\nCredit Benoît Piranda");
}


void MainWindow::on_actionLoad_triggered() {
    auto fileName = QFileDialog::getOpenFileName(this,tr("Open json description file"), "../../data", tr("JSON Files (*.json)"));
    if (!fileName.isEmpty()) {
        ui->canvas->clear();
        loadJson(fileName);
        ui->canvas->update();
    }
}

