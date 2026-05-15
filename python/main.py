import json
import subprocess
import os

def calcular_ruta_con_cpp():
    # 1. Definimos la estructura del transporte a consultar
    # Simplificamos las aristas a una lista de listas: [origen, destino, peso]
    # para que sea muy fácil de parsear por el C++ con std::regex
    datos = {
        "directed": True,
        "start": "Estacion_A",
        "destination": "Estacion_C",
        "edges": [
            ["Estacion_A", "Estacion_B", 4.0],
            ["Estacion_A", "Estacion_C", 10.0],
            ["Estacion_B", "Estacion_C", 2.0]
        ]
    }

    # Convertimos los datos de Python a un string JSON estándar
    json_data = json.dumps(datos)

    # 2. Rutas al ejecutable compilado
    cpp_executable = os.path.join(os.path.dirname(__file__), "..", "cpp", "python_bridge.exe")

    try:
        # 3. Comunicarse con el ejecutable C++ usando stdin y stdout
        proceso = subprocess.Popen(
            [cpp_executable],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True # Permite enviar strings en lugar de bytes puros
        )

        # Enviamos el JSON al programa C++ y capturamos la respuesta
        stdout_data, stderr_data = proceso.communicate(input=json_data)

        if proceso.returncode != 0:
            print(f"Error fatal de C++: {stderr_data}")
            return

        # 4. Parseamos el JSON retornado por el stdout de C++
        resultado_cpp = json.loads(stdout_data)

        # Analizamos el resultado
        if resultado_cpp.get("status") == "success":
            print(f"✅ RUTA ENCONTRADA EN C++!")
            print(f"Distancia Total: {resultado_cpp['total_distance']}")
            print(f"Camino: {' -> '.join(resultado_cpp['path'])}")
        else:
            print(f"❌ ERROR EN EL CÁLCULO: {resultado_cpp.get('message')}")

    except FileNotFoundError:
        print(f"Ejecutable no encontrado. Asegúrate de compilar 'python_bridge.cpp' como '{cpp_executable}'")

if __name__ == "__main__":
    calcular_ruta_con_cpp()
