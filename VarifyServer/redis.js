const config_module = require('./config.json'); // 确保你正确引入 config.json 文件
const Redis = require("ioredis");
console.log("Connecting to Redis with:", config_module.redis);
// 创建 Redis 客户端实例
const RedisCli = new Redis({
    host: config_module.redis_host,        // Redis 服务器主机名
    port: config_module.redis_port,        // Redis 服务器端口号
    password: config_module.redis_passwd,  // Redis 密码
    maxRetriesPerRequest: 5,               // 设置最大重试次数
    connectTimeout: 10000,                 // 设置连接超时时间
    retryStrategy(times) {
        console.log(`Retry attempt #${times}`);  // 打印每次重试的信息
        return Math.min(times * 100, 2000);  // 重试间隔逐渐增加
    }
});

// 监听错误信息
RedisCli.on("error", function (err) {
    console.error("RedisCli connect error:", err);
    // 可以考虑在这里尝试重连或者进行其他处理
    RedisCli.quit();
});

// 测试 Redis 连接
RedisCli.on("connect", () => {
    console.log("Successfully connected to Redis!");
});

// 获取 Redis 键值
async function GetRedis(key) {
    try {
        const result = await RedisCli.get(key);
        if (result === null) {
            console.log(`Result: <${result}> This key cannot be found...`);
            return null;
        }
        console.log(`Result: <${result}> Get key success!...`);
        return result;
    } catch (error) {
        console.error("GetRedis error is", error);
        return null;
    }
}

// 查询 Redis 中的键是否存在
async function QueryRedis(key) {
    try {
        const result = await RedisCli.exists(key); // 阻塞等待
        if (result === 0) {
            console.log(`Result: <${result}> This key is null...`);
            return null;
        }
        console.log(`Result: <${result}> With this value!...`);
        return result;
    } catch (error) {
        console.error("QueryRedis error is", error);
        return null;
    }
}

// 设置 Redis 键值和过期时间
async function SetRedisExpire(key, value, exptime) {
    try {
        await RedisCli.set(key, value);
        await RedisCli.expire(key, exptime);
        console.log(`Set ${key} with expiration ${exptime}`);
        return true;
    } catch (error) {
        console.error("SetRedisExpire error is", error);
        return false;
    }
}

// 退出 Redis 客户端
function Quit() {
    RedisCli.quit();
}

// 导出接口以便外部使用
module.exports = { GetRedis, QueryRedis, Quit, SetRedisExpire };
