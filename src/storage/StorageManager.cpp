#include "storage/StorageManager.h"

#include <SD_MMC.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <driver/sdmmc_types.h>
#include <esp_heap_caps.h>
#include <utility>

#include "board/BoardConfig.h"
#include "storage/EpubConverter.h"

#ifndef RSVP_ON_DEVICE_EPUB_CONVERSION
#define RSVP_ON_DEVICE_EPUB_CONVERSION 0
#endif

#ifndef RSVP_MAX_BOOK_WORDS
#define RSVP_MAX_BOOK_WORDS 0
#endif

namespace {

constexpr const char *kMountPoint = "/sdcard";
constexpr const char *kBooksPath = "/books";
constexpr size_t kMaxBookWords = static_cast<size_t>(RSVP_MAX_BOOK_WORDS);
constexpr size_t kMaxChapterTitleChars = 64;
constexpr int kSdFrequenciesKhz[] = {
    SDMMC_FREQ_DEFAULT,
    10000,
    SDMMC_FREQ_PROBING,
};

bool hasBookWordLimit() { return kMaxBookWords > 0; }

bool reachedBookWordLimit(size_t wordCount) {
  return hasBookWordLimit() && wordCount >= kMaxBookWords;
}

bool isWordBoundary(char c) { return c <= ' '; }

bool prefixHasBoundary(const String &lowered, const char *prefix) {
  const size_t prefixLength = std::strlen(prefix);
  if (!lowered.startsWith(prefix)) {
    return false;
  }
  if (lowered.length() == prefixLength) {
    return true;
  }

  const char next = lowered[prefixLength];
  return next <= ' ' || next == ':' || next == '.' || next == '-';
}

bool booksDirectoryExists() {
  File dir = SD_MMC.open(kBooksPath);
  const bool exists = dir && dir.isDirectory();
  if (dir) {
    dir.close();
  }
  return exists;
}

bool hasTextExtension(const String &path) {
  String lowered = path;
  lowered.toLowerCase();
  return lowered.endsWith(".txt");
}

bool hasRsvpExtension(const String &path) {
  String lowered = path;
  lowered.toLowerCase();
  return lowered.endsWith(".rsvp");
}

bool hasEpubExtension(const String &path) {
  String lowered = path;
  lowered.toLowerCase();
  return lowered.endsWith(".epub");
}

bool hasRsvpSibling(const String &path) {
  String siblingPath = path;
  const int dot = siblingPath.lastIndexOf('.');
  if (dot > 0) {
    siblingPath = siblingPath.substring(0, dot);
  }
  siblingPath += ".rsvp";

  File sibling = SD_MMC.open(siblingPath);
  const bool exists = sibling && !sibling.isDirectory() && sibling.size() > 0;
  if (sibling) {
    sibling.close();
  }
  return exists;
}

String epubSiblingPathForRsvp(const String &rsvpPath) {
  String epubPath = rsvpPath;
  // Strip .rsvp first.
  const int dot = epubPath.lastIndexOf('.');
  if (dot > 0) {
    epubPath = epubPath.substring(0, dot);
  }
  // Now strip a trailing ".partN" if present so split parts resolve to the
  // shared source EPUB (e.g. "GEB.part2" → "GEB", giving us "GEB.epub").
  const int partAt = epubPath.lastIndexOf(".part");
  if (partAt > 0) {
    bool allDigits = partAt + 5 < static_cast<int>(epubPath.length());
    for (int i = partAt + 5; i < static_cast<int>(epubPath.length()) && allDigits; ++i) {
      if (!isDigit(epubPath[i])) allDigits = false;
    }
    if (allDigits) epubPath = epubPath.substring(0, partAt);
  }
  epubPath += ".epub";
  return epubPath;
}

String normalizeBookPath(const String &path) {
  if (path.startsWith("/")) {
    return path;
  }
  return String(kBooksPath) + "/" + path;
}

String displayNameForPath(const String &path) {
  const int separator = path.lastIndexOf('/');
  if (separator < 0) {
    return path;
  }
  return path.substring(separator + 1);
}

String displayNameWithoutExtension(const String &path) {
  String name = displayNameForPath(path);
  String lowered = name;
  lowered.toLowerCase();
  if (lowered.endsWith(".txt")) {
    name.remove(name.length() - 4);
  } else if (lowered.endsWith(".rsvp")) {
    name.remove(name.length() - 5);
  } else if (lowered.endsWith(".epub")) {
    name.remove(name.length() - 5);
  }
  return name;
}

String rsvpCachePathForEpub(const String &epubPath) {
  String outputPath = epubPath;
  const int dot = outputPath.lastIndexOf('.');
  if (dot > 0) {
    outputPath = outputPath.substring(0, dot);
  }
  outputPath += ".rsvp";
  return outputPath;
}

struct EpubProgressContext {
  StorageManager::StatusCallback statusCallback = nullptr;
  void *statusContext = nullptr;
  String title;
  String label;
  int basePercent = 0;
  int spanPercent = 100;
};

void handleEpubProgress(void *rawContext, const char *line1, const char *line2,
                        int progressPercent) {
  EpubProgressContext *context = static_cast<EpubProgressContext *>(rawContext);
  if (context == nullptr) {
    return;
  }

  progressPercent = std::max(0, std::min(100, progressPercent));
  const int overallPercent =
      context->basePercent + ((context->spanPercent * progressPercent) / 100);
  const String detail = String(line1 == nullptr ? "" : line1) + " - " +
                        String(line2 == nullptr ? "" : line2);
  const char *title = context->title.isEmpty() ? "EPUB" : context->title.c_str();
  Serial.printf("[epub-progress] %d%% %s | %s | %s\n", overallPercent, title,
                context->label.c_str(), detail.c_str());

  // Keep the display on the static "Converting EPUB" screen while ZIP work is active.
  // Full-screen redraws from inside this callback have proven risky while the SD archive is open.
  yield();
  delay(0);
}

bool fileExistsAndHasBytes(const String &path) {
  File file = SD_MMC.open(path);
  const bool exists = file && !file.isDirectory() && file.size() > 0;
  if (file) {
    file.close();
  }
  return exists;
}

bool hasCurrentEpubCache(const String &epubPath) {
  const String rsvpPath = rsvpCachePathForEpub(epubPath);
  return fileExistsAndHasBytes(rsvpPath) && EpubConverter::isCurrentCache(rsvpPath);
}

bool markerExists(const String &path) {
  File file = SD_MMC.open(path);
  const bool exists = file && !file.isDirectory();
  if (file) {
    file.close();
  }
  return exists;
}

String epubLibraryLabel(const String &path) {
  const String rsvpPath = rsvpCachePathForEpub(path);
  if (markerExists(rsvpPath + ".failed")) {
    return "EPUB failed - check serial";
  }
  if (markerExists(rsvpPath + ".converting") || markerExists(rsvpPath + ".tmp")) {
    return "EPUB interrupted";
  }
  return "EPUB - converts on open";
}

int pathIndexIn(const std::vector<String> &paths, const String &target) {
  for (size_t i = 0; i < paths.size(); ++i) {
    if (paths[i] == target) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

void logHeapSnapshot(const char *label) {
  Serial.printf("[heap] %s free8=%lu free_spiram=%lu largest8=%lu largest_spiram=%lu\n",
                label == nullptr ? "" : label,
                static_cast<unsigned long>(heap_caps_get_free_size(MALLOC_CAP_8BIT)),
                static_cast<unsigned long>(heap_caps_get_free_size(MALLOC_CAP_SPIRAM)),
                static_cast<unsigned long>(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)),
                static_cast<unsigned long>(heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM)));
}

std::vector<String> collectBookPaths() {
  std::vector<String> bookPaths;

  File dir = SD_MMC.open(kBooksPath);
  if (!dir || !dir.isDirectory()) {
    if (dir) {
      dir.close();
    }
    return bookPaths;
  }

  File entry = dir.openNextFile();
  size_t scanned = 0;
  while (entry) {
    if (!entry.isDirectory()) {
      const String path = normalizeBookPath(String(entry.name()));
      const bool staleGeneratedRsvp =
          hasRsvpExtension(path) && fileExistsAndHasBytes(epubSiblingPathForRsvp(path)) &&
          !EpubConverter::isCurrentCache(path);
      const bool readableText = hasTextExtension(path) && !hasRsvpSibling(path);
      const bool pendingEpub =
          RSVP_ON_DEVICE_EPUB_CONVERSION && hasEpubExtension(path) && !hasCurrentEpubCache(path);
      if ((!staleGeneratedRsvp && hasRsvpExtension(path)) || readableText || pendingEpub) {
        bookPaths.push_back(path);
      }
    }
    entry.close();
    if ((++scanned & 0x07) == 0) {
      yield();
    }
    entry = dir.openNextFile();
  }

  dir.close();

  std::sort(bookPaths.begin(), bookPaths.end(), [](const String &left, const String &right) {
    String leftKey = displayNameForPath(left);
    String rightKey = displayNameForPath(right);
    leftKey.toLowerCase();
    rightKey.toLowerCase();
    return leftKey < rightKey;
  });

  return bookPaths;
}

bool isTrimmableEdgeChar(char c) {
  switch (c) {
    case '"':
    case '\'':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '<':
    case '>':
      return true;
    default:
      return false;
  }
}

bool isUtf8Continuation(uint8_t value) { return (value & 0xC0) == 0x80; }

bool decodeUtf8Codepoint(const String &text, size_t &index, uint32_t &codepoint) {
  const uint8_t first = static_cast<uint8_t>(text[index++]);
  if (first < 0x80) {
    codepoint = first;
    return true;
  }

  uint8_t continuationCount = 0;
  uint32_t minimumValue = 0;
  if ((first & 0xE0) == 0xC0) {
    codepoint = first & 0x1F;
    continuationCount = 1;
    minimumValue = 0x80;
  } else if ((first & 0xF0) == 0xE0) {
    codepoint = first & 0x0F;
    continuationCount = 2;
    minimumValue = 0x800;
  } else if ((first & 0xF8) == 0xF0) {
    codepoint = first & 0x07;
    continuationCount = 3;
    minimumValue = 0x10000;
  } else {
    return false;
  }

  if (index + continuationCount > text.length()) {
    return false;
  }

  for (uint8_t i = 0; i < continuationCount; ++i) {
    const uint8_t next = static_cast<uint8_t>(text[index]);
    if (!isUtf8Continuation(next)) {
      return false;
    }
    ++index;
    codepoint = (codepoint << 6) | (next & 0x3F);
  }

  if (codepoint < minimumValue || codepoint > 0x10FFFF ||
      (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
    return false;
  }

  return true;
}

void appendAsciiText(String &target, const char *text) {
  while (*text != '\0') {
    target += *text;
    ++text;
  }
}

void appendAsciiApproximation(String &target, uint32_t codepoint) {
  if (codepoint >= 32 && codepoint <= 126) {
    target += static_cast<char>(codepoint);
    return;
  }

  if (codepoint == '\t' || codepoint == '\n' || codepoint == '\r' || codepoint == 0x00A0 ||
      codepoint == 0x1680 || codepoint == 0x180E || codepoint == 0x2028 ||
      codepoint == 0x2029 || codepoint == 0x202F || codepoint == 0x205F ||
      codepoint == 0x3000 || (codepoint >= 0x2000 && codepoint <= 0x200A)) {
    target += ' ';
    return;
  }

  if ((codepoint >= 0x00C0 && codepoint <= 0x00C5) || codepoint == 0x0100 ||
      codepoint == 0x0102 || codepoint == 0x0104) {
    target += 'A';
    return;
  }
  if ((codepoint >= 0x00E0 && codepoint <= 0x00E5) || codepoint == 0x0101 ||
      codepoint == 0x0103 || codepoint == 0x0105) {
    target += 'a';
    return;
  }
  if (codepoint >= 0x00C8 && codepoint <= 0x00CB) {
    target += 'E';
    return;
  }
  if (codepoint >= 0x00E8 && codepoint <= 0x00EB) {
    target += 'e';
    return;
  }
  if (codepoint >= 0x00CC && codepoint <= 0x00CF) {
    target += 'I';
    return;
  }
  if (codepoint >= 0x00EC && codepoint <= 0x00EF) {
    target += 'i';
    return;
  }
  if ((codepoint >= 0x00D2 && codepoint <= 0x00D6) || codepoint == 0x00D8) {
    target += 'O';
    return;
  }
  if ((codepoint >= 0x00F2 && codepoint <= 0x00F6) || codepoint == 0x00F8) {
    target += 'o';
    return;
  }
  if (codepoint >= 0x00D9 && codepoint <= 0x00DC) {
    target += 'U';
    return;
  }
  if (codepoint >= 0x00F9 && codepoint <= 0x00FC) {
    target += 'u';
    return;
  }

  switch (codepoint) {
    case 0x2018:
    case 0x2019:
    case 0x201A:
    case 0x201B:
    case 0x2032:
    case 0x2035:
      target += '\'';
      return;
    case 0x201C:
    case 0x201D:
    case 0x201E:
    case 0x201F:
    case 0x00AB:
    case 0x00BB:
    case 0x2033:
    case 0x2036:
      target += '"';
      return;
    case 0x2010:
    case 0x2011:
    case 0x2012:
    case 0x2013:
    case 0x2014:
    case 0x2015:
    case 0x2043:
    case 0x2212:
      target += '-';
      return;
    case 0x2026:
      appendAsciiText(target, "...");
      return;
    case 0x2022:
    case 0x00B7:
    case 0x2219:
      target += '*';
      return;
    case 0x00A9:
      appendAsciiText(target, "(c)");
      return;
    case 0x00AE:
      appendAsciiText(target, "(r)");
      return;
    case 0x2122:
      appendAsciiText(target, "TM");
      return;
    case 0x00C6:
    case 0x01E2:
    case 0x01FC:
      appendAsciiText(target, "AE");
      return;
    case 0x00E6:
    case 0x01E3:
    case 0x01FD:
      appendAsciiText(target, "ae");
      return;
    case 0x00C7:
    case 0x0106:
    case 0x0108:
    case 0x010A:
    case 0x010C:
      target += 'C';
      return;
    case 0x00E7:
    case 0x0107:
    case 0x0109:
    case 0x010B:
    case 0x010D:
      target += 'c';
      return;
    case 0x00D0:
    case 0x010E:
    case 0x0110:
      target += 'D';
      return;
    case 0x00F0:
    case 0x010F:
    case 0x0111:
      target += 'd';
      return;
    case 0x0112:
    case 0x0114:
    case 0x0116:
    case 0x0118:
    case 0x011A:
      target += 'E';
      return;
    case 0x0113:
    case 0x0115:
    case 0x0117:
    case 0x0119:
    case 0x011B:
      target += 'e';
      return;
    case 0x011C:
    case 0x011E:
    case 0x0120:
    case 0x0122:
      target += 'G';
      return;
    case 0x011D:
    case 0x011F:
    case 0x0121:
    case 0x0123:
      target += 'g';
      return;
    case 0x0124:
    case 0x0126:
      target += 'H';
      return;
    case 0x0125:
    case 0x0127:
      target += 'h';
      return;
    case 0x0128:
    case 0x012A:
    case 0x012C:
    case 0x012E:
    case 0x0130:
      target += 'I';
      return;
    case 0x0129:
    case 0x012B:
    case 0x012D:
    case 0x012F:
    case 0x0131:
      target += 'i';
      return;
    case 0x0134:
      target += 'J';
      return;
    case 0x0135:
      target += 'j';
      return;
    case 0x0136:
      target += 'K';
      return;
    case 0x0137:
      target += 'k';
      return;
    case 0x0141:
      target += 'L';
      return;
    case 0x0142:
      target += 'l';
      return;
    case 0x00D1:
    case 0x0143:
    case 0x0145:
    case 0x0147:
      target += 'N';
      return;
    case 0x00F1:
    case 0x0144:
    case 0x0146:
    case 0x0148:
      target += 'n';
      return;
    case 0x014C:
    case 0x014E:
    case 0x0150:
      target += 'O';
      return;
    case 0x014D:
    case 0x014F:
    case 0x0151:
      target += 'o';
      return;
    case 0x0152:
      appendAsciiText(target, "OE");
      return;
    case 0x0153:
      appendAsciiText(target, "oe");
      return;
    case 0x0154:
    case 0x0156:
    case 0x0158:
      target += 'R';
      return;
    case 0x0155:
    case 0x0157:
    case 0x0159:
      target += 'r';
      return;
    case 0x015A:
    case 0x015C:
    case 0x015E:
    case 0x0160:
      target += 'S';
      return;
    case 0x015B:
    case 0x015D:
    case 0x015F:
    case 0x0161:
      target += 's';
      return;
    case 0x00DF:
      appendAsciiText(target, "ss");
      return;
    case 0x0162:
    case 0x0164:
    case 0x0166:
      target += 'T';
      return;
    case 0x0163:
    case 0x0165:
    case 0x0167:
      target += 't';
      return;
    case 0x0168:
    case 0x016A:
    case 0x016C:
    case 0x016E:
    case 0x0170:
    case 0x0172:
      target += 'U';
      return;
    case 0x0169:
    case 0x016B:
    case 0x016D:
    case 0x016F:
    case 0x0171:
    case 0x0173:
      target += 'u';
      return;
    case 0x0174:
      target += 'W';
      return;
    case 0x0175:
      target += 'w';
      return;
    case 0x00DD:
    case 0x0176:
    case 0x0178:
      target += 'Y';
      return;
    case 0x00FD:
    case 0x00FF:
    case 0x0177:
      target += 'y';
      return;
    case 0x0179:
    case 0x017B:
    case 0x017D:
      target += 'Z';
      return;
    case 0x017A:
    case 0x017C:
    case 0x017E:
      target += 'z';
      return;
    case 0x00DE:
      appendAsciiText(target, "Th");
      return;
    case 0x00FE:
      appendAsciiText(target, "th");
      return;
    case 0xFB00:
      appendAsciiText(target, "ff");
      return;
    case 0xFB01:
      appendAsciiText(target, "fi");
      return;
    case 0xFB02:
      appendAsciiText(target, "fl");
      return;
    case 0xFB03:
      appendAsciiText(target, "ffi");
      return;
    case 0xFB04:
      appendAsciiText(target, "ffl");
      return;
    case 0xFB05:
    case 0xFB06:
      appendAsciiText(target, "st");
      return;
    default:
      return;
  }
}

void appendSingleByteApproximation(String &target, uint8_t value) {
  switch (value) {
    case 0x82:
    case 0x91:
    case 0x92:
      target += '\'';
      return;
    case 0x84:
    case 0x93:
    case 0x94:
      target += '"';
      return;
    case 0x96:
    case 0x97:
      target += '-';
      return;
    case 0x85:
      appendAsciiText(target, "...");
      return;
    case 0x95:
      target += '*';
      return;
    case 0x99:
      appendAsciiText(target, "TM");
      return;
    default:
      if (value >= 0xA0) {
        appendAsciiApproximation(target, value);
      }
      return;
  }
}

String normalizeDisplayText(const String &text) {
  String normalized;
  normalized.reserve(text.length());

  size_t index = 0;
  while (index < text.length()) {
    const size_t before = index;
    uint32_t codepoint = 0;
    if (decodeUtf8Codepoint(text, index, codepoint)) {
      appendAsciiApproximation(normalized, codepoint);
      continue;
    }

    index = before + 1;
    appendSingleByteApproximation(normalized, static_cast<uint8_t>(text[before]));
  }

  String collapsed;
  collapsed.reserve(normalized.length());
  bool previousSpace = true;
  for (size_t i = 0; i < normalized.length(); ++i) {
    const char c = normalized[i];
    if (c <= ' ') {
      if (!previousSpace) {
        collapsed += ' ';
        previousSpace = true;
      }
      continue;
    }

    collapsed += c;
    previousSpace = false;
  }

  if (!collapsed.isEmpty() && collapsed[collapsed.length() - 1] == ' ') {
    collapsed.remove(collapsed.length() - 1, 1);
  }
  return collapsed;
}

void pushCleanWord(String token, std::vector<String> &words) {
  token.trim();

  if (token.length() >= 3 && static_cast<uint8_t>(token[0]) == 0xEF &&
      static_cast<uint8_t>(token[1]) == 0xBB && static_cast<uint8_t>(token[2]) == 0xBF) {
    token.remove(0, 3);
  }

  token = normalizeDisplayText(token);
  token.trim();

  while (!token.isEmpty() && isTrimmableEdgeChar(token[0])) {
    token.remove(0, 1);
  }

  while (!token.isEmpty() && isTrimmableEdgeChar(token[token.length() - 1])) {
    token.remove(token.length() - 1, 1);
  }

  bool hasAlphaNumeric = false;
  for (size_t i = 0; i < token.length(); ++i) {
    if (std::isalnum(static_cast<unsigned char>(token[i])) != 0) {
      hasAlphaNumeric = true;
      break;
    }
  }

  if (!token.isEmpty() && hasAlphaNumeric) {
    words.push_back(token);
  }
}

String stripBom(String text) {
  text.trim();
  if (text.length() >= 3 && static_cast<uint8_t>(text[0]) == 0xEF &&
      static_cast<uint8_t>(text[1]) == 0xBB && static_cast<uint8_t>(text[2]) == 0xBF) {
    text.remove(0, 3);
    text.trim();
  }
  return text;
}

bool chapterTitleFromLine(const String &line, String &title) {
  String trimmed = normalizeDisplayText(stripBom(line));
  trimmed.trim();
  if (trimmed.isEmpty() || trimmed.length() > kMaxChapterTitleChars) {
    return false;
  }

  if (trimmed.startsWith("#")) {
    size_t prefixLength = 0;
    while (prefixLength < trimmed.length() && trimmed[prefixLength] == '#') {
      ++prefixLength;
    }
    title = trimmed.substring(prefixLength);
    title.trim();
    return !title.isEmpty();
  }

  String lowered = trimmed;
  lowered.toLowerCase();
  if (prefixHasBoundary(lowered, "chapter") || prefixHasBoundary(lowered, "part") ||
      prefixHasBoundary(lowered, "book")) {
    title = trimmed;
    return true;
  }

  return false;
}

void addChapterMarker(BookContent &book, const String &title) {
  if (title.isEmpty()) {
    return;
  }

  ChapterMarker marker;
  marker.title = title;
  marker.wordIndex = book.words.size();

  if (!book.chapters.empty() && book.chapters.back().wordIndex == marker.wordIndex) {
    book.chapters.back() = marker;
    return;
  }

  book.chapters.push_back(marker);
}

void addParagraphMarker(BookContent &book) {
  const size_t wordIndex = book.words.size();
  if (!book.paragraphStarts.empty() && book.paragraphStarts.back() == wordIndex) {
    return;
  }

  book.paragraphStarts.push_back(wordIndex);
}

String directiveValue(const String &line, const char *directive) {
  String value = line.substring(std::strlen(directive));
  value.trim();
  if (!value.isEmpty() && (value[0] == ':' || value[0] == '-' || value[0] == '.')) {
    value.remove(0, 1);
    value.trim();
  }
  return normalizeDisplayText(value);
}

bool appendLineWords(const String &line, std::vector<String> &words) {
  const String normalizedLine = normalizeDisplayText(line);
  String currentWord;

  for (size_t i = 0; i < normalizedLine.length(); ++i) {
    const char c = normalizedLine[i];
    if (isWordBoundary(c)) {
      if (!currentWord.isEmpty()) {
        pushCleanWord(currentWord, words);
        currentWord = "";
        if (reachedBookWordLimit(words.size())) {
          return false;
        }
      }
      continue;
    }

    currentWord += c;
  }

  if (!currentWord.isEmpty() && !reachedBookWordLimit(words.size())) {
    pushCleanWord(currentWord, words);
  }

  return !reachedBookWordLimit(words.size());
}

bool processBookLine(const String &line, BookContent &book, bool &paragraphPending) {
  const String trimmed = stripBom(line);
  if (trimmed.isEmpty()) {
    paragraphPending = true;
    return true;
  }

  String chapterTitle;
  if (chapterTitleFromLine(line, chapterTitle)) {
    addChapterMarker(book, chapterTitle);
    paragraphPending = true;
  }

  if (paragraphPending) {
    addParagraphMarker(book);
    paragraphPending = false;
  }
  return appendLineWords(line, book.words);
}

bool processRsvpLine(const String &line, BookContent &book, bool &paragraphPending) {
  String trimmed = stripBom(line);
  if (trimmed.isEmpty()) {
    paragraphPending = true;
    return true;
  }

  if (trimmed.startsWith("@@")) {
    trimmed.remove(0, 1);
    if (paragraphPending) {
      addParagraphMarker(book);
      paragraphPending = false;
    }
    return appendLineWords(trimmed, book.words);
  }

  if (trimmed.startsWith("@")) {
    String lowered = trimmed;
    lowered.toLowerCase();
    if (prefixHasBoundary(lowered, "@para")) {
      paragraphPending = true;
      return true;
    }
    if (prefixHasBoundary(lowered, "@chapter")) {
      String title = directiveValue(trimmed, "@chapter");
      if (title.isEmpty()) {
        title = "Chapter";
      }
      addChapterMarker(book, title);
      paragraphPending = true;
      return true;
    }
    if (prefixHasBoundary(lowered, "@title")) {
      book.title = directiveValue(trimmed, "@title");
      return true;
    }
    if (prefixHasBoundary(lowered, "@author")) {
      book.author = directiveValue(trimmed, "@author");
      return true;
    }
    return true;
  }

  if (paragraphPending) {
    addParagraphMarker(book);
    paragraphPending = false;
  }
  return appendLineWords(line, book.words);
}

String readRsvpDirectiveValue(const String &path, const char *directive) {
  if (!hasRsvpExtension(path)) {
    return "";
  }

  File file = SD_MMC.open(path);
  if (!file || file.isDirectory()) {
    if (file) {
      file.close();
    }
    return "";
  }

  String line;
  while (file.available()) {
    const char c = static_cast<char>(file.read());
    if (c == '\r') {
      continue;
    }

    if (c != '\n') {
      line += c;
      if (line.length() > kMaxChapterTitleChars + 16) {
        line = "";
        break;
      }
      continue;
    }

    String trimmed = stripBom(line);
    if (trimmed.isEmpty()) {
      line = "";
      continue;
    }

    String lowered = trimmed;
    lowered.toLowerCase();
    if (prefixHasBoundary(lowered, directive)) {
      file.close();
      return directiveValue(trimmed, directive);
    }

    if (!trimmed.startsWith("@")) {
      break;
    }
    line = "";
  }

  file.close();
  return "";
}

}  // namespace

void StorageManager::setStatusCallback(StatusCallback callback, void *context) {
  statusCallback_ = callback;
  statusContext_ = context;
}

void StorageManager::notifyStatus(const char *title, const char *line1, const char *line2,
                                  int progressPercent) {
  Serial.printf("[storage-status] %d%% %s | %s | %s\n", progressPercent,
                title == nullptr ? "" : title, line1 == nullptr ? "" : line1,
                line2 == nullptr ? "" : line2);
  if (statusCallback_ != nullptr) {
    statusCallback_(statusContext_, title, line1, line2, progressPercent);
  }
}

bool StorageManager::begin() {
  mounted_ = false;
  listedOnce_ = false;
  bookPaths_.clear();

  if (!SD_MMC.setPins(BoardConfig::PIN_SD_CLK, BoardConfig::PIN_SD_CMD, BoardConfig::PIN_SD_D0)) {
    Serial.println("[storage] SD_MMC pin setup failed");
    return false;
  }

  for (int frequencyKhz : kSdFrequenciesKhz) {
    notifyStatus("SD", "Mounting card", "", 5);
    Serial.printf("[storage] Trying SD_MMC mount at %d kHz\n", frequencyKhz);
    SD_MMC.end();
    mounted_ = SD_MMC.begin(kMountPoint, true, false, frequencyKhz, 5);
    if (mounted_) {
      const uint64_t sizeMb = SD_MMC.cardSize() / (1024ULL * 1024ULL);
      Serial.printf("[storage] SD initialized (%llu MB) at %d kHz\n", sizeMb, frequencyKhz);
      notifyStatus("SD", "Scanning books", "EPUB converts on open", 10);
      refreshBookPaths();
      return true;
    }
  }

  Serial.println("[storage] SD init failed after retries");
  return false;
}

void StorageManager::end() {
  if (mounted_) {
    SD_MMC.end();
  }
  mounted_ = false;
  listedOnce_ = false;
  bookPaths_.clear();
  bookMeta_.clear();
}

void StorageManager::listBooks() {
  if (!mounted_ || listedOnce_) {
    return;
  }
  listedOnce_ = true;

  if (!booksDirectoryExists()) {
    Serial.println("[storage] /books directory not found");
    return;
  }

  Serial.println("[storage] listBooks: starting refreshBookPaths");
  if (bookPaths_.empty()) {
    refreshBookPaths();
  }
  Serial.printf("[storage] listBooks: refresh complete, %u paths\n",
                static_cast<unsigned>(bookPaths_.size()));
  if (bookPaths_.empty()) {
    Serial.println("[storage] No readable .rsvp, .txt, or .epub books found under /books");
    return;
  }

  // Oversized .rsvp files are split LAZILY — at the moment the user opens the
  // book — so boot doesn't sit on a multi-minute "Indexing" screen. See
  // App::loadBookAtIndex which calls splitOversizedRsvp() before loading if
  // the .rsvp exceeds the per-part word budget.
  Serial.printf("[storage] Library has %u books under /books\n",
                static_cast<unsigned int>(bookPaths_.size()));
}

void StorageManager::refreshBooks() {
  refreshBookPaths();
}

bool StorageManager::loadFirstBookWords(std::vector<String> &words, String *loadedPath) {
  return loadBookWords(0, words, loadedPath);
}

size_t StorageManager::bookCount() const { return bookPaths_.size(); }

String StorageManager::bookPath(size_t index) const {
  if (index >= bookPaths_.size()) {
    return "";
  }
  return bookPaths_[index];
}

bool StorageManager::wasConvertedFromEpub(size_t index) const {
  // Two signals, in order of cost: the BookMeta header read (already cached
  // from refreshBookPaths) is cheapest; falls back to checking for an .epub
  // sibling on disk for legacy books that were converted before @source was
  // a durable marker.
  if (index >= bookPaths_.size()) {
    return false;
  }
  const BookMeta &meta = bookMeta(index);
  if (meta.fromEpub) return true;
  if (epubSiblingCache_.size() != bookPaths_.size()) {
    epubSiblingCache_.assign(bookPaths_.size(), 0);
  }
  uint8_t &slot = epubSiblingCache_[index];
  if (slot == 0) {
    const String epubPath = epubSiblingPathForRsvp(bookPaths_[index]);
    File f = SD_MMC.open(epubPath);
    const bool exists = f && !f.isDirectory() && f.size() > 0;
    if (f) f.close();
    slot = exists ? 1 : 2;
  }
  return slot == 1;
}

bool StorageManager::deleteBookAtIndex(size_t index) {
  if (index >= bookPaths_.size()) return false;
  const String path = bookPaths_[index];

  // Strip the trailing .partN suffix (if any) so we can locate sibling parts.
  String base = path;
  if (base.endsWith(".rsvp")) base = base.substring(0, base.length() - 5);
  const int partAt = base.lastIndexOf(".part");
  if (partAt > 0) {
    bool allDigits = true;
    for (size_t i = partAt + 5; i < base.length(); ++i) {
      if (!isDigit(base[i])) { allDigits = false; break; }
    }
    if (allDigits) base = base.substring(0, partAt);
  }

  // Build the list of files to remove: the base .rsvp + sibling .epub +
  // any .partN.rsvp variants that share the base.
  std::vector<String> toRemove;
  toRemove.push_back(base + ".rsvp");
  toRemove.push_back(base + ".epub");
  // Probe a generous part range; ESP32 SD enumeration is cheap and we'd rather
  // catch a 12-part book than need a directory walk.
  for (uint16_t p = 1; p <= 64; ++p) {
    toRemove.push_back(base + ".part" + String(p) + ".rsvp");
  }

  size_t removed = 0;
  for (const String &candidate : toRemove) {
    if (SD_MMC.exists(candidate)) {
      if (SD_MMC.remove(candidate)) {
        ++removed;
        Serial.printf("[storage] deleted %s\n", candidate.c_str());
      } else {
        Serial.printf("[storage] delete failed: %s\n", candidate.c_str());
      }
    }
  }

  refreshBookPaths();
  return removed > 0;
}

bool StorageManager::splitOversizedRsvp(const String &path) {
  // Stay safely under the configured RAM cap. 200k words per part leaves
  // headroom under RSVP_MAX_BOOK_WORDS=250000 for system overhead.
  constexpr size_t kWordsPerPart = 200000;

  File src = SD_MMC.open(path, FILE_READ);
  if (!src || src.isDirectory()) {
    if (src) src.close();
    return false;
  }

  // Pass 1 — accumulate the original header (lines starting with @ plus any
  // blank lines before the first word) and count total body words to decide
  // whether splitting is even needed and how many parts we'll write.
  String header;
  String originalTitle;
  size_t totalWords = 0;
  size_t bodyStartPos = 0;
  bool inHeader = true;
  String line;
  while (src.available()) {
    const int c = src.read();
    if (c < 0) break;
    if (c == '\n') {
      String trimmed = line;
      trimmed.trim();
      if (inHeader) {
        if (trimmed.isEmpty() || trimmed.startsWith("@")) {
          header += line;
          header += '\n';
          if (trimmed.startsWith("@title ")) {
            originalTitle = trimmed.substring(7);
            originalTitle.trim();
          }
          bodyStartPos = src.position();
        } else {
          // First non-directive, non-blank line — body has begun.
          inHeader = false;
          if (!trimmed.isEmpty() && !trimmed.startsWith("@")) ++totalWords;
        }
      } else if (!trimmed.isEmpty() && !trimmed.startsWith("@")) {
        ++totalWords;
      }
      line = "";
    } else if (c != '\r') {
      line += static_cast<char>(c);
    }
  }
  if (!line.isEmpty()) {
    String trimmed = line;
    trimmed.trim();
    if (!trimmed.isEmpty() && !trimmed.startsWith("@")) ++totalWords;
  }

  if (totalWords <= kWordsPerPart) {
    src.close();
    return true;  // single-part file, no split necessary
  }

  const size_t numParts = (totalWords + kWordsPerPart - 1) / kWordsPerPart;
  Serial.printf("[storage] splitting %s: %u words → %u parts of %u\n",
                path.c_str(), static_cast<unsigned>(totalWords),
                static_cast<unsigned>(numParts), static_cast<unsigned>(kWordsPerPart));
  notifyStatus("Book", "Splitting", String(static_cast<unsigned>(numParts)).c_str(), 30);

  String basename = path;
  if (basename.endsWith(".rsvp")) basename = basename.substring(0, basename.length() - 5);

  if (!src.seek(bodyStartPos)) {
    src.close();
    return false;
  }

  String bufferedWord;
  bool eofReached = false;
  for (size_t partIdx = 1; partIdx <= numParts && !eofReached; ++partIdx) {
    const String partPath = basename + ".part" + String(static_cast<unsigned>(partIdx)) + ".rsvp";
    SD_MMC.remove(partPath);
    File out = SD_MMC.open(partPath, FILE_WRITE);
    if (!out) {
      src.close();
      return false;
    }

    // Re-emit the header but rewrite the @title to include the part suffix so
    // the library picker shows distinct entries per part.
    int cursor = 0;
    while (cursor < static_cast<int>(header.length())) {
      const int newline = header.indexOf('\n', cursor);
      const int lineEnd = newline < 0 ? static_cast<int>(header.length()) : newline;
      String headerLine = header.substring(cursor, lineEnd);
      if (headerLine.startsWith("@title ")) {
        String titled = originalTitle.isEmpty() ? String("Book") : originalTitle;
        out.print("@title ");
        out.print(titled);
        out.print(" (Part ");
        out.print(partIdx);
        out.print(" of ");
        out.print(numParts);
        out.println(")");
      } else {
        out.println(headerLine);
      }
      if (newline < 0) break;
      cursor = newline + 1;
    }
    out.print("@part ");
    out.print(partIdx);
    out.print(' ');
    out.println(numParts);
    out.println();

    size_t partWords = 0;
    String pending;
    auto emit = [&](const String &raw) {
      String t = raw;
      t.trim();
      if (t.startsWith("@")) {
        // Pass through chapter / paragraph directives so structure survives.
        out.println(raw);
        return false;  // not a body word
      }
      if (t.isEmpty()) {
        out.println();
        return false;
      }
      out.println(raw);
      ++partWords;
      return true;
    };

    while (src.available() && partWords < kWordsPerPart) {
      const int c = src.read();
      if (c < 0) {
        eofReached = true;
        break;
      }
      if (c == '\n') {
        emit(pending);
        pending = "";
      } else if (c != '\r') {
        pending += static_cast<char>(c);
      }
    }
    if (!pending.isEmpty() && partWords < kWordsPerPart) {
      emit(pending);
      pending = "";
    }
    out.close();
    yield();
    notifyStatus("Book", "Splitting",
                 (String("Part ") + partIdx + "/" + numParts).c_str(),
                 30 + static_cast<int>((partIdx * 60) / numParts));
  }

  src.close();
  SD_MMC.remove(path);
  notifyStatus("Book", "Split done",
               (String(numParts) + " parts").c_str(), 100);
  return true;
}

namespace {

void readRsvpHeader(const String &path, StorageManager::BookMeta &meta) {
  meta.title = "";
  meta.author = "";
  meta.words = 0;
  meta.chapters = 0;
  meta.loaded = true;

  File file = SD_MMC.open(path);
  if (!file || file.isDirectory()) {
    if (file) {
      file.close();
    }
    return;
  }

  String line;
  size_t scannedLines = 0;
  while (file.available() && scannedLines < 16) {
    const char c = static_cast<char>(file.read());
    if (c == '\r') {
      continue;
    }
    if (c != '\n') {
      line += c;
      if (line.length() > 256) {
        line = "";
      }
      continue;
    }

    String trimmed = stripBom(line);
    line = "";
    ++scannedLines;
    if (trimmed.isEmpty()) {
      continue;
    }
    if (!trimmed.startsWith("@")) {
      break;
    }

    String lowered = trimmed;
    lowered.toLowerCase();
    if (prefixHasBoundary(lowered, "@title")) {
      meta.title = directiveValue(trimmed, "@title");
    } else if (prefixHasBoundary(lowered, "@author")) {
      meta.author = directiveValue(trimmed, "@author");
    } else if (prefixHasBoundary(lowered, "@source")) {
      // The converter writes "@source <path-to-epub>" — its presence is a
      // durable marker that this .rsvp originated from an EPUB. Lets us drop
      // the .epub from the SD without losing the "filter by EPUB" behaviour.
      meta.fromEpub = true;
    } else if (prefixHasBoundary(lowered, "@stats")) {
      const String value = directiveValue(trimmed, "@stats");
      int start = 0;
      const int len = value.length();
      while (start < len) {
        int sp = value.indexOf(' ', start);
        if (sp < 0) sp = len;
        const String token = value.substring(start, sp);
        if (token.startsWith("words=")) {
          meta.words = static_cast<uint32_t>(token.substring(6).toInt());
        } else if (token.startsWith("chapters=")) {
          meta.chapters = static_cast<uint32_t>(token.substring(9).toInt());
        }
        start = sp + 1;
      }
    }
  }

  file.close();
}

}  // namespace

const StorageManager::BookMeta &StorageManager::bookMeta(size_t index) const {
  static BookMeta empty;
  if (index >= bookPaths_.size()) {
    return empty;
  }
  if (bookMeta_.size() != bookPaths_.size()) {
    bookMeta_.assign(bookPaths_.size(), BookMeta{});
  }
  BookMeta &meta = bookMeta_[index];
  if (!meta.loaded) {
    const String path = bookPaths_[index];
    if (hasRsvpExtension(path)) {
      readRsvpHeader(path, meta);
    } else {
      meta.loaded = true;
    }
  }
  return meta;
}

String StorageManager::bookDisplayName(size_t index) const {
  const String path = bookPath(index);
  if (path.isEmpty()) {
    return "";
  }
  const BookMeta &meta = bookMeta(index);
  if (!meta.title.isEmpty()) {
    return meta.title;
  }
  return normalizeDisplayText(displayNameWithoutExtension(path));
}

String StorageManager::bookAuthorName(size_t index) const {
  const String path = bookPath(index);
  if (path.isEmpty()) {
    return "";
  }
  if (hasEpubExtension(path)) {
    return epubLibraryLabel(path);
  }
  return bookMeta(index).author;
}

std::vector<String> StorageManager::listRingtoneNames() const {
  std::vector<String> names;
  if (!mounted_) return names;
  File dir = SD_MMC.open("/ringtones");
  if (!dir || !dir.isDirectory()) {
    if (dir) dir.close();
    return names;
  }
  File entry = dir.openNextFile();
  while (entry) {
    if (!entry.isDirectory()) {
      String name = String(entry.name());
      const int slash = name.lastIndexOf('/');
      if (slash >= 0) name = name.substring(slash + 1);
      String lowered = name;
      lowered.toLowerCase();
      if (lowered.endsWith(".rtttl") || lowered.endsWith(".wav")) {
        names.push_back(name);
      }
    }
    entry.close();
    entry = dir.openNextFile();
    yield();
  }
  dir.close();
  std::sort(names.begin(), names.end(), [](const String &a, const String &b) {
    String al = a;
    String bl = b;
    al.toLowerCase();
    bl.toLowerCase();
    return al < bl;
  });
  return names;
}

String StorageManager::ringtonePath(const String &name) const {
  if (name.isEmpty()) return "";
  String path = "/ringtones/";
  path += name;
  return path;
}

bool StorageManager::loadRingtone(const String &name, String &rtttlOut) const {
  rtttlOut = "";
  if (!mounted_ || name.isEmpty()) return false;
  String path = "/ringtones/";
  path += name;
  String lowered = path;
  lowered.toLowerCase();
  if (!lowered.endsWith(".rtttl")) {
    path += ".rtttl";
  }
  File f = SD_MMC.open(path);
  if (!f || f.isDirectory()) {
    if (f) f.close();
    return false;
  }
  while (f.available()) {
    rtttlOut += static_cast<char>(f.read());
    if (rtttlOut.length() > 4096) break;
  }
  f.close();
  rtttlOut.trim();
  return !rtttlOut.isEmpty();
}

bool StorageManager::bookStats(size_t index, uint32_t &words, uint32_t &chapters) const {
  const String path = bookPath(index);
  if (path.isEmpty() || !hasRsvpExtension(path)) {
    return false;
  }
  const BookMeta &meta = bookMeta(index);
  if (meta.words == 0 && meta.chapters == 0) {
    return false;
  }
  if (meta.words > 0) {
    words = meta.words;
  }
  if (meta.chapters > 0) {
    chapters = meta.chapters;
  }
  return true;
}

bool StorageManager::ensureEpubConverted(const String &epubPath, String &rsvpPath) {
  rsvpPath = rsvpCachePathForEpub(epubPath);

  if (!RSVP_ON_DEVICE_EPUB_CONVERSION) {
    Serial.printf("[storage] EPUB conversion disabled at build time: %s\n", epubPath.c_str());
    notifyStatus("EPUB unsupported", displayNameForPath(epubPath).c_str(),
                 "Build flag is disabled", 100);
    return false;
  }

  if (!fileExistsAndHasBytes(epubPath)) {
    Serial.printf("[storage] EPUB source missing or empty: %s\n", epubPath.c_str());
    notifyStatus("Preparing book", displayNameForPath(epubPath).c_str(), "EPUB missing", 100);
    return false;
  }

  if (fileExistsAndHasBytes(rsvpPath) && EpubConverter::isCurrentCache(rsvpPath)) {
    Serial.printf("[storage] EPUB cache hit: %s -> %s\n", epubPath.c_str(), rsvpPath.c_str());
    return true;
  }

  if (fileExistsAndHasBytes(rsvpPath)) {
    Serial.printf("[storage] EPUB cache stale after converter update: %s\n", rsvpPath.c_str());
  }

  File epubFile = SD_MMC.open(epubPath);
  const size_t epubBytes = epubFile ? static_cast<size_t>(epubFile.size()) : 0;
  if (epubFile) {
    epubFile.close();
  }

  Serial.printf("[storage] Preparing EPUB conversion: source=%s output=%s size=%lu bytes\n",
                epubPath.c_str(), rsvpPath.c_str(), static_cast<unsigned long>(epubBytes));
  logHeapSnapshot("before EPUB conversion");
  notifyStatus("Preparing book", displayNameForPath(epubPath).c_str(), "Converting EPUB", 0);

  EpubConverter::Options options;
  options.maxWords = kMaxBookWords;
  options.progressCallback = handleEpubProgress;
  EpubProgressContext progressContext;
  progressContext.statusCallback = statusCallback_;
  progressContext.statusContext = statusContext_;
  progressContext.title = "Preparing book";
  progressContext.label = displayNameForPath(epubPath);
  options.progressContext = &progressContext;

  const uint32_t startedMs = millis();
  const bool converted = EpubConverter::convertIfNeeded(epubPath, rsvpPath, options);
  const uint32_t elapsedMs = millis() - startedMs;
  logHeapSnapshot("after EPUB conversion");

  if (!converted || !fileExistsAndHasBytes(rsvpPath)) {
    Serial.printf("[storage] EPUB conversion failed after %lu ms: %s\n",
                  static_cast<unsigned long>(elapsedMs), epubPath.c_str());
    notifyStatus("Preparing book", "EPUB conversion failed", "Check serial monitor", 100);
    return false;
  }

  Serial.printf("[storage] EPUB conversion ready after %lu ms: %s\n",
                static_cast<unsigned long>(elapsedMs), rsvpPath.c_str());
  notifyStatus("Preparing book", displayNameForPath(rsvpPath).c_str(), "Conversion complete",
               100);
  return true;
}

bool StorageManager::loadBookContent(size_t index, BookContent &book, String *loadedPath,
                                     size_t *loadedIndex) {
  book.clear();

  if (!mounted_) {
    Serial.println("[storage] SD not mounted, cannot load book");
    return false;
  }

  if (!booksDirectoryExists()) {
    Serial.println("[storage] /books directory not found");
    return false;
  }

  if (bookPaths_.empty()) {
    refreshBookPaths();
  }
  if (bookPaths_.empty()) {
    Serial.println("[storage] No readable .rsvp, .txt, or .epub books found under /books");
    return false;
  }

  if (index >= bookPaths_.size()) {
    Serial.printf("[storage] Book index %u out of range\n", static_cast<unsigned int>(index));
    return false;
  }

  for (size_t offset = 0; offset < bookPaths_.size(); ++offset) {
    const size_t candidateIndex = (index + offset) % bookPaths_.size();
    String path = bookPaths_[candidateIndex];
    size_t parsedIndex = candidateIndex;

    if (hasEpubExtension(path)) {
      String rsvpPath;
      if (!ensureEpubConverted(path, rsvpPath)) {
        return false;
      }

      refreshBookPaths();
      const int convertedIndex = pathIndexIn(bookPaths_, rsvpPath);
      if (convertedIndex < 0) {
        Serial.printf("[storage] Converted RSVP not found in refreshed library: %s\n",
                      rsvpPath.c_str());
        return false;
      }

      path = rsvpPath;
      parsedIndex = static_cast<size_t>(convertedIndex);
    }

    File entry = SD_MMC.open(path);
    if (!entry || entry.isDirectory()) {
      if (entry) {
        entry.close();
      }
      continue;
    }

    String progressLabel = bookMeta(parsedIndex).title;
    if (progressLabel.isEmpty()) {
      progressLabel = normalizeDisplayText(displayNameWithoutExtension(path));
    }
    if (parseFile(entry, book, hasRsvpExtension(path), progressLabel)) {
      if (book.title.isEmpty()) {
        book.title = normalizeDisplayText(displayNameWithoutExtension(path));
      }
      Serial.printf("[storage] Loaded %u words and %u chapters from %s\n",
                    static_cast<unsigned int>(book.words.size()),
                    static_cast<unsigned int>(book.chapters.size()), path.c_str());
      notifyStatus("Loading", progressLabel.c_str(), "Ready", 100);
      if (loadedPath != nullptr) {
        *loadedPath = path;
      }
      if (loadedIndex != nullptr) {
        *loadedIndex = parsedIndex;
      }
      entry.close();
      return true;
    }

    book.clear();
    entry.close();
  }

  Serial.println("[storage] No readable book files found under /books");
  return false;
}

bool StorageManager::loadBookWords(size_t index, std::vector<String> &words, String *loadedPath,
                                   size_t *loadedIndex) {
  BookContent book;
  if (!loadBookContent(index, book, loadedPath, loadedIndex)) {
    words.clear();
    return false;
  }

  words = std::move(book.words);
  return true;
}

void StorageManager::refreshBookPaths() {
  if (!mounted_) {
    bookPaths_.clear();
    bookMeta_.clear();
    return;
  }

  Serial.println("[storage] refreshBookPaths: collectBookPaths begin");
  const uint32_t collectStartMs = millis();
  notifyStatus("SD", "Reading library", "", 50);
  bookPaths_ = collectBookPaths();
  Serial.printf("[storage] refreshBookPaths: collectBookPaths done in %lu ms, %u paths\n",
                static_cast<unsigned long>(millis() - collectStartMs),
                static_cast<unsigned>(bookPaths_.size()));
  bookMeta_.assign(bookPaths_.size(), BookMeta{});
  epubSiblingCache_.assign(bookPaths_.size(), 0);  // invalidate sibling cache

  const size_t count = bookPaths_.size();
  Serial.printf("[storage] refreshBookPaths: header read loop begin (%u files)\n",
                static_cast<unsigned>(count));
  const uint32_t headerLoopStartMs = millis();
  uint32_t lastStatusMs = millis();
  for (size_t i = 0; i < count; ++i) {
    if (hasRsvpExtension(bookPaths_[i])) {
      readRsvpHeader(bookPaths_[i], bookMeta_[i]);
    } else {
      bookMeta_[i].loaded = true;
    }
    // Sibling-EPUB cache is filled in a single batch *after* this loop using
    // one directory enumeration — much cheaper than a per-book SD open here.
    if ((i & 0x07) == 0) {
      yield();
    }
    if ((i & 0x1F) == 0x1F) {
      Serial.printf("[storage]   headers %u/%u\n", static_cast<unsigned>(i + 1),
                    static_cast<unsigned>(count));
    }
    const uint32_t now = millis();
    if (now - lastStatusMs >= 200) {
      const int percent = static_cast<int>(((i + 1) * 50ULL) / std::max<size_t>(1, count)) + 50;
      notifyStatus("SD", "Indexing books", "", std::min(percent, 99));
      lastStatusMs = now;
    }
  }
  Serial.printf("[storage] refreshBookPaths: header loop done in %lu ms\n",
                static_cast<unsigned long>(millis() - headerLoopStartMs));

  // Walk /books/ once and harvest the set of .epub basenames present, then
  // fan that out to populate the sibling-EPUB cache in O(N) hash lookups
  // instead of O(N) SD stats. This is what makes Library/Resume-from/Author
  // pickers feel snappy on first open after boot.
  {
    Serial.println("[storage] refreshBookPaths: epub stem walk begin");
    const uint32_t walkStartMs = millis();
    std::vector<String> epubStems;
    File dir = SD_MMC.open(kBooksPath);
    if (dir && dir.isDirectory()) {
      File entry = dir.openNextFile();
      while (entry) {
        if (!entry.isDirectory()) {
          String entryPath = normalizeBookPath(String(entry.name()));
          if (hasEpubExtension(entryPath)) {
            // Strip the trailing .epub
            int extDot = entryPath.lastIndexOf('.');
            if (extDot > 0) entryPath = entryPath.substring(0, extDot);
            entryPath.toLowerCase();
            epubStems.push_back(entryPath);
          }
        }
        entry.close();
        entry = dir.openNextFile();
        yield();
      }
      dir.close();
    }
    std::sort(epubStems.begin(), epubStems.end());
    auto stemForRsvp = [](const String &rsvpPath) {
      String s = rsvpPath;
      const int dot = s.lastIndexOf('.');
      if (dot > 0) s = s.substring(0, dot);
      const int partAt = s.lastIndexOf(".part");
      if (partAt > 0) {
        bool allDigits = partAt + 5 < static_cast<int>(s.length());
        for (int i = partAt + 5; i < static_cast<int>(s.length()) && allDigits; ++i) {
          if (!isDigit(s[i])) allDigits = false;
        }
        if (allDigits) s = s.substring(0, partAt);
      }
      s.toLowerCase();
      return s;
    };
    for (size_t i = 0; i < count; ++i) {
      if (!hasRsvpExtension(bookPaths_[i])) {
        epubSiblingCache_[i] = 2;
        continue;
      }
      const String stem = stemForRsvp(bookPaths_[i]);
      const bool found = std::binary_search(epubStems.begin(), epubStems.end(), stem);
      epubSiblingCache_[i] = found ? 1 : 2;
    }
    Serial.printf("[storage] refreshBookPaths: epub stem walk done in %lu ms (%u stems)\n",
                  static_cast<unsigned long>(millis() - walkStartMs),
                  static_cast<unsigned>(epubStems.size()));
  }

  size_t rsvpCount = 0;
  size_t textCount = 0;
  size_t pendingEpubCount = 0;
  for (const String &path : bookPaths_) {
    if (hasRsvpExtension(path)) {
      ++rsvpCount;
    } else if (hasTextExtension(path)) {
      ++textCount;
    } else if (hasEpubExtension(path)) {
      ++pendingEpubCount;
    }
  }

  Serial.printf("[storage] Library scan: %u books (%u rsvp, %u txt, %u pending epub)\n",
                static_cast<unsigned int>(bookPaths_.size()),
                static_cast<unsigned int>(rsvpCount), static_cast<unsigned int>(textCount),
                static_cast<unsigned int>(pendingEpubCount));
}

bool StorageManager::parseFile(File &file, BookContent &book, bool rsvpFormat,
                               const String &progressLabel) {
  book.clear();
  String line;
  bool paragraphPending = true;
  size_t bytesRead = 0;
  const size_t totalBytes = static_cast<size_t>(file.size());
  const bool emitProgress = totalBytes > 0 && !progressLabel.isEmpty();
  uint32_t lastProgressMs = millis();
  if (emitProgress) {
    notifyStatus("Loading", progressLabel.c_str(), "Reading from SD", 0);
  }

  constexpr size_t kReadBufferBytes = 4096;
  uint8_t buffer[kReadBufferBytes];
  bool stopRequested = false;
  while (!stopRequested && file.available()) {
    const int got = file.read(buffer, sizeof(buffer));
    if (got <= 0) {
      break;
    }
    for (int i = 0; i < got; ++i) {
      const char c = static_cast<char>(buffer[i]);
      if (c == '\r') {
        continue;
      }
      if (c == '\n') {
        const bool keepReading =
            rsvpFormat ? processRsvpLine(line, book, paragraphPending)
                       : processBookLine(line, book, paragraphPending);
        if (!keepReading) {
          if (hasBookWordLimit()) {
            Serial.printf("[storage] Reached %lu word limit, truncating book\n",
                          static_cast<unsigned long>(kMaxBookWords));
            String truncMsg = String("Truncated at ") +
                              String(static_cast<unsigned long>(kMaxBookWords / 1000)) +
                              "K words";
            notifyStatus("Book", "Too long for RAM", truncMsg.c_str(), 99);
          }
          stopRequested = true;
          break;
        }
        line = "";
        continue;
      }
      line += c;
    }

    bytesRead += static_cast<size_t>(got);
    yield();
    if (emitProgress) {
      const uint32_t now = millis();
      if (now - lastProgressMs >= 200) {
        const int percent =
            static_cast<int>((static_cast<uint64_t>(bytesRead) * 100ULL) / totalBytes);
        notifyStatus("Loading", progressLabel.c_str(), "Reading from SD",
                     std::min(percent, 99));
        lastProgressMs = now;
      }
    }
  }

  if (!line.isEmpty() && !reachedBookWordLimit(book.words.size())) {
    if (rsvpFormat) {
      processRsvpLine(line, book, paragraphPending);
    } else {
      processBookLine(line, book, paragraphPending);
    }
  }

  if (!book.words.empty() && book.paragraphStarts.empty()) {
    book.paragraphStarts.push_back(0);
  }

  return !book.words.empty();
}
