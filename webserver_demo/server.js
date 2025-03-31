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

let lastRoadName = "";
let lastMessage = "";
let cost = 0;
let trafficStatuses = [];
let errorFlag = false;
let mqttData = [];

const DATA_FILE = "data.json"; // File lưu dữ liệu
const TIMEOUT_DURATION = 3000; // 3 giây
let timeout = null;
let timecheck = true;

app.use(cors());
app.use(express.json());

mqttClient.on("connect", () => {
    console.log("Connected to MQTT broker");
    mqttClient.subscribe("elcomOBU/29V180060", () => {
        console.log("📌 Dữ liệu trước khi ghi vào file:", JSON.stringify(mqttData, null, 2));
saveDataToFile(mqttData);

        console.log("Subscribed to testelcol topic");
    });
});

// Hàm loại bỏ dấu tiếng Việt
function removeDiacritics(str) {
    return str.normalize("NFD")
        .replace(/[\u0300-\u036f]/g, "")
        .replace(/đ/g, "d").replace(/Đ/g, "D")
        .toLowerCase();
}

// Reset timeout
function resetTimeout() {
    if (timeout) clearTimeout(timeout);
    timeout = setTimeout(() => { timecheck = false; }, TIMEOUT_DURATION);
}

// Hàm lấy tên đường từ OpenStreetMap
async function getStreetName(lat, lon) {
    const url = `https://nominatim.openstreetmap.org/reverse?lat=${lat}&lon=${lon}&format=json`;

    try {
        const response = await axios.get(url, {
            headers: { 'User-Agent': 'YourAppName/1.0 (email@example.com)' },
            timeout: 5000 // Thêm timeout
        });
        return response.data.address.road || "Unnamed Road";
    } catch (error) {
        console.error("Error fetching street name:", error.message);
        return "Error fetching road name";
    }
}

// Lấy trạng thái giao thông từ TomTom API
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

    const roadName = await getStreetName(parseFloat(lat), parseFloat(lng));
    const road = removeDiacritics(roadName);

    // Cập nhật phí đường
    if (road !== lastRoadName) {
        lastRoadName = road;
        cost += roadCosts[road] || 0;
        mqttClient.publish("testelcoml", `cost: ${cost}`);
        console.log(`Updated cost: ${cost}`);
    }

    // Lưu dữ liệu mới
    const newData = {
        timestamp: new Date().toISOString(),
        lat: parseFloat(lat),
        lng: parseFloat(lng),
        road: roadName,
        cost: cost
    };
    mqttData.push(newData);
    if (mqttData.length > 100) mqttData.shift();
    saveDataToFile(mqttData);

    // Kiểm tra giao thông khi đủ 5 lần cập nhật
    if (mqttData.length % 5 === 0) {
        const trafficStatus = await getTrafficStatus(lat, lng);
        if (trafficStatus === "Heavy Traffic") {
            trafficStatuses.push(roadName);
        }
        console.log(`Traffic: ${trafficStatus}`);
    }
});

// API kiểm tra thông tin gần nhất
app.get("/api/last-message", (req, res) => {
    res.json({ 
        message: lastMessage, 
        road: lastRoadName,
        error: errorFlag,
        cost: cost,
        timecheck: timecheck,
        trafficStatuses: trafficStatuses
     });
});

// API xoá dữ liệu
app.delete('/api/clear-data', (req, res) => {
    lastMessage = ""; 
    lastRoadName = ""; 
    cost = 0;
    trafficStatuses = [];
    console.log('Data cleared');
    res.status(200).send('Data cleared');
});

// Khởi động server
app.listen(PORT, '0.0.0.0', () => {
    console.log(`Server running on http://0.0.0.0:${PORT}`);
});
