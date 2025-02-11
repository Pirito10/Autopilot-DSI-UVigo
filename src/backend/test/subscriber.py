#
# Zenoh Subscriber - Escucha valores de constantes PID en un tópico
#

import time
import zenoh
import argparse
import json
from zenoh import Reliability, Sample

# --- Argumentos ---
parser = argparse.ArgumentParser(description="Zenoh PID Subscriber")
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
                    help='El tópico al que suscribirse.')
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

# Tópico a suscribirse
topic = args.topic

# --- Suscriptor ---
def main():
    print("Abriendo sesión de Zenoh...")
    session = zenoh.open(conf)

    print(f"Declarando suscriptor en el tópico: '{topic}'")

    def callback(sample: Sample):
        payload = sample.payload.decode("utf-8")
        print(f">> Recibido: {payload}")

    # Declarar el suscriptor con confiabilidad
    sub = session.declare_subscriber(topic, callback, reliability=Reliability.RELIABLE())

    print("Presiona CTRL+C para detener...")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Suscriptor detenido.")
    finally:
        sub.undeclare()
        session.close()

if __name__ == '__main__':
    main()
