var map = L.map('map').setView([21.030034, 105.782190], 13);

// Lớp bản đồ mặc định với high-DPI (sắc nét)
L.tileLayer('https://mt1.google.com/vt/lyrs=m&x={x}&y={y}&z={z}&scale=20', {
  attribution: '&copy; <a href="https://www.google.com/maps">Google Maps</a>'
}).addTo(map);

// Lớp giao thông cũng sắc nét
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
let errorShown1 = true; // no signal error
let timecheck = true;
let errorLogs = [];  
let trafficStatuses = [];

var markersGroup = L.layerGroup();
map.addLayer(markersGroup);
var linesGroup = L.layerGroup();
map.addLayer(linesGroup);

// get data from backend
async function fetchDataFromBackend() {
  try {
    // const response = await fetch("http://192.168.1.2:3000/api/last-message"); // API từ backend
    const response = await fetch("http://192.168.8.46:3000/api/last-message");
    const data = await response.json();
    
    errorFlag = data.error;
    cost = data.cost;
    timecheck = data.timecheck;
    trafficStatuses = data.trafficStatuses;
    
    if (trafficStatuses.length > 0) {
      console.log(trafficStatuses);
    }
    
    // GPS error
    if (errorFlag) {
      const currentTime = new Date().toLocaleString();
      if (!errorShown0) {
        showError(`No GPS Signal!!! (${currentTime})`);
        errorShown0 = true; // Đặt cờ là đã hiển thị lỗi
        const errorMessage = `No GPS Signal From Device at (${currentTime})`;
        errorLogs.push(errorMessage);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
      }
    } else {
      const errorContainer = document.getElementById('errorBox'); // Phần tử chứa lỗi
      if (errorContainer) {
        errorContainer.innerHTML = ''; // Xóa toàn bộ nội dung lỗi
      }
      errorShown0 = false; 
    }
    
    // no signal error
    if (!timecheck) {
      const currentTime = new Date().toLocaleString();
      if (!errorShown1) {
        showError(`No Signal From Device at (${currentTime})`);
        errorShown1 = true; // Đặt cờ là đã hiển thị lỗi
        const errorMessage = `No Signal From Device at (${currentTime}) seconds`;
        errorLogs.push(errorMessage);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
      }
    } else {
      const errorContainer = document.getElementById('errorBox'); // Phần tử chứa lỗi
      if (errorContainer) {
        errorContainer.innerHTML = ''; // Xóa toàn bộ nội dung lỗi
      }
      errorShown1 = false; 
    }
    
    document.getElementById("costDisplay").textContent = `${cost} VND`;
    document.getElementById("distanceDisplay").textContent = `${totalDistance.toFixed(2)} km`;
    
    if (data.message) {
      const coords = data.message.split(',');
      const lat = parseFloat(coords[0].trim());
      const lng = parseFloat(coords[1].trim());
      document.getElementById("coordinatesDisplay").innerHTML = `${lat.toFixed(4)}<br>${lng.toFixed(4)}`;
      
      if (!isNaN(lat) && !isNaN(lng)) {
        if (lat === 0 || lng === 0) {
          console.log("xe khoi dong");
        } else {
          errorShown0 = false;
          updateMapWithCoordinates(lat, lng);
          // updateRoadName(lat, lng, data.road); 
        }
      }
    }
  } catch (error) {
    console.error("Error fetching data from backend:", error);
    if (!errorShown0) {
      showError("Failed to fetch data from the backend. Please try again later.");
      errorShown0 = true; // Đặt cờ là đã hiển thị lỗi
    }
  }
}

// Cập nhật tên đường và hiển thị popup
function updateRoadName(lat, lng, roadName) {
  if (roadName !== lastRoadName) {
    removeAllPopups();
    addTextToMap(lat, lng, roadName);
    console.log(`Tên đường: ${roadName} tại tọa độ [${lat}, ${lng}]`);
    lastRoadName = roadName;
  }
}

// Thêm popup vào bản đồ
function addTextToMap(lat, lng, text) {
  const popup = L.popup({
    closeButton: false, 
    autoClose: false,   
    className: 'text-popup'
  })
    .setLatLng([lat, lng])
    .setContent(text)
    .openOn(map);
  popupsArray.push(popup);
}

// Xóa tất cả popup
function removeAllPopups() {
  popupsArray.forEach(popup => {
    map.removeLayer(popup);
  });
  popupsArray = [];
}

// Tính khoảng cách giữa hai điểm
function calculateDistance(lat1, lng1, lat2, lng2) {
  const R = 6371; // Bán kính trái đất
  const toRadians = degree => degree * (Math.PI / 180);
  const φ1 = toRadians(lat1);
  const φ2 = toRadians(lat2);
  const Δφ = toRadians(lat2 - lat1);
  const Δλ = toRadians(lng2 - lng1);
  const a = Math.sin(Δφ / 2) ** 2 + 
            Math.cos(φ1) * Math.cos(φ2) * 
            Math.sin(Δλ / 2) ** 2;
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  return R * c; // Khoảng cách tính bằng km
}

let countPoint = 0;
let startPoint = null;
let endPoint = null;
const markers = [];
// Hàm cập nhật bản đồ với tọa độ mới từ thiết bị
function updateMapWithCoordinates(lat, lng) {
  coordinatesArray.push([lat, lng]);
  if (countPoint === 0) {
    startPoint = { lat: lat, lng: lng };
    addMarker(startPoint, 'Start Point');
    countPoint++;
  } else {
    endPoint = { lat: lat, lng: lng };
    //findRoute(startPoint, endPoint);
    drawLine(startPoint, endPoint);
    startPoint = { lat: lat, lng: lng };
  }
  if (coordinatesArray.length > 1) {
    const lastPoint = coordinatesArray[coordinatesArray.length - 2];
    const distance = calculateDistance(lastPoint[0], lastPoint[1], lat, lng);
    if (distance > 0.2) {
      const currentTime = new Date().toLocaleString();
      showError(`Abnormal in GPS signal at (${currentTime})`);
      errorShown0 = true; // Đặt cờ là đã hiển thị lỗi
      const errorMessage = `Abnormal in GPS signal at (${currentTime})`;
      errorLogs.push(errorMessage);
      localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
      console.log(errorMessage);
    }
    totalDistance += distance;
  }
}

function addMarker(latlng, label) {
  const m = L.marker(latlng).addTo(map).bindPopup(label).openPopup();
  markers.push(m);
}

// Nối trực tiếp 2 điểm
function drawLine(start, end) {
  const coords = [
    [start.lat, start.lng],
    [end.lat, end.lng]
  ];
  L.polyline(coords, { color: 'blue', weight: 4 }).addTo(map);
}

// Hiển thị thông báo lỗi
function showError(message) {
  let errorBox = document.getElementById("errorBox");
  let errorMessage = document.createElement("div");
  errorMessage.classList.add("error-message");
  errorMessage.textContent = message;
  errorBox.appendChild(errorMessage);
}

function openErrorPage() {
  window.open("error.html", "_blank");
}

// Gắn sự kiện cho nút "Show Errors"
document.getElementById("showErrors").addEventListener("click", openErrorPage);

document.getElementById('unsubscribe').addEventListener('click', async () => {
  try {
    await fetch('http://192.168.8.46:3000/api/clear-data', {
      method: 'DELETE',
    });
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
    const errorContainer = document.getElementById('errorBox');
    if (errorContainer) {
      errorContainer.innerHTML = '';
    }
    errorShown0 = true;
    errorShown1 = true;
  } catch (error) {
    console.error("Error clearing data on the server:", error);
  }
});

setInterval(fetchDataFromBackend, 1000);



// Đặt ngày mặc định là hôm nay
const dateInput = document.getElementById("datePicker");
const today = new Date().toISOString().split("T")[0];
dateInput.value = today;

// Khi chọn ngày khác, cập nhật thông tin
dateInput.addEventListener("change", function () {
    console.log("Ngày đã chọn:", this.value);
});

// Hàm tìm kiếm thiết bị (chưa triển khai)
function searchDevice() {
    const deviceName = document.getElementById("deviceName").value;
    alert("Tìm kiếm thiết bị: " + deviceName);
}
