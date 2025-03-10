const mongoose = require("mongoose");

const VehicleLocationSchema = new mongoose.Schema({
    latitude: Number,
    longitude: Number,
    roadName: String,
    trafficStatus: String,
    timestamp: { type: Date, default: Date.now },
});

module.exports = mongoose.model("VehicleLocation", VehicleLocationSchema);
