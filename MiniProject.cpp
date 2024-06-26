#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <climits>
#include <stack>
#include <algorithm>

using namespace std;

class Graph 
{
    map<string, map<string, int>> adjList;

public:
    void addEdge(const string& src, const string& dest, int weight) 
    {
        adjList[src][dest] = weight;
        adjList[dest][src] = weight; 
    }

    vector<string> getShortestPath(const string& src, const string& dest) 
    {
        map<string, int> distances;
        map<string, string> previous;
        auto cmp = [&](const string& left, const string& right) { return distances[left] > distances[right]; };
        priority_queue<string, vector<string>, decltype(cmp)> pq(cmp);
        for (const auto& city : adjList) 
        {
            distances[city.first] = INT_MAX;
        }
        distances[src] = 0;
        pq.push(src);
        while (!pq.empty()) 
        {
            string current = pq.top();
            pq.pop();
            for (const auto& neighbor : adjList[current]) 
            {
                int alt = distances[current] + neighbor.second;
                if (alt < distances[neighbor.first]) 
                {
                    distances[neighbor.first] = alt;
                    previous[neighbor.first] = current;
                    pq.push(neighbor.first);
                }
            }
        }
        vector<string> path;
        for (string at = dest; at != src; at = previous[at]) 
        {
            if (previous.find(at) == previous.end()) 
            {
                return {};
            }
            path.push_back(at);
        }
        path.push_back(src);
        reverse(path.begin(), path.end());
        return path;
    }

    int getPathDistance(const string& src, const string& dest) 
    {
        vector<string> path = getShortestPath(src, dest);
        if (path.empty()) return -1;
        int total_distance = 0;
        for (size_t i = 0; i < path.size() - 1; ++i) 
        {
            total_distance += adjList[path[i]][path[i + 1]];
        }
        return total_distance;
    }

    void clearAllEdges() 
    {
        adjList.clear();
    }

    vector<string> getAllVertices() 
    {
        vector<string> vertices;
        for (const auto& city : adjList) 
        {
            vertices.push_back(city.first);
        }
        return vertices;
    }

    bool edgeExists(const string& src, const string& dest) 
    {
        return adjList.count(src) && adjList[src].count(dest);
    }
};

Graph graph;
HWND hwndSource, hwndDest, hwndButton, hwndResult;
HWND hwndNewSrc, hwndNewDest, hwndWeight, hwndAddEdgeButton, hwndCheckEdgeButton;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) 
    {
    case WM_CREATE: 
    {
        // Window dimensions
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right;
        int height = rect.bottom;

        // Find Path section
        CreateWindow("STATIC", "Find Shortest Path", WS_VISIBLE | WS_CHILD | SS_CENTER, width / 2 - 150, 10, 300, 30, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "Source:", WS_VISIBLE | WS_CHILD, 10, 50, 60, 20, hwnd, NULL, NULL, NULL);
        hwndSource = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 80, 50, width - 100, 30, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "Destination:", WS_VISIBLE | WS_CHILD, 10, 90, 90, 50, hwnd, NULL, NULL, NULL);
        hwndDest = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 90, 90, width - 100, 30, hwnd, NULL, NULL, NULL);
        hwndButton = CreateWindow("BUTTON", "Find Path", WS_VISIBLE | WS_CHILD, width / 2 - 60, 130, 120, 40, hwnd, (HMENU)1, NULL, NULL);
        hwndResult = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 10, 180, width - 20, 100, hwnd, NULL, NULL, NULL);

        // Add Edge section
        CreateWindow("STATIC", "Add New Edge", WS_VISIBLE | WS_CHILD | SS_CENTER, width / 2 - 150, 290, 300, 30, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "New Source:", WS_VISIBLE | WS_CHILD, 10, 330, 90, 20, hwnd, NULL, NULL, NULL);
        hwndNewSrc = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 330, width - 120, 30, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "New Destination:", WS_VISIBLE | WS_CHILD, 10, 370, 110, 20, hwnd, NULL, NULL, NULL);
        hwndNewDest = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 370, width - 140, 30, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "Weight (km):", WS_VISIBLE | WS_CHILD, 10, 410, 90, 50, hwnd, NULL, NULL, NULL);
        hwndWeight = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 410, width - 110, 30, hwnd, NULL, NULL, NULL);
        hwndAddEdgeButton = CreateWindow("BUTTON", "Add Edge", WS_VISIBLE | WS_CHILD, width / 2 - 60, 450, 120, 40, hwnd, (HMENU)2, NULL, NULL);

        // Clear All Edges button
        CreateWindow("BUTTON", "Clear All Edges", WS_VISIBLE | WS_CHILD, width / 2 - 60, 500, 120, 40, hwnd, (HMENU)3, NULL, NULL);

        // Display Vertices button
        CreateWindow("BUTTON", "Display Vertices", WS_VISIBLE | WS_CHILD, width / 2 - 60, 550, 120, 40, hwnd, (HMENU)4, NULL, NULL);

        // Check Edge Existence button
        hwndCheckEdgeButton = CreateWindow("BUTTON", "Check Edge Existence", WS_VISIBLE | WS_CHILD, width / 2 - 120, 600, 240, 40, hwnd, (HMENU)5, NULL, NULL);

        return 0;
    }
    case WM_COMMAND: 
    {
        if (LOWORD(wParam) == 1) 
        {
            char source[100], destination[100];
            GetWindowText(hwndSource, source, 100);
            GetWindowText(hwndDest, destination, 100);
            string src = source, dest = destination;
            vector<string> path = graph.getShortestPath(src, dest);
            int distance = graph.getPathDistance(src, dest);
            string result;
            if (path.empty()) 
            {
                result = "No path found from " + src + " to " + dest;
            } 
            else 
            {
                result = "Shortest path from " + src + " to " + dest + ": ";
                for (size_t i = 0; i < path.size(); ++i) 
                {
                    result += path[i];
                    if (i < path.size() - 1) 
                    {
                        result += " -> ";
                    }
                }
                result += "\r\nTotal distance: " + to_string(distance) + " km";
            }
            SetWindowText(hwndResult, result.c_str());
        }
        if (LOWORD(wParam) == 2)
        {
            char newSource[100], newDest[100], weightStr[100];
            GetWindowText(hwndNewSrc, newSource, 100);
            GetWindowText(hwndNewDest, newDest, 100);
            GetWindowText(hwndWeight, weightStr, 100);
            string src = newSource, dest = newDest;
            int weight = atoi(weightStr);
            if (!src.empty() && !dest.empty() && weight > 0) 
            {
                graph.addEdge(src, dest, weight);
                MessageBox(hwnd, "Edge added successfully!", "Success", MB_OK);
            } 
            else 
            {
                MessageBox(hwnd, "Invalid input!", "Error", MB_OK);
            }
        }
        if (LOWORD(wParam) == 3)
        {
            graph.clearAllEdges();
            MessageBox(hwnd, "All edges cleared!", "Success", MB_OK);
        }
        if (LOWORD(wParam) == 4)
        {
            vector<string> vertices = graph.getAllVertices();
            if (vertices.empty())
            {
                MessageBox(hwnd, "No vertices found.", "Information", MB_OK);
            }
            else
            {
                string vertexList = "Vertices in the graph:\n";
                for (const auto& vertex : vertices)
                {
                    vertexList += vertex + "\n";
                }
                MessageBox(hwnd, vertexList.c_str(), "Vertices", MB_OK);
            }
        }
        if (LOWORD(wParam) == 5)
        {
            char checkSource[100], checkDest[100];
            GetWindowText(hwndNewSrc, checkSource, 100);
            GetWindowText(hwndNewDest, checkDest, 100);
            string src = checkSource, dest = checkDest;
            if (!src.empty() && !dest.empty())
            {
                bool exists = graph.edgeExists(src, dest);
                if (exists)
                {
                    MessageBox(hwnd, "Edge exists!", "Information", MB_OK);
                }
                else
                {
                    MessageBox(hwnd, "Edge does not exist.", "Information", MB_OK);
                }
            }
            else
            {
                MessageBox(hwnd, "Please enter both source and destination.", "Error", MB_OK);
            }
        }
        return 0;
    }
    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right;
        int height = rect.bottom;

        // Resize the controls
        MoveWindow(hwndSource, 80, 50, width - 100, 30, TRUE);
        MoveWindow(hwndDest, 90, 90, width - 100, 30, TRUE);
        MoveWindow(hwndButton, width / 2 - 60, 130, 120, 40, TRUE);
        MoveWindow(hwndResult, 10, 180, width - 20, 100, TRUE);
        MoveWindow(hwndNewSrc, 110, 330, width - 120, 30, TRUE);
        MoveWindow(hwndNewDest, 130, 370, width - 140, 30, TRUE);
        MoveWindow(hwndWeight, 100, 410, width - 110, 30, TRUE);
        MoveWindow(hwndAddEdgeButton, width / 2 - 60, 450, 120, 40, TRUE);
        MoveWindow(hwndCheckEdgeButton, width / 2 - 120, 500, 240, 40, TRUE);
        MoveWindow(GetDlgItem(hwnd, 3), width / 2 - 60, 550, 120, 40, TRUE); // Clear All Edges button
        MoveWindow(GetDlgItem(hwnd, 4), width / 2 - 60, 600, 120, 40, TRUE); // Display Vertices button

        return 0;
    }
    case WM_PAINT: 
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY: 
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    const char CLASS_NAME[] = "Sample Window Class";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Graph Shortest Path Finder", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 700,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Pre-defined edges
    graph.addEdge("Delhi", "Mumbai", 1400);
    graph.addEdge("Delhi", "Kolkata", 1500);
    graph.addEdge("Mumbai", "Chennai", 1330);
    graph.addEdge("Chennai", "Bangalore", 350);
    graph.addEdge("Kolkata", "Chennai", 1650);
    graph.addEdge("Kolkata", "Hyderabad", 1500);
    graph.addEdge("Hyderabad", "Bangalore", 570);
    graph.addEdge("Mumbai", "Bangalore", 980);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
