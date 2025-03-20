#include <string>
#include <regex>

const std::string RESET         = "\033[0m";
const std::string BOLD          = "\033[1m";
const std::string FAINT         = "\033[2m";
const std::string ITALIC        = "\033[3m";
const std::string UNDERLINE     = "\033[4m";
const std::string SLOWBLINK     = "\033[5m";
const std::string RAPIDBLINK    = "\033[6m";
const std::string INVERT        = "\033[7m";  // Swap foreground and background
const std::string HIDDEN        = "\033[8m";
const std::string STRIKE        = "\033[9m";

const std::string RESET_BOLD      = "\033[22m";
const std::string RESET_FAINT     = "\033[22m";  // No separate reset, same as bold
const std::string RESET_ITALIC    = "\033[23m";
const std::string RESET_UNDERLINE = "\033[24m";
const std::string RESET_BLINK     = "\033[25m";  // Resets both slow and rapid blink
const std::string RESET_INVERT    = "\033[27m";
const std::string RESET_HIDDEN    = "\033[28m";
const std::string RESET_STRIKE    = "\033[29m";

std::string ToAnsiCode(Color color) {
    switch (color) {
        case Color::RESET: return "\033[0m";
        case Color::BLACK: return "\033[30m";
        case Color::RED: return "\033[31m";
        case Color::GREEN: return "\033[32m";
        case Color::YELLOW: return "\033[33m";
        case Color::BLUE: return "\033[34m";
        case Color::MAGENTA: return "\033[35m";
        case Color::CYAN: return "\033[36m";
        case Color::WHITE: return "\033[37m";
        case Color::BG_RED: return "\033[48;5;1m";
        case Color::BG_BLUE: return "\033[48;5;4m";
        case Color::BG_GREEN: return "\033[48;5;2m";
        case Color::BG_YELLOW: return "\033[48;5;3m";
        case Color::BG_BLACK: return "\033[48;5;0m";
        case Color::BG_WHITE: return "\033[48;5;15m";
        case Color::BG_MAGENTA: return "\033[48;5;5m";
        case Color::BG_CYAN: return "\033[48;5;6m";
        default: return "\033[0m"; // Reset for unknown
    }
}

std::string ToAnsiCode(int color, bool isBackground) {
    if (color == 0) return "\033[0m"; // RESET
    if (color == 1) return "\033[1m"; // BOLD
    if (color == 4) return "\033[4m"; // UNDERLINE

    if (isBackground) {
        // Background colors
        if (color >= 0 && color <= 255) {
            return "\033[48;5;" + std::to_string(color) + "m";
        }
    } else {
        // Foreground colors
        if (color >= 0 && color <= 255) {
            return "\033[38;5;" + std::to_string(color) + "m";
        }
    }

    return "\033[0m"; // Reset for unknown input
}

std::unordered_map<std::string, std::string> ansiMap = {
    {"reset", "\033[0m"},
    {"b", "\033[1m"},         // Bold
    {"/b", "\033[22m"},       // Bold Off
    {"i", "\033[3m"},         // Italic
    {"/i", "\033[23m"},       // Italic Off
    {"u", "\033[4m"},         // Underline
    {"/u", "\033[24m"},       // Underline Off
    {"strike", "\033[9m"},    // Strikethrough
    {"/strike", "\033[29m"},  // Strikethrough Off
    {"slowblink", "\033[5m"}, // Slow Blink
    {"/slowblink", "\033[25m"}, // Blink Off
    {"rapidblink", "\033[6m"},  // Rapid Blink
    {"/rapidblink", "\033[25m"}, // Blink Off (Same as slowblink off)
    {"hidden", "\033[8m"},    // Hidden (Invisible Text)
    {"/hidden", "\033[28m"},  // Hidden Off
    {"invert", "\033[7m"},    // Invert (Swap foreground and background)
    {"/invert", "\033[27m"},  // Invert Off
    {"faint", "\033[2m"},     // Faint (Dim text)
    {"/faint", "\033[22m"},   // Faint Off (Same as bold off)
    {"red", "\033[31m"},
    {"yellow", "\033[33m"},
    {"blue", "\033[34m"},
    {"green", "\033[32m"},
    {"black", "\033[30m"},
    {"white", "\033[37m"},
    {"magenta", "\033[35m"},
    {"cyan", "\033[36m"},
    {"bgred", "\033[41m"},
    {"bgblue", "\033[44m"},
    {"bggreen", "\033[42m"},
    {"bgyellow", "\033[43m"},
    {"bgblack", "\033[40m"},
    {"bgwhite", "\033[47m"},
    {"bgmagenta", "\033[45m"},
    {"bgcyan", "\033[46m"}
};
std::string ProcessMarkdown(const std::string& input) {
    std::stack<std::string> activeTags;
    std::regex tagRegex(R"(<(/?[a-zA-Z0-9 ]+)>)");
    std::string result;
    std::smatch match;
    std::string::const_iterator searchStart(input.cbegin());

    while (std::regex_search(searchStart, input.cend(), match, tagRegex)) {
        result.append(searchStart, match[0].first);
        std::string tag = match[1].str();

        if (tag == "br") {
            result.append("\n");
        }
        else if (tag == "/reset") {
            while (!activeTags.empty()) activeTags.pop();
            result.append(ansiMap["reset"]);
        }
        else if (tag[0] == '/') {  // Closing tag
            std::string closingTag = tag.substr(1);
            if (!activeTags.empty() && activeTags.top() == closingTag) {
                activeTags.pop();
                result.append(ansiMap["reset"]);
                
                // Reapply remaining active tags
                std::stack<std::string> tempStack = activeTags;
                while (!tempStack.empty()) {
                    result.append(ansiMap[tempStack.top()]);
                    tempStack.pop();
                }
            } else {
                result.append(match[0].str());  // Push the original tag
            }
        }
        else {  // Opening tag
            std::string ansiCode;
            if (tag.find("color ") == 0 || tag.find("bg ") == 0) {
                size_t pos = tag.find(" ");
                if (pos != std::string::npos) {
                    std::string colorStr = tag.substr(pos + 1);
                    if (!colorStr.empty() && std::all_of(colorStr.begin(), colorStr.end(), ::isdigit)) {
                        int colorCode = std::stoi(colorStr);
                        ansiCode = ToAnsiCode(colorCode, tag.find("bg ") == 0);
                    }
                }
            }
            else if (ansiMap.find(tag) != ansiMap.end()) {
                ansiCode = ansiMap[tag];
            }

            if (!ansiCode.empty()) {
                activeTags.push(tag);
                result.append(ansiCode);
            } else {
                result.append(match[0].str());  // Push the original tag
            }
        }
        searchStart = match[0].second;
    }

    result.append(searchStart, input.cend());
    return result;
}

