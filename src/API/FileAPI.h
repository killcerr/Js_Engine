#pragma once

#include "Utils/Defines.h"
#include "Utils/Using.h"

namespace jse {
class FileAPI : public ScriptClass {
public:
    explicit FileAPI() : ScriptClass(ScriptClass::ConstructFromCpp<FileAPI>{}) {}

    static Local<Object> newFileAPI() { return (new FileAPI())->getScriptObject(); }

public:
    METHODS(toString);

    static METHODS(readFile);

    static METHODS(writeFile);

    static METHODS(createDirectory);

    static METHODS(copyFile);

    static METHODS(moveFile);

    static METHODS(del);

    static METHODS(exists);

    static METHODS(isDirectory);

    static METHODS(isFile);

    static METHODS(getFileSize);

    static METHODS(listDirectory);

    static ClassDefine<FileAPI> builder;
};
} // namespace jse