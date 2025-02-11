from flask import Flask, render_template, request, jsonify
import requests
import threading
import time
import paramiko 
from ssh_terminal import ssh_bp

app = Flask(__name__)

base_url = ""
fetching_data = False
ssh_user = ""
ssh_password = ""

# Registrar el blueprint de SSH Terminal
app.register_blueprint(ssh_bp)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/set-ip', methods=['POST'])
def set_ip():
    global base_url
    base_url = request.json['ip']
    return jsonify({"message": "IP set successfully", "baseUrl": base_url})

@app.route('/get-data', methods=['GET'])
def get_data():
    topic = request.args.get('topic')
    if topic in ['constantes_pid', 'pos_deseada', 'armado_motores', 'motores', 'pid']:
        response = requests.get(f'http://{base_url}:8000/get-data?topic={topic}')
        return jsonify(response.json())
    else:
        return jsonify({"error": "Invalid topic"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)