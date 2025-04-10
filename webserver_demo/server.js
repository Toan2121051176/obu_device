const mqtt = require("mqtt");
const express = require("express");
const cors = require("cors");
const axios = require("axios");
const fs = require("fs");

const roadCosts = require("./roadCost.json");

const app = express();
const PORT = 3000;

const mqttBrokerUrl = "wss://broker.emqx.io:8084/mqtt";
const mqttClient = mqtt.connect(mqttBrokerUrl);

let lastMessage = "";
let cost = 0;
let trafficStatuses = [];
let errorFlag = false;
let mqttData = [];

const DATA_FILE = "data.json";
const TIMEOUT_DURATION = 3000;
let timeout = null;
let timecheck = true;

app.use(cors());
app.use(express.json());

mqttClient.on("connect", () => {
    console.log("Connected to MQTT broker");
    mqttClient.subscribe("testelcol", () => {
        console.log("📌 Dữ liệu trước khi ghi vào file:", JSON.stringify(mqttData, null, 2));
        saveDataToFile(mqttData);
        console.log("Subscribed to testelcol topic");
    });
});

// Reset timeout
function resetTimeout() {
    if (timeout) clearTimeout(timeout);
    timeout = setTimeout(() => { timecheck = false; }, TIMEOUT_DURATION);
}

// Hàm lấy trạng thái giao thông từ TomTom API
const TOMTOM_API_KEY = "an3iZV1eIoJS0UqOK74G6MHIHaGv6ETr";

async function getTrafficStatus(lat, lng) {
    const url = `https://api.tomtom.com/traffic/services/4/flowSegmentData/absolute/10/json?point=${lat},${lng}&key=${TOMTOM_API_KEY}`;
    try {
        const response = await axios.get(url, { timeout: 10000 });
        const { currentSpeed, freeFlowSpeed } = response.data.flowSegmentData;
        const congestion = currentSpeed / freeFlowSpeed;

        return congestion < 0.5 ? "Heavy Traffic" :
               congestion < 0.65 ? "Moderate Traffic" : "Light Traffic";
    } catch (error) {
        console.error("Error fetching traffic status:", error.message);
        return "Traffic data unavailable";
    }
}

// Lưu dữ liệu vào file JSON
function saveDataToFile(data) {
    fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2), "utf8");
}

// Xử lý tin nhắn MQTT
mqttClient.on("message", async (topic, message) => {
    console.log(`Received: ${message.toString()} on topic ${topic}`);
    lastMessage = message.toString();
    timecheck = true;
    resetTimeout();

    const [lat, lng] = lastMessage.split(",");
    if (!lat || !lng || isNaN(parseFloat(lat)) || isNaN(parseFloat(lng))) {
        console.error("Invalid message format.");
        errorFlag = true;
        return;
    } else {
        errorFlag = false;
    }

    // Bạn có thể tính phí theo toạ độ nếu muốn, ở đây bỏ tính theo tên đường
    // const road = removeDiacritics(roadName);
    // if (road !== lastRoadName) { ... } => bỏ luôn phần này nếu không cần

    // Lưu dữ liệu mới
    const newData = {
        timestamp: new Date().toISOString(),
        lat: parseFloat(lat),
        lng: parseFloat(lng),
        // Nếu vẫn cần trường road thì gán mặc định:
        road: null,
        cost: cost
    };
    mqttData.push(newData);
    if (mqttData.length > 100) mqttData.shift();
    saveDataToFile(mqttData);

    // Kiểm tra giao thông khi đủ 5 lần cập nhật
    if (mqttData.length % 5 === 0) {
        const trafficStatus = await getTrafficStatus(lat, lng);
        if (trafficStatus === "Heavy Traffic") {
            trafficStatuses.push(`(${lat},${lng})`);
        }
        console.log(`Traffic: ${trafficStatus}`);
    }
});

// API kiểm tra thông tin gần nhất
app.get("/api/last-message", (req, res) => {
    res.json({ 
        message: lastMessage,
        error: errorFlag,
        cost: cost,
        timecheck: timecheck,
        trafficStatuses: trafficStatuses
     });
});

// API xoá dữ liệu
app.delete('/api/clear-data', (req, res) => {
    lastMessage = ""; 
    cost = 0;
    trafficStatuses = [];
    console.log('Data cleared');
    res.status(200).send('Data cleared');
});

// Khởi động server
app.listen(PORT, '0.0.0.0', () => {
    console.log(`Server running on http://0.0.0.0:${PORT}`);
});
