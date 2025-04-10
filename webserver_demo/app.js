document.addEventListener("DOMContentLoaded", () => {
  // Khởi tạo bản đồ
  const map = L.map('map').setView([21.030034, 105.782190], 13);

  L.tileLayer('https://mt1.google.com/vt/lyrs=m&x={x}&y={y}&z={z}&scale=20', {
    attribution: '&copy; <a href="https://www.google.com/maps">Google Maps</a>'
  }).addTo(map);

  L.tileLayer('https://mt1.google.com/vt/lyrs=m@221097413,traffic&x={x}&y={y}&z={z}&scale=20', {
    attribution: '&copy; <a href="https://www.google.com/maps">Google Maps</a>'
  }).addTo(map);

  // Biến toàn cục
  let coordinatesArray = []; 
  let totalDistance = 0;
  let errorLogs = [];
  let cost = 0;
  let tollRoads = {};
  const paidRoads = new Set();

  let errorShownGPS = false;
  let errorShownSignal = false;

  const markersGroup = L.layerGroup().addTo(map);
  const linesGroup = L.layerGroup().addTo(map);

  // Hàm hiển thị lỗi
  function showError(message) {
    const box = document.getElementById("errorBox");
    if (!box) return;

    const div = document.createElement("div");
    div.className = "error-message";
    div.textContent = message;
    box.appendChild(div);
  }

  // Tính khoảng cách giữa 2 điểm (Haversine)
  function calculateDistance(lat1, lng1, lat2, lng2) {
    const R = 6371;
    const toRad = deg => deg * Math.PI / 180;
    const dLat = toRad(lat2 - lat1);
    const dLng = toRad(lng2 - lng1);
    const a = Math.sin(dLat / 2) ** 2 +
              Math.cos(toRad(lat1)) * Math.cos(toRad(lat2)) *
              Math.sin(dLng / 2) ** 2;
    return R * 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  }

  // Thêm marker hoặc vẽ đường
  function updateMapWithCoordinates(lat, lng) {
    coordinatesArray.push([lat, lng]);

    if (coordinatesArray.length > 1) {
      const [prevLat, prevLng] = coordinatesArray[coordinatesArray.length - 2];
      const distance = calculateDistance(prevLat, prevLng, lat, lng);
      totalDistance += distance;

      L.polyline([[prevLat, prevLng], [lat, lng]], {
        color: 'blue', weight: 4
      }).addTo(linesGroup);

      if (distance > 0.2) {
        const time = new Date().toLocaleString();
        showError(`⚠️ GPS bất thường tại ${time}`);
        errorLogs.push(`GPS lỗi tại ${time}`);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
      }
    } else {
      L.marker([lat, lng]).addTo(markersGroup).bindPopup("Thiết Bị").openPopup();
    }

    const distEl = document.getElementById("distanceDisplay");
    if (distEl) distEl.textContent = `${totalDistance.toFixed(2)} km`;
  }

  // Kiểm tra và tính phí đường thu phí
  function checkAndChargeTollRoad(roadName) {
    if (tollRoads[roadName] && !paidRoads.has(roadName)) {
      const fee = tollRoads[roadName];
      cost += fee;
      paidRoads.add(roadName);
      const costEl = document.getElementById("costDisplay");
      if (costEl) costEl.textContent = `${cost} VND`;
      showError(`🚧 Qua đường thu phí: ${roadName.toUpperCase()} (+${fee} VND)`);
    }
  }

  // Lấy dữ liệu từ server
  async function fetchDataFromBackend() {
    try {
      const res = await fetch("http://localhost:3000/api/last-message");
      const data = await res.json();

      const errorBox = document.getElementById("errorBox");

      // Lỗi GPS
      if (data.error && !errorShownGPS) {
        const time = new Date().toLocaleString();
        showError(`📡 Không có tín hiệu GPS (${time})`);
        errorLogs.push(`GPS lỗi tại ${time}`);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
        errorShownGPS = true;
      } else if (!data.error) {
        if (errorBox) errorBox.innerHTML = '';
        errorShownGPS = false;
      }

      // Lỗi không có tín hiệu
      if (!data.timecheck && !errorShownSignal) {
        const time = new Date().toLocaleString();
        showError(`❌ Không có tín hiệu thiết bị (${time})`);
        errorLogs.push(`Mất tín hiệu tại ${time}`);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
        errorShownSignal = true;
      } else if (data.timecheck) {
        if (errorBox) errorBox.innerHTML = '';
        errorShownSignal = false;
      }

      // Hiển thị vị trí
      if (data.message) {
        const [lat, lng] = data.message.split(",").map(Number);
        const coordsEl = document.getElementById("coordinatesDisplay");
        if (coordsEl) coordsEl.innerHTML = `${lat.toFixed(4)}<br>${lng.toFixed(4)}`;
        if (!isNaN(lat) && !isNaN(lng) && lat !== 0 && lng !== 0) {
          updateMapWithCoordinates(lat, lng);
        }
      }

      // Kiểm tra đường thu phí
      const roadName = data.road?.toLowerCase().trim();
      if (roadName) checkAndChargeTollRoad(roadName);

    } catch (err) {
      console.error("Lỗi lấy dữ liệu:", err);
      if (!errorShownGPS) {
        showError("⚠️ Không thể kết nối đến server.");
        errorShownGPS = true;
      }
    }
  }

  // Cập nhật mỗi giây
  setInterval(fetchDataFromBackend, 1000);

  // Dừng theo dõi
  document.getElementById("unsubscribe")?.addEventListener("click", async () => {
    try {
      await fetch('http://localhost:3000/api/last-message', { method: 'DELETE' });

      map.eachLayer(layer => {
        if (layer instanceof L.Marker || layer instanceof L.Polyline) {
          map.removeLayer(layer);
        }
      });

      coordinatesArray = [];
      totalDistance = 0;
      cost = 0;
      paidRoads.clear();
      const box = document.getElementById("errorBox");
      if (box) box.innerHTML = '';
      errorShownGPS = false;
      errorShownSignal = false;
    } catch (error) {
      console.error("Error clearing data:", error);
    }
  });

  // Xem lỗi
  document.getElementById("showErrors")?.addEventListener("click", () => {
    window.open("error.html", "_blank");
  });

  // Tìm thiết bị
  document.getElementById("searchButton")?.addEventListener("click", () => {
    const name = document.getElementById("deviceName")?.value;
    alert("Tìm kiếm thiết bị: " + name);
  });

  // Mặc định chọn ngày hôm nay
  const today = new Date().toISOString().split("T")[0];
  const dateInput = document.getElementById("datePicker");
  if (dateInput) {
    dateInput.value = today;
    dateInput.addEventListener("change", function () {
      console.log("Ngày đã chọn:", this.value);
    });
  }

});
