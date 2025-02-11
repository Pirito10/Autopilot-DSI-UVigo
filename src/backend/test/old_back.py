from flask import Flask, jsonify
from flask_cors import CORS  # Importar CORS
import zenoh
import threading
from zenoh import Reliability, Sample

app = Flask(__name__)
CORS(app)  # Habilitar CORS para todas las rutas

pid_constants = {
    "roll": {"Kp": 0.0, "Ki": 0.0, "Kd": 0.0},
    "pitch": {"Kp": 0.0, "Ki": 0.0, "Kd": 0.0}
}

def zenoh_subscriber():
    global pid_constants  # Usamos la variable global
    session = zenoh.open(zenoh.Config())
    topic = 'constantes_pid'

    def listener(sample: Sample):
        global pid_constants  # Declaramos global para modificar la variable global
        try:
            payload = sample.payload.decode('utf-8')
            values = list(map(float, payload.split(',')))

            pid_constants = {
                "roll": {"Kp": values[0], "Ki": values[1], "Kd": values[2]},
                "pitch": {"Kp": values[3], "Ki": values[4], "Kd": values[5]}
            }
            print(f"Updated PID constants: {pid_constants}")
        except (ValueError, IndexError):
            print("Error parsing payload, invalid data format")

    #Declarar el suscriptor con confiabilidad
    print(f"Subscribing to topic: {topic}")
    sub = session.declare_subscriber(topic, listener, reliability=Reliability.RELIABLE())

@app.route('/pid-constants', methods=['GET'])
def get_pid_constants():
    # print(f"GET")
    return jsonify(pid_constants)

if __name__ == '__main__':
    threading.Thread(target=zenoh_subscriber, daemon=True).start()
    app.run(host='0.0.0.0', port=5000)
