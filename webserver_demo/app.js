let map;
let marker;
let coordinatesArray = [];
let totalDistance = 0;
let lastRoadName = "";
let cost = 0;
let errorFlag = false;
let errorLogs = [];
let trafficStatuses = [];
let startPoint = null;
let endPoint = null;
const markers = [];

function initMap() {
    map = L.map('map').setView([10.7769, 106.7009], 13);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '&copy; OpenStreetMap contributors'
    }).addTo(map);
}

// 🚀 **Tìm kiếm thiết bị và hiển thị lên bản đồ**
async function searchDevice() {
    const deviceName = document.getElementById("deviceName").value.trim();
    
    if (!deviceName) {
        alert("Vui lòng nhập tên thiết bị!");
        return;
    }

    try {
        const response = await fetch("http://localhost:3000/api/last-message");
        const data = await response.json();

        if (data && data.message) {
            document.getElementById("coords").textContent = data.message;
            document.getElementById("road").textContent = data.road || "Không xác định";
            document.getElementById("cost").textContent = data.cost || 0;
            document.getElementById("traffic").textContent = data.trafficStatuses.length > 0 ? data.trafficStatuses.join(", ") : "Không có thông tin";

            const [lat, lng] = data.message.split(",");
            const latitude = parseFloat(lat);
            const longitude = parseFloat(lng);

            if (!isNaN(latitude) && !isNaN(longitude)) {
                updateMarker(latitude, longitude);
            } else {
                alert("Lỗi: Tọa độ không hợp lệ!");
            }
        } else {
            alert("Không tìm thấy thông tin thiết bị!");
        }
    } catch (error) {
        console.error("Lỗi khi lấy dữ liệu:", error);
        alert("Không thể lấy dữ liệu, vui lòng thử lại!");
    }
}

// 🚀 **Cập nhật vị trí marker trên bản đồ**
function updateMarker(lat, lng) {
    if (marker) {
        map.removeLayer(marker);
    }
    marker = L.marker([lat, lng]).addTo(map)
        .bindPopup("Vị trí thiết bị")
        .openPopup();
    map.setView([lat, lng], 15);
}

// 🚀 **Lấy dữ liệu từ backend mỗi giây**
async function fetchDataFromBackend() {
    try {
        const response = await fetch("http://localhost:3000/api/last-message");
        const data = await response.json();
        
        errorFlag = data.error;
        cost = data.cost;
        trafficStatuses = data.trafficStatuses;

        document.getElementById("costDisplay").textContent = `${cost} VND`;
        document.getElementById("distanceDisplay").textContent = `${totalDistance.toFixed(2)} km`;

        if (data.message) {
            const [lat, lng] = data.message.split(",");
            const latitude = parseFloat(lat);
            const longitude = parseFloat(lng);

            if (!isNaN(latitude) && !isNaN(longitude) && (latitude !== 0 || longitude !== 0)) {
                updateMapWithCoordinates(latitude, longitude);
                updateRoadName(latitude, longitude, data.road);
            }
        }
    } catch (error) {
        console.error("Error fetching data from backend:", error);
    }
}

// 🚀 **Cập nhật tên đường và hiển thị popup**
function updateRoadName(lat, lng, roadName) {
    if (roadName !== lastRoadName) {
        removeAllPopups();
        addTextToMap(lat, lng, roadName);
        console.log(`Tên đường: ${roadName} tại tọa độ [${lat}, ${lng}]`);
        lastRoadName = roadName;
    }
}

// 🚀 **Hiển thị tên đường lên bản đồ**
function addTextToMap(lat, lng, text) {
    L.popup({ closeButton: false, autoClose: false, className: 'text-popup' })
        .setLatLng([lat, lng])
        .setContent(text)
        .openOn(map);
}

// 🚀 **Xóa tất cả popup**
function removeAllPopups() {
    map.eachLayer(layer => {
        if (layer instanceof L.Popup) {
            map.removeLayer(layer);
        }
    });
}

// 🚀 **Cập nhật quãng đường và vẽ đường**
function updateMapWithCoordinates(lat, lng) {
    coordinatesArray.push([lat, lng]);
 
    if (!startPoint) {
        startPoint = { lat, lng };
        addMarker(startPoint, 'Start Point');
    } else {
        endPoint = { lat, lng };
        findRoute(startPoint, endPoint);
        startPoint = { lat, lng };
    }

    if (coordinatesArray.length > 1) {
        const lastPoint = coordinatesArray[coordinatesArray.length - 2];
        const distance = calculateDistance(lastPoint[0], lastPoint[1], lat, lng);
        totalDistance += distance;
    }
}

// 🚀 **Thêm marker vào bản đồ**
function addMarker(latlng, label) {
    L.marker(latlng).addTo(map).bindPopup(label).openPopup();
}

// 🚀 **Tính khoảng cách giữa hai điểm**
function calculateDistance(lat1, lng1, lat2, lng2) {
    const R = 6371;
    const toRadians = degree => degree * (Math.PI / 180);
    const Δφ = toRadians(lat2 - lat1);
    const Δλ = toRadians(lng2 - lng1);
    const φ1 = toRadians(lat1);
    const φ2 = toRadians(lat2);

    const a = Math.sin(Δφ / 2) ** 2 + Math.cos(φ1) * Math.cos(φ2) * Math.sin(Δλ / 2) ** 2;
    return R * (2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a)));
}

// 🚀 **Tìm và vẽ đường trên bản đồ**
async function findRoute(start, end) {
    const osrmUrl = `http://router.project-osrm.org/match/v1/driving/${start.lng},${start.lat};${end.lng},${end.lat}?geometries=geojson`;

    try {
        const response = await fetch(osrmUrl);
        const data = await response.json();

        if (data.code === 'Ok') {
            const coords = data.matchings[0].geometry.coordinates.map(([lng, lat]) => [lat, lng]);
            L.polyline(coords, { color: 'blue', weight: 8 }).addTo(map);
        }
    } catch (error) {
        console.error('Error fetching route:', error);
    }
}

// 🚀 **Xóa dữ liệu trên bản đồ**
document.getElementById('unsubscribe').addEventListener('click', async () => {
    try {
        await fetch('http://localhost:3000/api/clear-data', { method: 'DELETE' });

        map.eachLayer(layer => {
            if (layer instanceof L.Marker || layer instanceof L.Polyline) {
                map.removeLayer(layer);
            }
        });

        coordinatesArray = [];
        totalDistance = 0;
        cost = 0;
        startPoint = null;
        endPoint = null;
        document.getElementById("costDisplay").textContent = "0 VND";
        document.getElementById("distanceDisplay").textContent = "0 km";
    } catch (error) {
        console.error("Error clearing data:", error);
    }
});

// 🚀 **Tự động cập nhật dữ liệu mỗi giây**
setInterval(fetchDataFromBackend, 1000);

// 🚀 **Khởi tạo bản đồ khi tải trang**
document.addEventListener("DOMContentLoaded", initMap);
