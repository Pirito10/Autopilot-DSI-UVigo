from flask import Flask, jsonify, request
from flask_cors import CORS
import zenoh
import threading
import time
from zenoh import Reliability, Sample

app = Flask(__name__)
CORS(app)

# Variables globales para almacenar los datos de los tópicos
pid_constants = {
    "roll": {"Kp": 0.0, "Ki": 0.0, "Kd": 0.0},
    "pitch": {"Kp": 0.0, "Ki": 0.0, "Kd": 0.0}
    }
pos_deseada = {
    "throttle": 0.0, "roll": 0.0, "pitch": 0.0, "yaw": 0.0
    }
armado_motores = {
    "throttle": 0.0, "switchC": 0.0, "switchD": 0.0
    }
motores = {
    "motor_RL": 0.0, "motor_FR": 0.0, "motor_RR": 0.0, "motor_FL": 0.0
    }
pid = {
    "u_roll": 0.0, "u_pitch": 0.0
    }

def zenoh_subscriber(topic, callback):
    conf = zenoh.Config()
    session = zenoh.open(conf)
    sub = session.declare_subscriber(topic, callback, reliability=Reliability.RELIABLE())
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        sub.undeclare()
        session.close()

def constantes_pid_callback(sample: Sample):
    global pid_constants
    payload = sample.payload.decode("utf-8")
    values = list(map(float, payload.split(',')))
    pid_constants = {
        "roll": {"Kp": values[0], "Ki": values[1], "Kd": values[2]},
        "pitch": {"Kp": values[3], "Ki": values[4], "Kd": values[5]}
    }
    print(f"Updated PID constants: {pid_constants}")

def pos_deseada_callback(sample: Sample):
    global pos_deseada
    payload = sample.payload.decode("utf-8")
    values = list(map(float, payload.split(',')))
    pos_deseada = {
        "throttle": values[0],
        "roll": values[1],
        "pitch": values[2],
        "yaw": values[3]
    }
    print(f"Updated pos_deseada: {pos_deseada}")

def armado_motores_callback(sample: Sample):
    global armado_motores
    payload = sample.payload.decode("utf-8")
    values = list(map(float, payload.split(',')))
    armado_motores = {
        "throttle": values[0],
        "switchC": values[1],
        "switchD": values[2]
    }
    print(f"Updated armado_motores: {armado_motores}")

def motores_callback(sample: Sample):
    global motores
    payload = sample.payload.decode("utf-8")
    values = list(map(float, payload.split(',')))
    motores = {
        "motor_RL": values[0],
        "motor_FR": values[1],
        "motor_RR": values[2],
        "motor_FL": values[3]
    }
    print(f"Updated motores: {motores}")

def pid_callback(sample: Sample):
    global pid
    payload = sample.payload.decode("utf-8")
    values = list(map(float, payload.split(',')))
    pid = {
        "u_roll": values[0],
        "u_pitch": values[1]
    }
    print(f"Updated pid: {pid}")

@app.route('/get-data', methods=['GET'])
def get_data():
    topic = request.args.get('topic')
    if topic == 'constantes_pid':
        return jsonify(pid_constants)
    elif topic == 'pos_deseada':
        return jsonify(pos_deseada)
    elif topic == 'armado_motores':
        return jsonify(armado_motores)
    elif topic == 'motores':
        return jsonify(motores)
    elif topic == 'pid':
        return jsonify(pid)
    else:
        return jsonify({"error": "Invalid topic"}), 400

if __name__ == '__main__':
    # Crear hilos para cada suscripción
    threading.Thread(target=zenoh_subscriber, args=('constantes_pid', constantes_pid_callback), daemon=True).start()
    threading.Thread(target=zenoh_subscriber, args=('pos_deseada', pos_deseada_callback), daemon=True).start()
    threading.Thread(target=zenoh_subscriber, args=('armado_motores', armado_motores_callback), daemon=True).start()
    threading.Thread(target=zenoh_subscriber, args=('motores', motores_callback), daemon=True).start()
    threading.Thread(target=zenoh_subscriber, args=('pid', pid_callback), daemon=True).start()

    app.run(host='0.0.0.0', port=8000)