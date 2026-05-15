#include <iostream>
#include <string>
#include "Graph.hpp"
#include "Dijkstra.hpp"

using namespace transport;

int main() {
    // Instanciamos un grafo dirigido en el cual los nodos son std::string (ej. nombres de estaciones)
    Graph<std::string, double> city_graph(true);

    // Agregamos conexiones (origen, destino, peso/tiempo)
    city_graph.add_edge("Estacion_A", "Estacion_B", 4.0);
    city_graph.add_edge("Estacion_A", "Estacion_C", 2.0);
    city_graph.add_edge("Estacion_C", "Estacion_B", 1.0); // Atajo! A->C->B toma 3.0 vs 4.0
    city_graph.add_edge("Estacion_B", "Estacion_D", 5.0);
    city_graph.add_edge("Estacion_C", "Estacion_D", 8.0);
    
    // Nodo aislado (simulando un destino inalcanzable)
    city_graph.add_node("Estacion_Aislada");

    std::cout << "=== SIMULADOR DE TRANSPORTE ===\n\n";

    try {
        // CASO NORMAL: Ruta óptima
        std::cout << "[Ruta A -> D]\n";
        auto result = Dijkstra<std::string, double>::shortest_path(city_graph, "Estacion_A", "Estacion_D");
        
        std::cout << "Tiempo/Costo Total: " << result.total_distance << "\n";
        std::cout << "Camino recorrido: ";
        for (const auto& station : result.path) {
            std::cout << station << " -> ";
        }
        std::cout << "Llegada\n\n";

        // CASO BORDE: Destino inalcanzable
        std::cout << "[Ruta A -> Estacion_Aislada]\n";
        Dijkstra<std::string, double>::shortest_path(city_graph, "Estacion_A", "Estacion_Aislada");

    } catch (const std::exception& e) {
        std::cerr << "Error capturado: " << e.what() << "\n\n";
    }

    try {
        // CASO BORDE: Nodo no existe
        std::cout << "[Ruta A -> Estacion_Fantasma]\n";
        Dijkstra<std::string, double>::shortest_path(city_graph, "Estacion_A", "Estacion_Fantasma");
    } catch (const std::exception& e) {
        std::cerr << "Error capturado: " << e.what() << "\n\n";
    }

    return 0;
}
