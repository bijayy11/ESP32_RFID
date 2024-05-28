from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/card_detected', methods=['POST'])
def card_detected():
    data = request.get_json()
    uid = data.get('uid')
    print(f"Card detected with UID: {uid}")
    return jsonify({"status": "success", "message": f"UID {uid} received"}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
