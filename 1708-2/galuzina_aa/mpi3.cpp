#include <time.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <mpi.h>
#include <vector>
#include <time.h>
#include <stdexcept>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <random>
#include <ctime>
 
using namespace std;
 
const int infinity = 999999999;
enum GraphType { UNDIRECTED, DIRECTED };
 
class Graph {
private:
    std::vector<int> adjacency;
    GraphType type;
public:
    explicit Graph(const int& vsize = 1, GraphType _type = UNDIRECTED);
    std::vector<int> getAdjacency() const { return adjacency; }
    GraphType getType() const { return type; }
    void setAdjacency(const std::vector<int> _adj);
};
 
Graph::Graph(const int& vsize, GraphType _type) : type(_type) {
    if (vsize <= 0)
        throw - 1;
    adjacency.resize(vsize*vsize);
}
 
void Graph::setAdjacency(const std::vector<int> _adj) {
    if (_adj.size() != adjacency.size())
        throw - 1;
    type = UNDIRECTED;
    int n = sqrt(static_cast<int>(_adj.size()));
    if (n*n != static_cast<int>(_adj.size()))
        throw - 1;
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            if (_adj[j + i * n] != _adj[i + j * n]) {
                type = DIRECTED;
                break;
            }
        }
    }
    adjacency = _adj;
}
 
std::vector<int> createRandomGraph(const int& n, GraphType type) {
    if (n <= 0)
        throw - 1;
    std::default_random_engine random;
    random.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> adjacency = std::vector<int>(n*n);
    for (int i = 0; i < n; ++i) {
        for (int j = (type == 0) ? i : 0; j < n; ++j) {
            if (i == j) {
                adjacency[j + i * n] = 0;
            }
            else {
                adjacency[j + i * n] = random() % 100;
                if (adjacency[j + i * n] == 0) {
                    adjacency[j + i * n] = infinity;
                }
                if (type == 0) {
                    adjacency[i + j * n] = adjacency[j + i * n];
                }
            }
        }
    }
    return adjacency;
}
 
 
std::vector<int> dijkstraSequential(const Graph& graph, const int& src) {
    int n = sqrt(static_cast<int>(graph.getAdjacency().size()));
    if (src < 0 || src >= n)
        throw - 1;
    std::vector<int> adj = graph.getAdjacency();
    std::vector<int> dist(n, infinity);
    std::vector<int> marked(n, 0);
    dist[src] = 0;
    for (int i = 0; i < n - 1; ++i) {
        int vertex = minDistanceVertex(dist, marked);
        marked[vertex] = 1;
        for (int j = 0; j < n; ++j) {
            if (!marked[j] && adj[j + vertex * n] != infinity
                && (dist[vertex] + adj[j + vertex * n] < dist[j])) {
                dist[j] = dist[vertex] + adj[j + vertex * n];
            }
        }
    }
    return dist;
}
 
std::vector<int> dijkstraParallel(const Graph& graph, const int& src) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = sqrt(static_cast<int>(graph.getAdjacency().size()));
    if (src < 0 || src >= n)
        throw - 1;
    int delta = n / size;
    int mod = n % size;
    int localmod = (rank == 0) ? n % size : 0;
    std::vector<int> sendgraph(n*n);
    std::vector<int> loc_adjacency(delta*n);
 
    if (rank == 0) {
        if (graph.getType() == 0)
            sendgraph = graph.getAdjacency();
        else
            sendgraph = getTransposeAdjacency(graph.getAdjacency());
    }
    if (delta != 0)
        MPI_Scatter(&sendgraph[mod*n], delta*n, MPI_INT, &loc_adjacency[0], delta*n, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        loc_adjacency.insert(loc_adjacency.begin(), sendgraph.begin(), sendgraph.begin() + mod * n);
    }
    std::vector<int> locdist(delta + localmod, infinity);
    std::vector<int> locmarked(delta + localmod, 0);
 
    for (int i = 0; i < delta + localmod; ++i) {
        locdist[i] = loc_adjacency[src + i * n];
        if (locdist[i] == 0)
            locmarked[i] = 1;
    }
 
    struct {
        int value;
        int index;
    } in, out;
 
    for (int count = 0; count < n - 1; ++count) {
        int locindex = minDistanceVertex(locdist, locmarked);
        if (locindex != -1) {
            in.index = mod - localmod + locindex + rank * (delta + localmod);
            in.value = locdist[locindex];
        }
        else {
            in.index = -1;
            in.value = infinity;
        }
        MPI_Allreduce(&in, &out, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);
        if (out.index == -1)
            break;
        if (in.index == out.index && locindex != -1) {
            locmarked[locindex] = 1;
        }
        for (int i = 0; i < delta + localmod; ++i) {
            if (!locmarked[i] && loc_adjacency[out.index + i * n] != infinity
                && (out.value + loc_adjacency[out.index + i * n] < locdist[i])) {
                locdist[i] = out.value + loc_adjacency[out.index + i * n];
            }
        }
    }
    std::vector<int> res;
    if (rank == 0) {
        MPI_Status status;
        std::vector<int> recv(delta);
        res.insert(res.begin(), locdist.begin(), locdist.end());
        for (int i = 1; i < size; ++i) {
            MPI_Recv(&recv[0], delta, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
            res.insert(res.end(), recv.begin(), recv.end());
        }
    }
    else {
        MPI_Send(&locdist[0], delta, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
    return res;
}
 
int minDistanceVertex(const std::vector<int>& dist, const std::vector<int>& marker) {
    int minvalue = infinity;
    int res = -1;
    for (int i = 0; i < static_cast<int>(dist.size()); ++i) {
        if (marker[i] == 0 && dist[i] <= minvalue) {
            minvalue = dist[i];
            res = i;
        }
    }
    return res;
}
 
std::vector<int> getTransposeAdjacency(const std::vector<int>& mat) {
    int n = sqrt(static_cast<int>(mat.size()));
    std::vector<int> tmp(n*n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            tmp[i + j * n] = mat[j + i * n];
        }
    }
    return tmp;
}
 
int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int nv = 500;
    Graph graph(nv);
    std::vector<int> seqres;
    std::vector<int> parres;
    if (rank == 0) {
        graph.setAdjacency(createRandomGraph(nv, UNDIRECTED));
        seqres = dijkstraSequential(graph, 0);
    }
    parres = dijkstraParallel(graph, 0);
    if (rank == 0) {
        for (int i = 0; i < nv; i++) {
            cout <<"seq" << seqres[i];
        }
    }
    if (rank == 0) {
        for (int i = 0; i < nv; i++) {
            cout<<"par" << parres[i];
        }
    }
    MPI_Finalize();
    return 0;
}