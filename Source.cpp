#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <utility>
#include "RTree.h"
#include <ctime>
#include <SFML/Graphics.hpp>
using namespace std;

void print(const int& command, vector<vector<vector<pair<int, int>>>>& objects_n, string& output) {
    output.resize(0);
    output = to_string(command);

    switch (command)
    {
    case 0:// OBJECTS
        output += "|" + to_string(objects_n[0].size());
        for (auto& polygon : objects_n[0]) {
            output += "|" + to_string(polygon.size());

            for (auto& point : polygon) {
                output += "|" + to_string(point.first) + "|" + to_string(point.second);
            }
        }
        break;

    case 1: //MBRS:
        cout << " number of (objects) :" << to_string(objects_n.size()) << endl;
        output += "|" + to_string(objects_n.size());
        for (auto& objects : objects_n) {
            output += "|" + to_string(objects.size());
            cout << " number of polygons :" << to_string(objects.size()) << endl;
            for (auto& polygon : objects) {
                output += "|" + to_string(polygon.size());
                cout << " number of points :" << to_string(polygon.size()) << endl;
                for (auto& point : polygon) {
                    output += "|" + to_string(point.first) + "|" + to_string(point.second);
                    cout << "   point:" << to_string(point.first) << " | " << to_string(point.second) << endl;
                }
            }
            cout << endl << "--------------------" << endl;
        }
        break;
    default:
        output += "|0";
        break;
    }

    output += "|END";
}

void print_pair(vector<pair<int, int>> output) {
    for (auto& x : output)
    {
        cout << " ( " << x.first << " , " << x.second << " ) ";
    }
}

int resolucion = 5;
sf::RectangleShape RecDraw(const Rect& R, int windowHeight = 500, sf::Color c = sf::Color::Blue, float tam = 1) {
    int xtam = R.m_max[0] - R.m_min[0];
    int ytam = R.m_max[1] - R.m_min[1];
    sf::RectangleShape r({ static_cast<float>(resolucion * xtam), static_cast<float>(resolucion * ytam) });
    r.setPosition(sf::Vector2f(static_cast<float>(resolucion * R.m_min[0]), static_cast<float>(windowHeight - resolucion * R.m_max[1])));
    sf::Color transparentColor = sf::Color(0, 0, 0, 0);
    r.setFillColor(transparentColor);
    sf::Color outlineColor = c;
    r.setOutlineColor(outlineColor);
    r.setOutlineThickness(tam);
    return r;
}


sf::RenderWindow window(sf::VideoMode(700, 500), "R-Tree");


void PointDraw(vector<pair<int, int>> V) {
    for (int i = 0; i < V.size(); i++) {
        sf::CircleShape pointShape(3);
        pointShape.setFillColor(sf::Color::White);
        pointShape.setPosition({ static_cast<float>(resolucion * V[i].first - 3), static_cast<float>(500 - resolucion * V[i].second - 3) });
        window.draw(pointShape);
    }
}

bool Overlaping(const Rect* a_rectA, const Rect* a_rectB)
{
    bool overlapX = (a_rectA->m_min[0] <= a_rectB->m_max[0] &&
        a_rectA->m_max[0] >= a_rectB->m_min[0]);

    bool overlapY = (a_rectA->m_min[1] <= a_rectB->m_max[1] &&
        a_rectA->m_max[1] >= a_rectB->m_min[1]);

    return overlapX && overlapY;
}

void Search(Node* a_node, Rect& Busq, vector<Rect>& result)
{
    if (a_node->IsInternalNode())
    {
        
        for (int i = 0; i < a_node->m_count; ++i)
        {
            if (Overlaping(&Busq, &(a_node->m_branch[i].m_rect)))
            {
                Search(a_node->m_branch[i].m_child, Busq, result);
            }
        }
    }
    else
    {
        for (int i = 0; i < a_node->m_count; ++i)
        {
            if (Overlaping(&Busq, &(a_node->m_branch[i].m_rect))) 
            {
                result.push_back(a_node->m_branch[i].m_rect); 
            }
        }
    }
}


void PrintRectangles(const vector<Rect>& result)
{
    cout << "Rectangles found:" << endl;
    for (const auto& rect : result)
    {
        cout << "Rect: (" << rect.m_min[0] << ", " << rect.m_min[1] << ") - (" << rect.m_max[0] << ", " << rect.m_max[1] << ")" << endl;
    }
}



int main(int argc, char* argv[])
{
    vector<vector<pair<int, int>>> vpoints;

    //   Primer Test

    vector<pair<int, int>> points = { {20, 59}, {20, 43}, {50, 58}, {48, 67}, {105, 68}, {74, 64}, {83, 40}, {104, 54} };

    for (unsigned int i = 0; i < points.size(); i += 2) {
        vector<pair<int, int>> sub1(points.begin() + i, points.begin() + i + 2);
        vpoints.push_back(sub1);
    }
    vector<pair<int, int>> points2 = { {12, 28}, {19, 15}, {40, 29}, {69, 25}, {70, 28}, {60, 15} };
    for (unsigned int i = 0; i < points2.size(); i += 3) {
        vector<pair<int, int>> sub1(points2.begin() + i, points2.begin() + i + 3);
        vpoints.push_back(sub1);
    }


    //Insteramos puntos aleatorios
    int n_rand = 2;
    for(int d=0;d<n_rand;d++)
    {
        vector<pair<int, int>> pointPair(2);
        for (int i = 0; i < 2; ++i) {
            pointPair[i] = make_pair(rand() % 90, rand() % 100); 
        }
        vpoints.push_back(pointPair);

        vector<pair<int, int>> pointPair2(3);
        for (int i = 0; i < 3; ++i) {
            pointPair2[i] = make_pair(rand() % 90, 10 + rand() % 80);
        }
        vpoints.push_back(pointPair2);
    }
    
    

    RTree rtree;

    string output;
    vector<vector<pair<int, int>>> objects;
    vector<vector<vector<pair<int, int>>>> objects_n;

    for (auto& x : vpoints)
    {
        cout << "inserting " << x.size() << ": ";
        print_pair(x);
        Rect rect = rtree.MBR(x);
        rtree.Insert(rect.m_min, rect.m_max, x);
        cout << endl;
    }

    rtree.getMBRs(objects_n);
    print(1, objects_n, output);

    cout << " INSERTING ONE MORE " << endl;
    vector<pair<int, int>> ad = { {53, 4}, {54, 12} };

    cout << "inserting " << ad.size() << ": ";
    print_pair(ad);
    Rect rect = rtree.MBR(ad);
    rtree.Insert(rect.m_min, rect.m_max, ad);
    cout << endl;

    rtree.getMBRs(objects_n);
    print(1, objects_n, output);

    cout << " REMOVING LAST INSERTED " << endl;
    print_pair(ad);
    rtree.Remove(rect.m_min, rect.m_max, ad);
    cout << endl;

    rtree.getMBRs(objects_n);
    print(1, objects_n, output);

    Rect searchRect(30, 30, 60, 70);  // Definir el rectángulo de búsqueda
    vector<Rect> searchResult;  // Vector para almacenar los resultados

    // Llamar a la función de búsqueda
    Search(rtree.m_root, searchRect, searchResult);
    PrintRectangles(searchResult);

    sf::RectangleShape background({ 0, 0 });
    window.setFramerateLimit(0);
    background.setFillColor(sf::Color::White);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(background);


        for (auto& it : rtree.mObjs) {	
        	window.draw(RecDraw(Rect(rtree.MBR(it))));
        	PointDraw(it);
        }

        /*for (auto& it : searchResult) {// graficador de consultas
        	window.draw(RecDraw(it,500,sf::Color::Red,2));
        }
        window.draw(RecDraw(searchRect, 500, sf::Color::Yellow,3));*/

        window.display();
    }

    return 0;
}
