#include "EngineManager.h"
#include "API/APIHelper.h"
#include "BindAPI.h"
#include "EngineData.h"
#include "Entry.h"
#include <thread>
#include <unordered_map>

namespace jse {
EngineManager& EngineManager::getInstance() {
    static EngineManager instance;
    return instance;
}

bool EngineManager::hasEngine(EngineID engineId) { return this->mEngines.contains(engineId); }

bool EngineManager::destroyEngine(EngineID engineId) {
    if (!this->hasEngine(engineId)) {
        return false;
    }

    auto engine = this->mEngines[engineId];
    engine->destroy(); // 销毁引擎

    this->mEngines.erase(engineId);
    return true;
}

ScriptEngine* EngineManager::getEngine(EngineID engineId) {
    if (!this->hasEngine(engineId)) {
        return nullptr;
    }

    return this->mEngines[engineId];
}

ScriptEngine* EngineManager::createEngine() {
    static EngineID __NextEngineId = 0;
    EngineID const  engineId       = __NextEngineId++;

    auto engine              = new ScriptEngineImpl();
    this->mEngines[engineId] = engine;

    EngineScope scope(engine);                               // 进入引擎作用域
    BindAPI(engine);                                         // 绑定API
    engine->setData(std::make_shared<EngineData>(engineId)); // 设置引擎自身数据

    return engine;
}


void EngineManager::initMessageLoop() {
    mMessageLoopRunning = true;
    mMessageLoopThread  = std::thread([this]() {
        while (this->mMessageLoopRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (!this->mMessageLoopRunning) return;
            for (auto& [_, engine] : this->mEngines) {
                try {
                    if (EngineScope::currentEngine() == engine) {
                        engine->messageQueue()->loopQueue(script::utils::MessageQueue::LoopType::kLoopOnce);
                    } else {
                        EngineScope scope(engine);
                        engine->messageQueue()->loopQueue(script::utils::MessageQueue::LoopType::kLoopOnce);
                    }
                } catch (script ::Exception const& e) {
                    Entry::getInstance()->getLogger().error("Error occurred while looping message queue");
                    PrintScriptError(e);
                } catch (...) {
                    PrintScriptError("Unknown error occurred while looping message queue");
                }
            }
        }
    });
    mMessageLoopThread.detach();
}

void EngineManager::stopMessageLoop() {
    mMessageLoopRunning = false;
    if (mMessageLoopThread.joinable()) {
        mMessageLoopThread.join();
    }
}


} // namespace jse
