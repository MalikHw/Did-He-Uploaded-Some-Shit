#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/modify/UploadPopup.hpp>

using namespace geode::prelude;

// Converts m_levelLength int to a readable string so i don't fuck out mid code
static std::string lengthString(int len) {
    switch (len) {
        case 0: return "Tiny";
        case 1: return "Short";
        case 2: return "Medium";
        case 3: return "Long";
        case 4: return "XL";
        case 5: return "Plat";
        default: return "Unknown";
    }
}

// Processes {isUploaded"..."} and {isUpdated"..."} conditional vars
static std::string processConditionals(std::string text, bool isUpdate) {
    auto process = [&](std::string& str, std::string const& tag, bool show) {
        std::string open = "{" + tag + "\"";
        size_t pos = 0;
        while ((pos = str.find(open, pos)) != std::string::npos) {
            size_t contentStart = pos + open.size();
            size_t closing = str.find("\"}", contentStart);
            if (closing == std::string::npos) break;
            std::string inner = str.substr(contentStart, closing - contentStart);
            std::string replacement = show ? inner : "";
            str.replace(pos, closing + 2 - pos, replacement);
            pos += replacement.size();
        }
    };
    process(text, "isUploaded", !isUpdate);
    process(text, "isUpdated", isUpdate);
    return text;
}

// fills in all the template vars in the message
static std::string buildMessage(GJGameLevel* level, bool isUpdate) {
    auto mod = Mod::get();
    bool useCustom = mod->getSettingValue<bool>("use-custom-text");
    bool rolePing  = mod->getSettingValue<bool>("role-ping");
    std::string roleID = mod->getSettingValue<std::string>("role-id");
    std::string creator = level->m_creatorName;
    std::string name    = level->m_levelName;
    std::string id      = std::to_string((int)level->m_levelID);
    std::string length  = lengthString(level->m_levelLength);
    std::string objects = std::to_string((int)level->m_objectCount);
    std::string text;
    if (useCustom) {
        text = mod->getSettingValue<std::string>("custom-text");
        // replace simple vars
        auto replace = [&](std::string& s, std::string const& from, std::string const& to) {
            size_t pos = 0;
            while ((pos = s.find(from, pos)) != std::string::npos) {
                s.replace(pos, from.size(), to);
                pos += to.size();
            }
        };
        replace(text, "{n}",       "\n");
        replace(text, "{creator}", creator);
        replace(text, "{name}",    name);
        replace(text, "{id}",      id);
        replace(text, "{lengh}",   length);
        replace(text, "{objects}", objects);
        // role ping in custom text via {role}
        replace(text, "{role}", rolePing ? fmt::format("<@&{}>", roleID) : "");
        text = processConditionals(text, isUpdate);
    } else {
        if (isUpdate) {
            text = fmt::format(
                "## Level Updated!\n"
                "{} UPDATED a level!\n"
                "### {}\n"
                "#### {}\n"
                "-# {} ({} objects)",
                creator, name, id, length, objects
            );
        } else {
            text = fmt::format(
                "## New Level!\n"
                "{} dropped a new level!\n"
                "### {}\n"
                "#### {}\n"
                "-# {} ({} objects)",
                creator, name, id, length, objects
            );
        }
        // role ping goes at the bottom spoilered in the preset
        if (rolePing) {
            text += fmt::format("\n||<@&{}>||", roleID);
        }
    }
    return text;
}

// Fire and forget, sends the webhook, logs success/failure
static void sendWebhook(GJGameLevel* level, bool isUpdate) {
    if (!Mod::get()->getSettingValue<bool>("enabled")) return;
    std::string webhookURL = Mod::get()->getSettingValue<std::string>("webhook-url");
    if (webhookURL.empty()) {
        log::warn("Webhook URL is empty, skipping");
        return;
    }
    std::string message = buildMessage(level, isUpdate);
    auto json = matjson::Value();
    json["content"] = message;
    auto req = web::WebRequest();
    req.header("Content-Type", "application/json");
    req.bodyJSON(json);
    // async::spawn
    req.post(webhookURL).listen(
        [](web::WebResponse* res) {
            if (res->ok()) {
                log::info("Webhook sent successfully ({})", res->code());
            } else {
                log::warn("Webhook failed with status {}: {}", res->code(), res->string().unwrapOr("no body"));
            }
        },
        []() {
            log::warn("Webhook request was cancelled");
        }
    );
}

class $modify(UploadPopup) {
    void levelUploadFinished(GJGameLevel* level) {
        UploadPopup::levelUploadFinished(level);
        // m_levelVersion > 1 means the level already on the servers
        bool isUpdate = level->m_levelVersion > 1;
        if (isUpdate && !Mod::get()->getSettingValue<bool>("send-on-update")) return;
        sendWebhook(level, isUpdate);
    }
};
