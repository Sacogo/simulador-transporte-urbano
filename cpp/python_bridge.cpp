#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include "Graph.hpp"
#include "Dijkstra.hpp"

using namespace transport;

// Estructura de entrada esperada desde Python
struct Request {
    bool directed = false;
    std::string start;
    std::string destination;
};

int main() {
    // 1. LEER LA ENTRADA DESDE STDIN
    // Almacenamos todo lo que Python nos manda en un std::string
    std::ostringstream ss;
    ss << std::cin.rdbuf();
    std::string json_input = ss.str();

    // Si no hay datos, terminamos de forma silenciosa
    if (json_input.empty()) return 0;

    Request req;
    
    // 2. PARSEAR JSON CON STL (usando std::regex introducido en C++11)
    // C++ no tiene un módulo JSON nativo. Para no depender de nlohmann/json u otros,
    // usaremos expresiones regulares asumiendo un esquema controlado de JSON generado por Python.
    
    // Extraer boolean 'directed'
    std::regex directed_regex(R"("directed"\s*:\s*(true|false))");
    std::smatch match;
    if (std::regex_search(json_input, match, directed_regex)) {
        req.directed = (match[1].str() == "true");
    }

    // Extraer strings 'start' y 'destination'
    std::regex start_regex(R"("start"\s*:\s*"([^"]+)")");
    if (std::regex_search(json_input, match, start_regex)) {
        req.start = match[1].str();
    }

    std::regex dest_regex(R"("destination"\s*:\s*"([^"]+)")");
    if (std::regex_search(json_input, match, dest_regex)) {
        req.destination = match[1].str();
    }

    // Instanciar el grafo
    Graph<std::string, double> graph(req.directed);

    // Extraer la lista de aristas
    // Python mandará las aristas en este formato comprimido: ["Estacion_A", "Estacion_B", 4.5]
    std::regex edge_regex(R"(\[\s*"([^"]+)"\s*,\s*"([^"]+)"\s*,\s*([0-9]+(?:\.[0-9]+)?)\s*\])");
    
    // Iterador de regex para buscar todas las aristas globalmente en el JSON
    auto edges_begin = std::sregex_iterator(json_input.begin(), json_input.end(), edge_regex);
    auto edges_end = std::sregex_iterator();

    for (std::sregex_iterator i = edges_begin; i != edges_end; ++i) {
        std::smatch edge_match = *i;
        std::string source = edge_match[1].str();
        std::string dest = edge_match[2].str();
        double weight = std::stod(edge_match[3].str());
        
        graph.add_edge(source, dest, weight);
    }

    // 3. EJECUTAR DIJKSTRA Y SALIDA
    try {
        auto result = Dijkstra<std::string, double>::shortest_path(graph, req.start, req.destination);
        
        // Imprimimos la salida en formato JSON estricto directamente a STDOUT
        std::cout << "{\n";
        std::cout << "  \"status\": \"success\",\n";
        std::cout << "  \"total_distance\": " << result.total_distance << ",\n";
        std::cout << "  \"path\": [";
        for (size_t i = 0; i < result.path.size(); ++i) {
            std::cout << "\"" << result.path[i] << "\"";
            if (i < result.path.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
        std::cout << "}\n";

    } catch (const std::exception& e) {
        // Manejamos cualquier error estructural para que Python pueda leer un json de error en vez de colapsar
        std::cout << "{\n";
        std::cout << "  \"status\": \"error\",\n";
        std::cout << "  \"message\": \"" << e.what() << "\"\n";
        std::cout << "}\n";
    }

    return 0;
}
