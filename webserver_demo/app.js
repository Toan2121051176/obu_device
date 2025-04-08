// Khởi tạo bản đồ Leaflet
var map = L.map('map').setView([21.030034, 105.782190], 13);

L.tileLayer('https://mt1.google.com/vt/lyrs=m&x={x}&y={y}&z={z}&scale=20', {
  attribution: '&copy; <a href="https://www.google.com/maps">Google Maps</a>'
}).addTo(map);

L.tileLayer('https://mt1.google.com/vt/lyrs=m@221097413,traffic&x={x}&y={y}&z={z}&scale=20', {
  attribution: '&copy; <a href="https://www.google.com/maps">Google Maps</a>'
}).addTo(map);

let coordinatesArray = [];
let totalDistance = 0;
let popupsArray = [];
let lastRoadName = "";
let errorFlag = false;
let cost = 0;
let errorShown0 = true; // GPS error
let errorShown1 = true; // No signal error
let errorLogs = [];
let trafficStatuses = [];
let tollRoads = {}; // Từ file roads.json
const paidRoads = new Set(); // Tránh tính phí trùng

var markersGroup = L.layerGroup().addTo(map);
var linesGroup = L.layerGroup().addTo(map);

// Load danh sách đường thu phí từ file roads.json
fetch("roads.json")
  .then(response => response.json())
  .then(data => {
    tollRoads = data;
    console.log("Toll roads loaded", tollRoads);
  })
  .catch(error => {
    console.error("Failed to load toll road data:", error);
  });

// Lấy dữ liệu từ backend
async function fetchDataFromBackend() {
  try {
    const response = await fetch("http://192.168.8.46:3000/api/last-message");
    const data = await response.json();

    errorFlag = data.error;
    cost = data.cost;
    trafficStatuses = data.trafficStatuses;
    const roadName = data.road?.toLowerCase().trim();

    if (roadName) {
      checkAndChargeTollRoad(roadName);
    }

    if (errorFlag) {
      const currentTime = new Date().toLocaleString();
      if (!errorShown0) {
        showError(`No GPS Signal!!! (${currentTime})`);
        errorShown0 = true;
        errorLogs.push(`No GPS Signal From Device at (${currentTime})`);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
      }
    } else {
      document.getElementById('errorBox').innerHTML = '';
      errorShown0 = false;
    }

    if (!data.timecheck) {
      const currentTime = new Date().toLocaleString();
      if (!errorShown1) {
        showError(`No Signal From Device at (${currentTime})`);
        errorShown1 = true;
        errorLogs.push(`No Signal From Device at (${currentTime})`);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
      }
    } else {
      document.getElementById('errorBox').innerHTML = '';
      errorShown1 = false;
    }

    document.getElementById("costDisplay").textContent = `${cost} VND`;
    document.getElementById("distanceDisplay").textContent = `${totalDistance.toFixed(2)} km`;

    if (data.message) {
      const [lat, lng] = data.message.split(',').map(coord => parseFloat(coord.trim()));
      document.getElementById("coordinatesDisplay").innerHTML = `${lat.toFixed(4)}<br>${lng.toFixed(4)}`;

      if (!isNaN(lat) && !isNaN(lng) && lat !== 0 && lng !== 0) {
        updateMapWithCoordinates(lat, lng);
      }
    }
  } catch (error) {
    console.error("Error fetching data from backend:", error);
    if (!errorShown0) {
      showError("Failed to fetch data from the backend. Please try again later.");
      errorShown0 = true;
    }
  }
}

function checkAndChargeTollRoad(roadName) {
  if (tollRoads[roadName] && !paidRoads.has(roadName)) {
    const fee = tollRoads[roadName];
    cost += fee;
    paidRoads.add(roadName);
    document.getElementById("costDisplay").textContent = `${cost} VND`;
    showError(`🚧 Passed Toll Road: ${roadName.toUpperCase()} (+${fee} VND)`);
    console.log(`Charged ${fee} VND for ${roadName}`);
  }
}

function updateMapWithCoordinates(lat, lng) {
  coordinatesArray.push([lat, lng]);

  if (coordinatesArray.length > 1) {
    const [prevLat, prevLng] = coordinatesArray[coordinatesArray.length - 2];
    const distance = calculateDistance(prevLat, prevLng, lat, lng);
    totalDistance += distance;
    drawLine({ lat: prevLat, lng: prevLng }, { lat, lng });

    if (distance > 0.2) {
      const currentTime = new Date().toLocaleString();
      showError(`Abnormal in GPS signal at (${currentTime})`);
      errorLogs.push(`Abnormal in GPS signal at (${currentTime})`);
      localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
    }
  } else {
    addMarker({ lat, lng }, 'Start Point');
  }
}

function drawLine(start, end) {
  L.polyline([[start.lat, start.lng], [end.lat, end.lng]], {
    color: 'blue', weight: 4
  }).addTo(linesGroup);
}

function addMarker(latlng, label) {
  L.marker(latlng).addTo(markersGroup).bindPopup(label).openPopup();
}

function calculateDistance(lat1, lng1, lat2, lng2) {
  const R = 6371;
  const toRad = deg => deg * (Math.PI / 180);
  const dLat = toRad(lat2 - lat1);
  const dLng = toRad(lng2 - lng1);
  const a = Math.sin(dLat / 2) ** 2 + Math.cos(toRad(lat1)) * Math.cos(toRad(lat2)) * Math.sin(dLng / 2) ** 2;
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  return R * c;
}

function showError(message) {
  const box = document.getElementById("errorBox");
  const div = document.createElement("div");
  div.classList.add("error-message");
  div.textContent = message;
  box.appendChild(div);
}

function openErrorPage() {
  window.open("error.html", "_blank");
}

document.getElementById("showErrors").addEventListener("click", openErrorPage);

document.getElementById('unsubscribe').addEventListener('click', async () => {
  try {
    await fetch('http://192.168.8.46:3000/api/clear-data', { method: 'DELETE' });
    map.eachLayer(layer => {
      if (layer instanceof L.Marker || layer instanceof L.Polyline) {
        map.removeLayer(layer);
      }
    });
    coordinatesArray = [];
    totalDistance = 0;
    cost = 0;
    paidRoads.clear();
    document.getElementById('errorBox').innerHTML = '';
    errorShown0 = true;
    errorShown1 = true;
  } catch (error) {
    console.error("Error clearing data:", error);
  }
});

setInterval(fetchDataFromBackend, 1000);

// Ngày mặc định là hôm nay
document.getElementById("datePicker").value = new Date().toISOString().split("T")[0];

document.getElementById("datePicker").addEventListener("change", function () {
  console.log("Ngày đã chọn:", this.value);
});

document.getElementById("searchButton").addEventListener("click", function () {
  const name = document.getElementById("deviceName").value;
  alert("Tìm kiếm thiết bị: " + name);
});
