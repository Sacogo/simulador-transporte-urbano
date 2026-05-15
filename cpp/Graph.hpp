#pragma once

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace transport {

// Usamos templates para que la clase sea genérica. 
// Por defecto, los IDs de los nodos serán 'int' y los pesos (ej. distancia, tiempo) serán 'double'.
template <typename NodeId = int, typename Weight = double>
class Graph {
public:
    // Estructura anidada para representar las aristas (conexiones) con su respectivo peso.
    struct Edge {
        NodeId destination;
        Weight weight;
    };

private:
    // Lista de adyacencia: Usamos un unordered_map (Tabla Hash) para búsquedas de nodos en O(1) promedio.
    // Mapea el ID de un nodo a un vector de todas sus aristas salientes (sus vecinos).
    std::unordered_map<NodeId, std::vector<Edge>> adjacency_list;
    
    // Propiedad que define si el grafo es dirigido (un solo sentido) o no dirigido (doble sentido).
    bool directed;

public:
    // Constructor. Por defecto, asumimos que el grafo puede no ser dirigido.
    explicit Graph(bool is_directed = false) : directed(is_directed) {}

    // Agregar un nodo al grafo.
    void add_node(const NodeId& node) {
        // Usamos el método 'contains' (introducido en C++20, presente en C++23)
        // Solo insertamos si el nodo no existe previamente para no sobreescribir sus aristas.
        if (!adjacency_list.contains(node)) {
            adjacency_list[node] = std::vector<Edge>();
        }
    }

    // Agregar una arista entre un nodo origen y un destino, con un peso asociado (ej. distancia).
    void add_edge(const NodeId& source, const NodeId& destination, const Weight& weight) {
        // Aseguramos que ambos nodos existan en la red antes de conectarlos.
        add_node(source);
        add_node(destination);

        // Agregamos la ruta de origen a destino a la lista de adyacencia del origen.
        adjacency_list[source].push_back(Edge{destination, weight});

        // Si la red no es dirigida (ej. una calle bidireccional), también agregamos la ruta de regreso.
        if (!directed) {
            adjacency_list[destination].push_back(Edge{source, weight});
        }
    }

    // Eliminar un nodo del grafo y todas sus conexiones.
    void remove_node(const NodeId& node) {
        // Si el nodo no existe, no hay nada que hacer.
        if (!adjacency_list.contains(node)) return;

        // 1. Eliminar el nodo y todas sus aristas salientes (el registro completo en el mapa).
        adjacency_list.erase(node);

        // 2. Eliminar todas las aristas entrantes a este nodo desde los nodos restantes.
        for (auto& [current_node, edges] : adjacency_list) {
            // Usamos std::erase_if (introducido en C++20/23) para eliminar eficientemente 
            // los elementos de un contenedor que cumplen un predicado o condición.
            std::erase_if(edges, [&node](const Edge& edge) {
                return edge.destination == node;
            });
        }
    }

    // Eliminar una arista específica entre dos nodos.
    void remove_edge(const NodeId& source, const NodeId& destination) {
        // Si el nodo origen existe, buscamos y eliminamos la arista hacia el destino.
        if (adjacency_list.contains(source)) {
            std::erase_if(adjacency_list[source], [&destination](const Edge& edge) {
                return edge.destination == destination;
            });
        }

        // Si es no dirigido, también debemos eliminar la arista de regreso.
        if (!directed && adjacency_list.contains(destination)) {
            std::erase_if(adjacency_list[destination], [&source](const Edge& edge) {
                return edge.destination == source;
            });
        }
    }

    // Obtener los vecinos (aristas salientes) de un nodo.
    // Usamos [[nodiscard]] para indicar al compilador que advierta si se ignora el valor de retorno.
    [[nodiscard]] const std::vector<Edge>& get_neighbors(const NodeId& node) const {
        auto it = adjacency_list.find(node);
        if (it != adjacency_list.end()) {
            return it->second;
        }
        // Lanzamos una excepción si se intenta consultar un nodo que no existe en el grafo.
        throw std::invalid_argument("El nodo no existe en la red de transporte.");
    }

    // Consultar si la red es dirigida.
    // 'noexcept' garantiza que esta función nunca lanzará una excepción.
    [[nodiscard]] bool is_directed() const noexcept {
        return directed;
    }
};

} // namespace transport
