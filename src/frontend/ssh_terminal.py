########################
##### SSH Terminal #####
########################

from flask import Blueprint, render_template, request, jsonify
import paramiko
import select
import threading
import time
import re

ssh_bp = Blueprint('ssh_terminal', __name__)

class SSHConnection:
    def __init__(self):
        self.client = None
        self.channel = None
        self.host = None
        self.username = None
        self.password = None
        self.output_buffer = []
        self.is_running = False
        self.current_command = None
        self._output_thread = None

    def connect(self, host, username, password):
        try:
            self.client = paramiko.SSHClient()
            self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            self.client.connect(hostname=host, username=username, password=password)
            self.channel = self.client.invoke_shell()
            self.host = host
            self.username = username
            self.password = password
            return True
        except Exception as e:
            return str(e)

    def _read_output(self):
        ansi_escape = re.compile(r'\x1B[@-_][0-?]*[ -/]*[@-~]')
        while self.is_running:
            if self.channel and self.channel.recv_ready():
                data = self.channel.recv(4096).decode('utf-8')
                clean_data = ansi_escape.sub('', data)
                self.output_buffer.append(clean_data)
            time.sleep(0.1)

    def execute(self, command):
        if not self.channel:
            return "No hay conexión SSH activa"
        
        try:
            self.output_buffer = []
            self.is_running = True
            self.current_command = command
            
            if not self._output_thread or not self._output_thread.is_alive():
                self._output_thread = threading.Thread(target=self._read_output)
                self._output_thread.daemon = True
                self._output_thread.start()
            
            self.channel.send(command + "\n")
            return "Comando iniciado"
        except Exception as e:
            return str(e)

    def get_output(self):
        if self.output_buffer:
            output = ''.join(self.output_buffer)
            self.output_buffer = []
            return output
        return ""

    def stop_command(self):
        if self.channel:
            try:
                self.channel.send("\x03")  # Envía Ctrl+C
                return "Señal de interrupción enviada"
            except:
                return "Error al enviar señal de interrupción"
        return "No hay conexión activa"

    def close(self):
        self.is_running = False
        if self._output_thread:
            self._output_thread.join(timeout=1)
        if self.channel:
            self.channel.close()
        if self.client:
            self.client.close()

ssh_connection = SSHConnection()

@ssh_bp.route('/terminal')
def terminal():
    return render_template('terminal.html')

@ssh_bp.route('/connect', methods=['POST'])
def connect():
    data = request.json
    result = ssh_connection.connect(
        data['host'],
        data['username'],
        data['password']
    )
    return jsonify({"status": "success" if result is True else "error", 
                   "message": "Conexión establecida" if result is True else str(result)})

@ssh_bp.route('/execute', methods=['POST'])
def execute():
    command = request.json.get('command')
    output = ssh_connection.execute(command)
    return jsonify({"output": output})

@ssh_bp.route('/get-output', methods=['GET'])
def get_output():
    output = ssh_connection.get_output()
    return jsonify({"output": output})

@ssh_bp.route('/stop-command', methods=['POST'])
def stop_command():
    result = ssh_connection.stop_command()
    return jsonify({"message": result})

@ssh_bp.route('/disconnect', methods=['POST'])
def disconnect():
    ssh_connection.close()
    return jsonify({"status": "success", "message": "Conexión cerrada"})