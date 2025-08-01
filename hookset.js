/**
 * Lua字节码替换工具
 */

// 配置
const MODULE_NAME = "libcocos2dlua.so";
const LUAL_LOADBUFFER_OFFSET = 0xBE23D4;
const LUA_FOLDER = "/data/local/tmp/";

// 查找模块并安装Hook
const module = Process.findModuleByName(MODULE_NAME);
if (!module) {
    console.log(`[-] 未找到模块: ${MODULE_NAME}`);
} else {
    console.log(`[+] 找到模块: ${MODULE_NAME} @ ${module.base}`);
    
    const luaLLoadBufferAddr = module.base.add(LUAL_LOADBUFFER_OFFSET);
    console.log(`[+] luaL_loadbuffer地址: ${luaLLoadBufferAddr}`);

    // 保存原函数
    const originalFunction = new NativeFunction(luaLLoadBufferAddr, 'pointer', ['pointer', 'pointer', 'int', 'pointer']);

    // Hook函数
    Interceptor.replace(luaLLoadBufferAddr, new NativeCallback(function(L, buff, size, name) {
        let chunkName = "unknown";

        // 读取脚本名称
        if (name && !ptr(name).isNull()) {
            try {
                chunkName = ptr(name).readCString() || "unknown";
            } catch (e) {
                chunkName = "read_error";
            }
        }

        console.log(`\n[*] 正在加载脚本: ${chunkName} (${size} 字节)`);

        // 检查脚本名称长度
        if (chunkName && chunkName.length < 128) {
            // 构建文件路径
            let fileName = chunkName.replace(/\//g, ".");
            let filePath = LUA_FOLDER + fileName;
            
            console.log(`[*] 查找替换文件: ${filePath}`);

            try {
                // 尝试打开文件
                const file = new File(filePath, "rb");
                if (file) {
                    // 获取文件大小
                    file.seek(0, 2); // SEEK_END
                    const fileSize = file.tell();
                    file.seek(0, 0); // SEEK_SET
                    
                    if (fileSize > 0) {
                        // 读取文件内容
                        const buffer = file.readBytes(fileSize);
                        file.close();
                        
                        if (buffer) {
                            console.log(`[+] 找到替换文件: ${fileSize} 字节`);
                            
                            // 分配新内存并复制文件内容
                            const newBuffer = Memory.alloc(fileSize);
                            
                            // 将文件内容复制到新内存
                            for (let i = 0; i < fileSize; i++) {
                                Memory.writeU8(newBuffer.add(i), buffer[i]);
                            }
                            
                            console.log(`[+] 使用替换文件调用原函数`);
                            
                            // 调用原函数
                            return originalFunction(L, newBuffer, ptr(fileSize), name);
                        }
                    }
                    file.close();
                }
            } catch (e) {
                console.log(`[-] 读取替换文件失败: ${e}`);
            }
        }

        // 没有找到替换文件，使用原始文件
        console.log(`[*] 使用原始文件`);
        return originalFunction(L, buff, size, name);

    }, 'pointer', ['pointer', 'pointer', 'int', 'pointer']));
    
    console.log("[+] Hook安装完成");
    console.log(`[*] 监控目录: ${LUA_FOLDER}`);
    console.log(`[*] 等待脚本加载...`);
}

console.log("[+] Lua字节码替换工具就绪");