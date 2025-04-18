document.addEventListener("DOMContentLoaded", () => {
  const map = L.map('map').setView([21.030034, 105.782190], 13);

  L.tileLayer('https://mt1.google.com/vt/lyrs=m&x={x}&y={y}&z={z}&scale=20', {
    attribution: '&copy; <a href="https://www.google.com/maps">Google Maps</a>'
  }).addTo(map);

  let coordinatesArray = [];
  let totalDistance = 0;
  let cost = 0;
  let tollRoads = {};
  const paidRoads = new Set();

  let errorLogs = [];
  let errorShownGPS = false;
  let errorShownSignal = false;

  let carMarker = null;
  let startMarker = null;

  const linesGroup = L.layerGroup().addTo(map);

  function showError(message) {
    const box = document.getElementById("errorBox");
    if (!box) return;
    const div = document.createElement("div");
    div.className = "error-message";
    div.textContent = message;
    box.appendChild(div);
  }

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

  function updateMapWithCoordinates(lat, lng) {
    coordinatesArray.push([lat, lng]);
  
    if (coordinatesArray.length === 1) {
      // Tạo marker điểm bắt đầu
      startMarker = L.marker([lat, lng], {
        icon: L.divIcon({
          html: '<div style="position: relative;"><span style="position: absolute; top: -20px; left: -10px; background: white; padding: 2px 4px; border-radius: 4px; font-size: 10px;">Bắt đầu</span>📍</div>',
          className: '',
          iconSize: [24, 24],
          iconAnchor: [12, 12]
        })
      }).addTo(map).bindPopup("Điểm bắt đầu").openPopup();
  
      // Tạo marker xe lần đầu tiên
      carMarker = L.marker([lat, lng], {
        icon: L.divIcon({
          html: `
            <div style="position: relative; text-align: center;">
              <span style="position: absolute; top: -24px; left: 50%; transform: translateX(-50%); background: white; padding: 2px 6px; border-radius: 4px; font-size: 10px;">29V1-80060</span>
              <span style="font-size: 28px;">🚗</span>
            </div>
          `,
          className: '',
          iconSize: [32, 32],
          iconAnchor: [16, 16]
        })
      }).addTo(map);
  
    } else {
      const [prevLat, prevLng] = coordinatesArray[coordinatesArray.length - 2];
      const distance = calculateDistance(prevLat, prevLng, lat, lng);
      totalDistance += distance;
  
      // Vẽ đoạn đường mới
      L.polyline([[prevLat, prevLng], [lat, lng]], {
        color: 'blue',
        weight: 4
      }).addTo(linesGroup);
  
      // Kiểm tra lỗi GPS (di chuyển quá xa)
      if (distance > 0.2) {
        const time = new Date().toLocaleString();
        showError(`⚠️ GPS bất thường tại ${time}`);
        errorLogs.push(`GPS lỗi tại ${time}`);
        localStorage.setItem("errorLogs", JSON.stringify(errorLogs));
      }
  
      // Di chuyển marker xe
      if (carMarker) {
        if (carMarker.slideTo) {
          carMarker.slideTo([lat, lng], {
            duration: 1000,
            keepAtCenter: false
          });
        } else {
          carMarker.setLatLng([lat, lng]);
        }
      }
    }
  
    // Cập nhật thông tin quãng đường và chi phí
    const distEl = document.getElementById("distanceDisplay");
    if (distEl) distEl.textContent = `${Math.round(totalDistance)} km`;
    
    const kmCost = Math.round(totalDistance * 1000);
    const kmCostEl = document.getElementById("kmCostDisplay");
    if (kmCostEl) kmCostEl.textContent = `${kmCost.toLocaleString()} VND`;
    
    const totalEl = document.getElementById("costDisplay");
    if (totalEl) totalEl.textContent = `${(kmCost + cost).toLocaleString()} VND`;
    
  }
  

  function checkAndChargeTollRoad(roadName) {
    if (tollRoads[roadName] && !paidRoads.has(roadName)) {
      const fee = tollRoads[roadName];
      cost += fee;
      paidRoads.add(roadName);
      showError(`🚧 Qua đường thu phí: ${roadName.toUpperCase()} (+${fee} VND)`);
    }
  }

  async function fetchDataFromBackend() {
    try {
      const res = await fetch("http://localhost:3000/api/last-message");
      const data = await res.json();
      const errorBox = document.getElementById("errorBox");

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

      if (data.message) {
        const [lat, lng] = data.message.split(",").map(Number);
        const coordsEl = document.getElementById("coordinatesDisplay");
        if (coordsEl) coordsEl.innerHTML = `${lat.toFixed(4)}<br>${lng.toFixed(4)}`;
        if (!isNaN(lat) && !isNaN(lng) && lat !== 0 && lng !== 0) {
          updateMapWithCoordinates(lat, lng);
        }
      }

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

  setInterval(fetchDataFromBackend, 1000);

  document.getElementById("btnStopTracking")?.addEventListener("click", async () => {
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
      carMarker = null;
      startMarker = null;

      const box = document.getElementById("errorBox");
      if (box) box.innerHTML = '';
      errorShownGPS = false;
      errorShownSignal = false;

      document.getElementById("distanceDisplay").textContent = "0.00 km";
      document.getElementById("kmCostDisplay").textContent = "0 VND";
      document.getElementById("costDisplay").textContent = "0 VND";

    } catch (error) {
      console.error("Error clearing data:", error);
    }
  });

  document.getElementById("showErrors")?.addEventListener("click", () => {
    window.open("error.html", "_blank");
  });

  window.searchDevice = function () {
    const name = document.getElementById("deviceName")?.value;
    alert("Tìm kiếm thiết bị: " + name);
  };

  const today = new Date().toISOString().split("T")[0];
  const dateInput = document.getElementById("datePicker");
  if (dateInput) {
    dateInput.value = today;
    dateInput.addEventListener("change", function () {
      console.log("Ngày đã chọn:", this.value);
    });
  }
});
