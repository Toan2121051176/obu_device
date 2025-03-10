const mongoose = require('mongoose');

const uri = 'mongodb+srv://toansd9a:5mE6AeCyVdgU3ygI@cluster0.1gjar.mongodb.net/myDatabase?retryWrites=true&w=majority';

const connectDB = async () => {
    try {
        await mongoose.connect(uri, {
            useNewUrlParser: true,
            useUnifiedTopology: true,
        });
        console.log('✅ Kết nối MongoDB thành công!');
    } catch (error) {
        console.error('❌ Lỗi kết nối MongoDB:', error);
        process.exit(1); // Dừng chương trình nếu kết nối thất bại
    }
};

module.exports = connectDB;
