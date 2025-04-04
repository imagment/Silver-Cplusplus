#ifndef SILVER_COLOR_HPP
#define SILVER_COLOR_HPP

#include <string>
#include <unordered_map>

enum class Color {
    RESET,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BOLD,
    UNDERLINE,
    BG_BLACK,
    BG_RED,
    BG_GREEN,
    BG_YELLOW,
    BG_BLUE,
    BG_MAGENTA,
    BG_CYAN,
    BG_WHITE
};

extern const std::string RESET;
extern const std::string BOLD;
extern const std::string FAINT;
extern const std::string ITALIC;
extern const std::string UNDERLINE;
extern const std::string SLOWBLINK;
extern const std::string RAPIDBLINK;
extern const std::string INVERT;
extern const std::string HIDDEN;
extern const std::string STRIKE;

extern const std::string RESET_BOLD;
extern const std::string RESET_FAINT;
extern const std::string RESET_ITALIC;
extern const std::string RESET_UNDERLINE;
extern const std::string RESET_BLINK;
extern const std::string RESET_INVERT;
extern const std::string RESET_HIDDEN;
extern const std::string RESET_STRIKE;

extern std::unordered_map<std::string, std::string> ansiMap;

std::string ToAnsiCode(Color color);
std::string ToAnsiCode(const std::string& color);
std::string ToAnsiCode(int color, bool isBackground = false);
std::string ProcessMarkdown(const std::string& input);

#endif
