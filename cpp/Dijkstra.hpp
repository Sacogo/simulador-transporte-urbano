#pragma once

#include "Graph.hpp"
#include <vector>
#include <unordered_map>
#include <queue>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <functional>

namespace transport {

// Clase separada que implementa algoritmos de ruteo sobre nuestra clase Graph
template <typename NodeId = int, typename Weight = double>
class Dijkstra {
public:
    // Estructura para retornar ordenadamente los resultados
    struct Result {
        Weight total_distance;
        std::vector<NodeId> path;
    };

    // Método estático: no requiere instanciar la clase Dijkstra. Recibe el grafo por referencia const.
    [[nodiscard]] static Result shortest_path(const Graph<NodeId, Weight>& graph, 
                                              const NodeId& start, 
                                              const NodeId& destination) {
        
        // 1er Caso Borde: Nodo origen o destino no existe.
        // graph.get_neighbors lanzará std::invalid_argument si alguno de los nodos no está en el grafo.
        try {
            graph.get_neighbors(start);
            graph.get_neighbors(destination);
        } catch (const std::invalid_argument&) {
            throw std::invalid_argument("El nodo origen o el nodo destino no existen en el grafo.");
        }

        // Si el origen es igual al destino, la distancia es 0 y el camino es el mismo nodo.
        if (start == destination) {
            return Result{Weight{0}, {start}};
        }

        // Tabla de distancias mínimas conocidas desde el origen hasta cada nodo.
        std::unordered_map<NodeId, Weight> distances;
        
        // Mapa para reconstruir el camino. Guarda: {nodo_actual -> nodo_previo}
        std::unordered_map<NodeId, NodeId> previous;
        
        // Priority Queue (Min-Heap) para procesar siempre el nodo más cercano descubierto.
        // Almacenamos std::pair<Weight, NodeId>. La cola de prioridad ordena por el primer elemento del par.
        using PQueueElement = std::pair<Weight, NodeId>;
        std::priority_queue<PQueueElement, std::vector<PQueueElement>, std::greater<PQueueElement>> pq;

        // Inicializamos el algoritmo con el nodo de origen.
        distances[start] = Weight{0};
        pq.push({Weight{0}, start});

        while (!pq.empty()) {
            // Extraer el nodo con menor distancia actual (O(log V))
            auto [current_dist, current_node] = pq.top();
            pq.pop();

            // Optimización: Early Exit si llegamos al destino.
            if (current_node == destination) {
                break;
            }

            // Ignorar caminos obsoletos en la cola de prioridad si ya encontramos uno mejor.
            if (distances.contains(current_node) && current_dist > distances[current_node]) {
                continue;
            }

            // Explorar todos los vecinos del nodo actual (O(E))
            const auto& neighbors = graph.get_neighbors(current_node);
            for (const auto& edge : neighbors) {
                Weight new_dist = current_dist + edge.weight;
                
                // Si es la primera vez que vemos este nodo, o encontramos un camino más corto:
                if (!distances.contains(edge.destination) || new_dist < distances[edge.destination]) {
                    distances[edge.destination] = new_dist;
                    previous[edge.destination] = current_node;
                    // Agregar a la cola de prioridad (O(log V))
                    pq.push({new_dist, edge.destination});
                }
            }
        }

        // 2do Caso Borde: Destino inalcanzable / Grafo desconectado.
        // Si el destino nunca fue agregado al mapa 'distances', significa que no existe ruta.
        if (!distances.contains(destination)) {
            throw std::runtime_error("El destino es inalcanzable desde el origen (grafo desconectado).");
        }

        // Reconstruir el camino desde el destino hacia el origen usando el mapa 'previous'
        std::vector<NodeId> path;
        for (NodeId at = destination; ; at = previous[at]) {
            path.push_back(at);
            if (at == start) break;
        }

        // Invertir el vector, ya que lo armamos desde el destino hacia el origen.
        // Usamos std::ranges::reverse (C++20/23) para mayor claridad.
        std::ranges::reverse(path);

        return Result{distances[destination], path};
    }
};

} // namespace transport
