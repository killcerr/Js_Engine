#include "PermissibleAPI.h"
#include "API/CommandSenderAPI.h"
#include "API/PermissionAPI.h"
#include "API/PluginAPI.h"
#include "APIHelper.h"
#include "Loader/JavaScriptPlugin.h"
#include "Utils/Convert.h"
#include "Utils/Defines.h"
#include "Utils/Using.h"
#include "endstone/permissions/permissible.h"
#include "endstone/plugin/plugin.h"
#include "endstone/util/result.h"
#include "fmt/core.h"
#include <stdexcept>


namespace jse {

ClassDefine<PermissibleAPI> PermissibleAPI::builder =
    defineClass<PermissibleAPI>("Permissible")
        .constructor(nullptr)
        .instanceFunction("toString", &PermissibleAPI::toString)
        .instanceFunction("isOp", &PermissibleAPI::isOP)
        .instanceFunction("setOp", &PermissibleAPI::setOp)
        .instanceFunction("isPermissionSet", &PermissibleAPI::isPermissionSet)
        .instanceFunction("hasPermission", &PermissibleAPI::hasPermission)
        .instanceFunction("addAttachment", &PermissibleAPI::addAttachment)
        .instanceFunction("removeAttachment", &PermissibleAPI::removeAttachment)
        .instanceFunction("recalculatePermissions", &PermissibleAPI::recalculatePermissions)
        .instanceFunction("getEffectivePermissions", &PermissibleAPI::getEffectivePermissions)
        .instanceFunction("asCommandSender", &PermissibleAPI::getEffectivePermissions)
        .build();


Local<Value> PermissibleAPI::toString(Arguments const& args) { return ConvertToScriptX("<Permissible>"); };

Local<Value> PermissibleAPI::isOP(Arguments const& args) {
    try {
        return ConvertToScriptX(this->mPermissible->isOp());
    }
    Catch;
}

Local<Value> PermissibleAPI::setOp(Arguments const& args) {
    try {
        this->mPermissible->setOp(args[0].asBoolean().value());
        return Local<Value>();
    }
    Catch;
}

Local<Value> PermissibleAPI::isPermissionSet(Arguments const& args) {
    try {
        if (args[0].isString()) {
            // overload 1
            return ConvertToScriptX(this->mPermissible->isPermissionSet(args[0].asString().toString()));
        } else if (IsInstanceOf<PermissionAPI>(args[0])) {
            // overload 2
            return ConvertToScriptX(this->mPermissible->isPermissionSet(*PermissionAPI::unWarp(args[0])));
        }
        return ConvertToScriptX(false);
    }
    Catch;
}

Local<Value> PermissibleAPI::hasPermission(Arguments const& args) {
    try {
        if (args[0].isString()) {
            // overload 1
            return ConvertToScriptX(this->mPermissible->hasPermission(args[0].asString().toString()));
        } else if (IsInstanceOf<PermissionAPI>(args[0])) {
            // overload 2
            return ConvertToScriptX(this->mPermissible->hasPermission(*PermissionAPI::unWarp(args[0])));
        }
        return ConvertToScriptX(false);
    }
    Catch;
}

Local<Value> PermissibleAPI::addAttachment(Arguments const& args) {
    try {
        endstone::Result<endstone::PermissionAttachment*> val;
        if (IsInstanceOf<PluginAPI>(args[0]) && args.size() == 1) {
            // overload 1
            val = this->mPermissible->addAttachment(*ENGINE_DATA()->mPlugin);
        } else if (IsInstanceOf<PluginAPI>(args[0]) && args[1].isString() && args.size() == 3) {
            // overload 2
            val = this->mPermissible->addAttachment(
                *ENGINE_DATA()->mPlugin,
                args[1].asString().toString(),
                args[2].asBoolean().value()
            );
        }

        if (!val.has_value()) {
            throw std::runtime_error(fmt::format("{}\n{}", val.error().getMessage(), val.error().getStackTrace()));
        }

        // TODO: PermissionAttachment constructor
        return Local<Value>();
    }
    Catch;
}

Local<Value> PermissibleAPI::removeAttachment(Arguments const& args) {
    try {
        // TODO: PermissionAttachment
        return Local<Value>();
    }
    Catch;
}

Local<Value> PermissibleAPI::recalculatePermissions(Arguments const& args) {
    try {
        this->mPermissible->recalculatePermissions();
        return Local<Value>();
    }
    Catch;
}

Local<Value> PermissibleAPI::getEffectivePermissions(Arguments const& args) {
    try {
        // TODO: PermissionAttachmentInfo
        return Local<Value>();
    }
    Catch;
}

Local<Value> PermissibleAPI::asCommandSender(Arguments const& args) {
    try {
        // TODO: CommandSender
        return Local<Value>();
    }
    Catch;
}


} // namespace jse