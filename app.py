from flask import Flask, request, jsonify, render_template
from werkzeug.security import generate_password_hash, check_password_hash
import subprocess
import json
import os
import sqlite3

app = Flask(__name__)

# ==========================================
# 1. DATABASE SETUP (Secure Users Table)
# ==========================================
def init_db():
    conn = sqlite3.connect('users.db')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS users (
                    username TEXT UNIQUE, 
                    password TEXT
                 )''')
    conn.commit()
    conn.close()

init_db()

# ==========================================
# 2. WEB ROUTES
# ==========================================
@app.route('/')
def home():
    return render_template('index.html')

@app.route('/login', methods=['POST'])
def login():
    data = request.json
    username = data.get('username', '').strip()
    password = data.get('password', '').strip()
    
    if not username or not password:
        return jsonify({"error": "Username and password required"}), 400

    conn = sqlite3.connect('users.db')
    c = conn.cursor()
    
    c.execute("SELECT * FROM users WHERE username=?", (username,))
    user = c.fetchone()

    if user:
        stored_hash = user[1]
        if check_password_hash(stored_hash, password):
            conn.close()
            return jsonify({"status": "old", "message": f"Welcome back, {username}!"})
        else:
            conn.close()
            return jsonify({"error": "Incorrect password. Please try again."}), 401
    else:
        hashed_pw = generate_password_hash(password)
        try:
            c.execute("INSERT INTO users (username, password) VALUES (?, ?)", (username, hashed_pw))
            conn.commit()
            conn.close()
            return jsonify({"status": "new", "message": f"Welcome, {username}! Profile created securely."})
        except sqlite3.Error:
            conn.close()
            return jsonify({"error": "Database error during registration."}), 500

@app.route('/calculate', methods=['POST'])
def calculate():
    data = request.json
    
    # Extract variables matching the frontend inputs for the C++ engine
    args = [
        str(data.get('people', 1)),
        str(data.get('diet_type', 'veg')),
        str(data.get('meals_per_day', 3)),
        str(data.get('electricity_kwh', 0)),
        str(data.get('lpg_kg', 0)),
        str(data.get('screen_hours', 0)),
        str(data.get('data_gb', 0)),
        str(data.get('car_km', 0)),
        str(data.get('bike_km', 0)),
        str(data.get('metro_km', 0)),
        str(data.get('bus_km', 0)),
        str(data.get('flight_km', 0))
    ]

    exe_path = os.path.join(os.path.dirname(__file__), 'backend.exe')

    if not os.path.exists(exe_path):
        return jsonify({"error": "C++ Backend Executable not found! Compile it first."}), 500

    try:
        result = subprocess.run([exe_path] + args, capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"C++ Error: {result.stderr}") 
            return jsonify({"error": "C++ Backend Processing Failed."}), 500

        output_data = json.loads(result.stdout)
        return jsonify(output_data)

    except json.JSONDecodeError:
        return jsonify({"error": "Invalid data returned from C++ backend."}), 500
    except Exception as e:
        return jsonify({"error": f"Server Bridge Error: {str(e)}"}), 500

if __name__ == '__main__':
    print("===================================================")
    print("  ECO-CALC SERVER IS RUNNING ON PORT 5000")
    print("===================================================")
    app.run(debug=True, port=5000)