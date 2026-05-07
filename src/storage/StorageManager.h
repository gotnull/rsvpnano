#pragma once

#include <Arduino.h>
#include <FS.h>
#include <vector>

#include "reader/BookContent.h"

class StorageManager {
 public:
  using StatusCallback = void (*)(void *context, const char *title, const char *line1,
                                  const char *line2, int progressPercent);

  struct BookMeta {
    String title;
    String author;
    uint32_t words = 0;
    uint32_t chapters = 0;
    bool loaded = false;
  };

  void setStatusCallback(StatusCallback callback, void *context);
  bool begin();
  void end();
  void listBooks();
  void refreshBooks();
  bool loadFirstBookWords(std::vector<String> &words, String *loadedPath = nullptr);
  bool loadBookContent(size_t index, BookContent &book, String *loadedPath = nullptr,
                       size_t *loadedIndex = nullptr);
  size_t bookCount() const;
  String bookPath(size_t index) const;
  String bookDisplayName(size_t index) const;
  String bookAuthorName(size_t index) const;
  bool bookStats(size_t index, uint32_t &words, uint32_t &chapters) const;
  // Each entry is the bare filename (with extension) under /ringtones/.
  std::vector<String> listRingtoneNames() const;
  // Reads a .rtttl file body into rtttlOut. Pass the filename with or without
  // extension; ".rtttl" is appended if absent.
  bool loadRingtone(const String &name, String &rtttlOut) const;
  // Returns the full /ringtones/ path for the file (for WAV streaming etc).
  String ringtonePath(const String &name) const;
  bool loadBookWords(size_t index, std::vector<String> &words, String *loadedPath = nullptr,
                     size_t *loadedIndex = nullptr);
  // Converts an EPUB on the SD card into a sibling .rsvp file (idempotent).
  // Used by the in-app book downloader after fetching a remote .epub so the
  // new title appears in the library on the next listBooks() pass.
  bool ensureEpubConverted(const String &epubPath, String &rsvpPath);
  // Returns true if there's a sibling .epub file next to this book's .rsvp on
  // the SD, indicating the book originated from an EPUB conversion.
  bool wasConvertedFromEpub(size_t index) const;

 private:
  bool parseFile(File &file, BookContent &book, bool rsvpFormat,
                 const String &progressLabel = "");
  void refreshBookPaths();
  void notifyStatus(const char *title, const char *line1 = "", const char *line2 = "",
                    int progressPercent = -1);
  const BookMeta &bookMeta(size_t index) const;

  bool mounted_ = false;
  bool listedOnce_ = false;
  StatusCallback statusCallback_ = nullptr;
  void *statusContext_ = nullptr;
  std::vector<String> bookPaths_;
  mutable std::vector<BookMeta> bookMeta_;
};
