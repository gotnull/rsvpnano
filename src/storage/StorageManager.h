#pragma once

#include <Arduino.h>
#include <FS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <atomic>
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
    bool fromEpub = false;  // @source directive seen in the .rsvp header
  };

  void setStatusCallback(StatusCallback callback, void *context);
  bool begin();
  void end();
  // True if SD_MMC is currently mounted. Used by App for periodic auto-remount
  // attempts when the initial boot mount failed.
  bool isMounted() const { return mounted_; }
  // Main-loop entry point. Spawns the SD scan on a dedicated FreeRTOS task
  // and returns immediately so the UI thread is never blocked by SD I/O.
  // Idempotent — subsequent calls while a scan is in progress (or after one
  // has completed) are no-ops. See requestListBooksAsync() / isReady() /
  // consumeScanCompleted() for status polling.
  void listBooks();
  // Force a fresh scan (used after USB-transfer end / SD remount where the
  // file set may have changed). Drops the listedOnce_ guard so the worker
  // re-walks /books even if it ran before.
  void requestRescanAsync();
  void refreshBooks();
  bool isScanInProgress() const { return scanInProgress_.load(); }
  // True once the worker has populated bookPaths_ at least once. While
  // false, all bookCount() / bookPath() / bookDisplayName() / bookStats()
  // accessors return safe defaults so the UI never reads a half-built list.
  bool isReady() const { return scanReady_.load() && !scanInProgress_.load(); }
  // Atomic test-and-clear, called from the main loop's update tick so it
  // can re-render whichever picker was waiting on the scan.
  bool consumeScanCompleted() {
    bool expected = true;
    return scanCompletedOneShot_.compare_exchange_strong(expected, false);
  }
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
  // Tracker modules under /mods/ — bare filenames including extension.
  // Filters to .mod/.xm/.s3m/.it. Sorted case-insensitively.
  std::vector<String> listModuleNames() const;
  // Full /mods/<name> path for ModPlayer::playFile().
  String modulePath(const String &name) const;
  bool loadBookWords(size_t index, std::vector<String> &words, String *loadedPath = nullptr,
                     size_t *loadedIndex = nullptr);
  // Converts an EPUB on the SD card into a sibling .rsvp file (idempotent).
  // Used by the in-app book downloader after fetching a remote .epub so the
  // new title appears in the library on the next listBooks() pass.
  bool ensureEpubConverted(const String &epubPath, String &rsvpPath);
  // Returns true if there's a sibling .epub file next to this book's .rsvp on
  // the SD, indicating the book originated from an EPUB conversion.
  bool wasConvertedFromEpub(size_t index) const;
  // If the .rsvp at `path` exceeds the per-part word budget, splits it into
  // sibling files like `<base>.part1.rsvp` / `<base>.part2.rsvp`, with each
  // part carrying its own header that names it "Title (Part N of M)". The
  // original single file is removed on success. No-op (returns true) if the
  // book already fits in one part.
  bool splitOversizedRsvp(const String &path);
  // Removes a book from the SD: deletes the .rsvp at this index, its sibling
  // .epub (if present), and — when this book is part of a multi-part split —
  // all the other .partN.rsvp / single-base companions. Refreshes book paths
  // on success so the index list reflects the deletion immediately.
  bool deleteBookAtIndex(size_t index);

 private:
  bool parseFile(File &file, BookContent &book, bool rsvpFormat,
                 const String &progressLabel = "");
  void refreshBookPaths();
  void notifyStatus(const char *title, const char *line1 = "", const char *line2 = "",
                    int progressPercent = -1);
  const BookMeta &bookMeta(size_t index) const;
  // Internal: spawn the scan task if one isn't running. Caller decides
  // whether to reset listedOnce_ first (force re-scan vs. one-shot).
  void spawnScanTaskIfIdle();
  static void scanTaskTrampoline(void *arg);
  void scanTaskRun();

  bool mounted_ = false;
  bool listedOnce_ = false;
  // Worker-task lifecycle. scanInProgress_ is set true the moment the
  // worker is spawned and false right before it self-deletes. scanReady_
  // flips true after the FIRST successful scan and stays true thereafter
  // (so accessors keep returning real data while a subsequent re-scan
  // runs). scanCompletedOneShot_ is the main-loop wakeup signal —
  // consumeScanCompleted() flips it back to false.
  std::atomic<bool> scanInProgress_{false};
  std::atomic<bool> scanReady_{false};
  std::atomic<bool> scanCompletedOneShot_{false};
  TaskHandle_t scanTaskHandle_ = nullptr;
  StatusCallback statusCallback_ = nullptr;
  void *statusContext_ = nullptr;
  std::vector<String> bookPaths_;
  mutable std::vector<BookMeta> bookMeta_;
  // 0 = unknown, 1 = has sibling .epub, 2 = no sibling. Computed lazily on
  // first ask and reused so the picker filter doesn't re-stat every book on
  // each open. Reset whenever bookPaths_ changes.
  mutable std::vector<uint8_t> epubSiblingCache_;
};
