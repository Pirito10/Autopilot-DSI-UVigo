#
# Zenoh Publisher - Publica valores de constantes PID en un tópico
#

import time
import random
import zenoh
import argparse
import json

# --- Argumentos ---
parser = argparse.ArgumentParser(description="Zenoh PID Publisher")
parser.add_argument('--mode', '-m', dest='mode',
                    choices=['peer', 'client'],
                    type=str,
                    help='El modo de la sesión Zenoh.')
parser.add_argument('--connect', '-e', dest='connect',
                    metavar='ENDPOINT',
                    action='append',
                    type=str,
                    help='Endpoints para conectarse.')
parser.add_argument('--listen', '-l', dest='listen',
                    metavar='ENDPOINT',
                    action='append',
                    type=str,
                    help='Endpoints para escuchar.')
parser.add_argument('--topic', '-t', dest='topic',
                    default='constantes_pid',
                    type=str,
                    help='El tópico donde se publicarán los valores.')
parser.add_argument('--config', '-c', dest='config',
                    metavar='FILE',
                    type=str,
                    help='Archivo de configuración de Zenoh.')

args = parser.parse_args()

# Configuración de Zenoh
conf = zenoh.Config.from_file(args.config) if args.config is not None else zenoh.Config()
if args.mode is not None:
    conf.insert_json5(zenoh.config.MODE_KEY, json.dumps(args.mode))
if args.connect is not None:
    conf.insert_json5(zenoh.config.CONNECT_KEY, json.dumps(args.connect))
if args.listen is not None:
    conf.insert_json5(zenoh.config.LISTEN_KEY, json.dumps(args.listen))

# Tópico donde publicar
topic = args.topic

# --- Publicador ---
def main():
    print("Abriendo sesión de Zenoh...")
    session = zenoh.open(conf)

    print(f"Declarando publicador en el tópico: '{topic}'")
    pub = session.declare_publisher(topic)

    print("Presiona CTRL+C para detener...")
    try:
        while True:
            # Generar valores aleatorios para las constantes PID
            roll_Kp = round(random.uniform(0, 10), 2)
            roll_Ki = round(random.uniform(0, 10), 2)
            roll_Kd = round(random.uniform(0, 10), 2)
            pitch_Kp = round(random.uniform(0, 10), 2)
            pitch_Ki = round(random.uniform(0, 10), 2)
            pitch_Kd = round(random.uniform(0, 10), 2)

            # Preparar y publicar el payload
            payload = f"{roll_Kp},{roll_Ki},{roll_Kd},{pitch_Kp},{pitch_Ki},{pitch_Kd}"
            pub.put(payload)
            print(f"Publicado: {payload}")

            # Esperar 1 segundo antes del siguiente mensaje
            time.sleep(1)
    except KeyboardInterrupt:
        print("Publicador detenido.")
    finally:
        pub.undeclare()
        session.close()

if __name__ == '__main__':
    main()
