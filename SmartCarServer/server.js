var net = require('net');
  
var server = net.createServer();
var sockets = [];


const Sequelize = require('sequelize');
const sequelize = new Sequelize('SmartCar', 'root', '123456', {
  host: 'localhost',
  dialect: 'mysql',

  pool: {
    max: 5,
    min: 0,
    acquire: 30000,
    idle: 10000
  },

  // 仅限 SQLite
  // storage: 'path/to/database.sqlite'
});

// 或者你可以简单地使用 uri 连接
// const sequelize = new Sequelize('postgres://user:pass@example.com:5432/dbname');


sequelize
  .authenticate()
  .then(() => {
    console.log('Connection has been established successfully.');
  })
  .catch(err => {
    console.error('Unable to connect to the database:', err);
  });


const Log = sequelize.define('log', {
  data: {
    type: Sequelize.STRING
  }
});

// // force: true 如果表已经存在，将会丢弃表
// Log.sync({force: false}).then(() => {
//   // 表已创建
//   return Log.create({
//     data: 'testdata'
//   });
// });



function Format(myDate,fmt){ //author: meizz
    var o = {
        "M+": myDate.getMonth() + 1, //月份
        "d+": myDate.getDate(), //日
        "h+": myDate.getHours(), //小时
        "m+": myDate.getMinutes(), //分
        "s+": myDate.getSeconds(), //秒
        "q+": Math.floor((myDate.getMonth() + 3) / 3), //季度
        "S": myDate.getMilliseconds() //毫秒
    };
    if (/(y+)/.test(fmt)) fmt = fmt.replace(RegExp.$1, (myDate.getFullYear() + "").substr(4 - RegExp.$1.length));
    for (var k in o)
        if (new RegExp("(" + k + ")").test(fmt))
            fmt = fmt.replace(RegExp.$1, (RegExp.$1.length == 1) ? (o[k]) : (("00" + o[k]).substr(("" + o[k]).length)));
    return fmt;
}

server.on('connection', function(socket) {
    console.log('got a new connection');
    sockets.push(socket);
    console.log('sockets length:', sockets.length);
    socket.on('data', function(data) {


        var timestamp = Date.parse(new Date());


        var myDate = Format((new Date),"yyyy-MM-dd hh:mm:ss.S");
        var str = JSON.stringify(data);  
        var str1 = JSON.parse(str);  
        // console.log(myDate + '  '+ str1.data);

        // force: true 如果表已经存在，将会丢弃表
        Log.sync({force: false}).then(() => {
          // 表已创建
          return Log.create({
            data: ''+str1.data
          });
        });


        sockets.forEach(function(otherSocket) {
            if (otherSocket !== socket) {
                otherSocket.write(data);
            }
        });
    });
    socket.on('close', function() {
        console.log('connection closed');
        var index = sockets.indexOf(socket);
        sockets.splice(index, 1);
        console.log('sockets length:', sockets.length);
    });
});
server.on('error', function(err) {
    console.log('Server error:', err.message);
});
server.on('close', function() {
    console.log('Server closed');
});
server.listen(4001);