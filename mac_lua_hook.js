/**
 * Mac版本Lua字节码Hook工具
 * 直接替换修改过的字节码文件
 */

console.log("[+] Mac版本Lua字节码Hook工具启动...");

// 配置
const MODULE_NAME = "ios";
const LUAL_LOADBUFFER_OFFSET = 0x839FFC;
const TARGET_MODULE = "app.module.dntgtest.fishConfig.Dntgtest_BulletSet";

/**
 * 获取应用沙盒目录
 */
function getAppSandboxDir() {
    try {
        const processPath = Process.enumerateModules()[0].path;
        
        // PlayCover路径处理
        if (processPath.includes("/Users/") && processPath.includes("/Library/Containers/io.playcover.PlayCover/")) {
            const match = processPath.match(/\/Users\/[^\/]+\/Library\/Containers\/io\.playcover\.PlayCover\/Applications\/[^\/]+/);
            if (match) {
                return match[0] + "/Documents/";
            }
        }
        
        return "./";
    } catch (e) {
        return "./";
    }
}

/**
 * 检查是否为目标模块
 */
function isTargetModule(chunkName) {
    return chunkName && chunkName.includes(TARGET_MODULE);
}

/**
 * 加载并替换字节码
 */
function loadAndReplaceBytecode(chunkName) {
    if (!isTargetModule(chunkName)) {
        return false;
    }
    
    try {
        const hookDir = getAppSandboxDir() + "luahook/";
        const hookFile = hookDir + "app.module.dntgtest.fishConfig.Dntgtest_BulletSet";
        
        console.log(`[🎯] 检测到目标模块: ${chunkName}`);
        console.log(`[+] 加载Hook字节码: ${hookFile}`);
        
        const file = new File(hookFile, "rb");
        const content = file.read();
        file.close();
        
        if (content && content.length > 0) {
            console.log(`[✅] Hook字节码加载成功 (${content.length} 字节)`);
            return {
                content: content,
                size: content.length
            };
        }
        
        console.log(`[-] Hook字节码为空: ${hookFile}`);
        return false;
    } catch (e) {
        console.log(`[-] 加载失败: ${e}`);
        return false;
    }
}

// 查找模块并安装Hook
const module = Process.findModuleByName(MODULE_NAME);
if (!module) {
    console.log(`[-] 未找到模块: ${MODULE_NAME}`);
} else {
    console.log(`[+] 找到模块: ${MODULE_NAME} @ ${module.base}`);
    
    const luaLLoadBufferAddr = module.base.add(LUAL_LOADBUFFER_OFFSET);
    console.log(`[+] luaL_loadbuffer地址: ${luaLLoadBufferAddr}`);
    
    // Hook函数
    Interceptor.attach(luaLLoadBufferAddr, {
        onEnter: function(args) {
            const buffer = args[1];
            const size = args[2];
            const namePtr = args[3];
            
            if (!buffer.isNull() && size.toInt32() > 0) {
                let chunkName = "unknown";
                if (!namePtr.isNull()) {
                    try {
                        chunkName = namePtr.readCString() || "unknown";
                    } catch (e) {
                        chunkName = "read_error";
                    }
                }
                
                // 检查是否需要替换
                const replacement = loadAndReplaceBytecode(chunkName);
                if (replacement) {
                    // 替换缓冲区内容
                    const newBuffer = Memory.alloc(replacement.size);
                    newBuffer.writeByteArray(replacement.content);
                    
                    // 修改参数
                    args[1] = newBuffer;
                    args[2] = ptr(replacement.size);
                    
                    console.log(`[✅] 字节码已替换: ${chunkName}`);
                }
            }
        }
    });
    
    console.log("[+] Hook安装完成！");
    console.log(`[*] 目标: ${TARGET_MODULE}`);
    console.log(`[*] 替换文件: ${getAppSandboxDir()}luahook/app.module.dntgtest.fishConfig.Dntgtest_BulletSet`);
} 