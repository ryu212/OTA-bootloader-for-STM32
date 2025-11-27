from flask import Flask, jsonify, render_template, request, send_from_directory
import os
import json
import binascii
import crcmod


app = Flask(__name__)

FIRMWARE_FOLDER = "./firmware"
VERSION_FILE = os.path.join(FIRMWARE_FOLDER, "version.json")

@app.route('/')
def index():
    return render_template("webserver.html")

@app.route('/api/upload_bin', methods=['POST'])
def upload_bin():
    # 1. Kiểm tra xem có file gửi lên không
    if 'firmware' not in request.files:
        return jsonify({"error": "No file part"}), 400

    file = request.files['firmware']

    # 2. Kiểm tra file có được chọn không
    if file.filename == '':
        return jsonify({"error": "No selected file"}), 400

    # 3. Lấy extension từ filename
    _, file_extension = os.path.splitext(file.filename)
    if file_extension.lower() != '.bin':
        return jsonify({"error": "File is not .bin"}), 401

    # 4. Tạo folder firmware nếu chưa tồn tại
    os.makedirs("./firmware", exist_ok=True)
    file.save(os.path.join("./firmware", "latest.bin"))
    # 5. Tính CRC32 của file
    crc32_func = crcmod.mkCrcFun(0x104C11DB7, rev=True, initCrc=0xFFFFFFFF, xorOut=0xFFFFFFFF)

    with open("./firmware/latest.bin", "rb") as f:
         data = f.read()
         checksum = crc32_func(data)
    print(f"checksum = {checksum}")
    version_data = {"version": file.filename, "checksum": checksum}
    with open(VERSION_FILE, "w") as f:
        json.dump(version_data, f, indent=4)
    return jsonify({"message": "Firmware uploaded successfully!"}), 200

@app.route('/firmware/<filename>', methods=['GET'])
def get_firmware(filename):
    jsonify()
    return send_from_directory("./firmware", filename, as_attachment=True)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
