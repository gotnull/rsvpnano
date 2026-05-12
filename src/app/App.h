#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <WiFiClient.h>
#include <vector>

#include "app/AppState.h"
#include "app/event/EventBus.h"
#include "app/event/Events.h"
#include "app/scene/LegacyScene.h"
#include "app/scene/SceneContext.h"
#include "app/scene/SceneManager.h"
#include "app/scene/ScreensaverScene.h"
#include "audio/AudioManager.h"
#include "audio/ModPlayer.h"
#include "demos/Plasma.h"
#include "demos/Rasterbars.h"
#include "demos/ShadeBobs.h"
#include "demos/SineScroller.h"
#include "demos/Starfield.h"
#include "demos/Pupul.h"
#include "demos/OldschoolIntro.h"
#include "demos/UnlimitedBobs.h"
#include "demos/Vectorball.h"
#include "display/DisplayManager.h"
#include "input/ButtonHandler.h"
#include "input/TouchHandler.h"
#include "network/BookDownloadManager.h"
#include "network/NotificationsManager.h"
#include "network/OtaManager.h"
#include "reader/ReadingLoop.h"
#include "screensaver/Screensaver.h"
#include "storage/StorageManager.h"
#include "usb/UsbMassStorageManager.h"

class App {
 public:
  App();

  void begin();
  void update(uint32_t nowMs);

 private:
  struct PausedTouchSession {
    bool active = false;
    uint16_t startX = 0;
    uint16_t startY = 0;
    uint16_t lastX = 0;
    uint16_t lastY = 0;
    uint32_t startMs = 0;
    uint32_t lastMs = 0;
    size_t startWordIndex = 0;
    int scrubStepsApplied = 0;
  };

  enum class TouchIntent {
    None,
    PlayHold,
    Scrub,
    Wpm,
  };

  enum class MenuScreen {
    Main,
    SettingsHome,
    SettingsDisplay,
    SettingsPacing,
    SettingsReadingSounds,
    NetworkPicker,
    TypographyTuning,
    AuthorPicker,
    BookPicker,
    ChapterPicker,
    RestartConfirm,
    TonePicker,
    RemoteBookPicker,
    BookDeleteConfirm,
    DemoPicker,
    ModulesPicker,
    ModulesFavorites,
    ModuleFavoriteConfirm,
  };

  // ----------------------------------------------------------------------
  // Reusable tab-group infrastructure for tabbed pickers. Any picker that
  // wants horizontal-swipe tab switching declares a static TabGroup
  // containing one TabDescriptor per tab (label + the MenuScreen it maps to
  // + the opener method). The shared helpers below render a tab-strip row
  // at the top of the list, advance/wrap on swipe, and tell move-selection
  // to skip the (non-selectable) strip row. To add tabs to another section,
  // declare a new TabGroup constant and ensure tabGroupFor() returns it for
  // each member MenuScreen.
  struct TabDescriptor {
    const char *label;
    MenuScreen screen;
    void (App::*opener)();
  };
  struct TabGroup {
    const TabDescriptor *tabs;
    size_t tabCount;
  };
  static const TabDescriptor kHomeTabsData[];
  static const TabGroup kHomeTabGroup;
  static const TabDescriptor kSettingsTabsData[];
  static const TabGroup kSettingsTabGroup;
  static const TabDescriptor kBooksTabsData[];
  static const TabGroup kBooksTabGroup;
  // Bundled tab args every render call passes through to DisplayManager.
  // If the current screen isn't in any TabGroup the labels vector is empty
  // and activeIdx/underline* are -1 — both renderMenuWithTabs and
  // renderLibrary treat that as "no tab band, render normally".
  struct TabRenderArgs {
    std::vector<String> labels;
    int activeIdx = -1;
    int underlineX = -1;
    int underlineW = -1;
  };
  TabRenderArgs tabRenderArgsForCurrentScreen() const;

  // Resolve the tab group a menu screen belongs to, or nullptr if the
  // screen is single-tab (no tab strip rendered).
  const TabGroup *tabGroupFor(MenuScreen screen) const;
  int tabIndexInGroup(const TabGroup &group, MenuScreen screen) const;
  // Build the row-0 tab-strip label ("[Modules]   Favorites").
  String buildTabStripLabel(const TabGroup &group, int activeIdx) const;
  // Advance the active tab by `direction` (wraps) and re-open the new
  // screen. Caller is responsible for resetting selectedIndex if needed.
  void cycleTabs(int direction);
  // True when the current menu screen has a tab group — used by
  // moveMenuSelection() to skip row 0 (the tab strip).
  bool currentScreenHasTabs() const { return tabGroupFor(menuScreen_) != nullptr; }

  enum class DemoKind : uint8_t {
    None = 0,
    Rasterbars,
    Starfield,
    SineScroller,
    Plasma,
    ShadeBobs,
    Vectorball,
    UnlimitedBobs,
    Pupul,
    OldschoolIntro,
  };

  void setState(AppState nextState, uint32_t nowMs);
  void updateState(uint32_t nowMs);
  void updateReader(uint32_t nowMs);
  void updateWpmFeedback(uint32_t nowMs);
  void maybeSaveReadingPosition(uint32_t nowMs);
  void handleBootButton(uint32_t nowMs);
  void handlePowerButton(uint32_t nowMs);
  void toggleMenuFromPowerButton(uint32_t nowMs);
  void openMainMenu(uint32_t nowMs);
  void cycleBrightness();
  void cycleThemeMode(uint32_t nowMs);
  void toggleDisplayFlip(uint32_t nowMs);
  void togglePhantomWords(uint32_t nowMs);
  void cycleReaderFontSize(uint32_t nowMs);
  void applyDisplayPreferences(uint32_t nowMs, bool rerender = true);
  void applyTypographySettings(uint32_t nowMs, bool rerender = true);
  uint8_t currentBrightnessPercent() const;
  bool updateBatteryStatus(uint32_t nowMs, bool force = false);
  void handleTouch(uint32_t nowMs);
  void applyPausedTouchGesture(const TouchEvent &event, uint32_t nowMs);
  int scrubStepsForDrag(int deltaX) const;
  void applyScrubTarget(int targetSteps);
  void applyMenuTouchGesture(const TouchEvent &event, uint32_t nowMs);
  void moveMenuSelection(int direction);
  void selectMenuItem(uint32_t nowMs);
  // Generic "navigate to parent" for menu screens that have a Back/No row.
  // Returns false if the current screen has no parent (e.g. Main). Used by
  // the left-swipe shortcut so the user never has to scroll to the Back row.
  bool goBack(uint32_t nowMs);
  // Per-Settings-tab openers used by the Settings TabGroup. Each one sets
  // menuScreen_, resets the settings selection index, rebuilds items, and
  // re-renders. Mirrors the pattern openModulesPicker / openModulesFavorites.
  void openSettingsHome();
  // Opener for the "Read" home tab — sets menuScreen_=Main and re-renders.
  // Matches the TabDescriptor opener signature (void(App::*)()).
  void openMainMenuTab();
  void openSettingsDisplay();
  void openSettingsPacing();
  void openSettingsReadingSounds();
  // No-arg wrapper for RemoteBookPicker so the opener matches the
  // TabDescriptor signature (which is void(App::*)()).
  void openRemoteBookPickerTab();
  void openSettings();
  void selectSettingsItem(uint32_t nowMs);
  void openTypographyTuning();
  void selectTypographyTuningItem(uint32_t nowMs);
  void cycleTypographyPreviewSample(int direction);
  void rebuildSettingsMenuItems();
  void applyPacingSettings();
  String pacingLevelLabel(uint8_t levelIndex) const;
  String themeModeLabel() const;
  String phantomWordsLabel() const;
  String readerFontSizeLabel() const;
  String typographyTuningLabel() const;
  String typographyTuningValueLabel() const;
  void openAuthorPicker();
  void selectAuthorPickerItem(uint32_t nowMs);
  void openBookPicker();
  void openBookPickerForAuthor(const String &author);
  void selectBookPickerItem(uint32_t nowMs);
  void openChapterPicker();
  void selectChapterPickerItem(uint32_t nowMs);
  void openRestartConfirm();
  void selectRestartConfirmItem(uint32_t nowMs);
  void enterUsbTransfer(uint32_t nowMs);
  void updateUsbTransfer(uint32_t nowMs);
  void exitUsbTransfer(uint32_t nowMs);
  void runOtaUpdate(uint32_t nowMs);
  void toggleNotificationsEnabled(uint32_t nowMs);
  void openTonePicker();
  void selectTonePickerItem(uint32_t nowMs);
  void renderTonePicker();
  void openRemoteBookPicker(uint32_t nowMs);
  void selectRemoteBookPickerItem(uint32_t nowMs);
  void renderRemoteBookPicker();
  void openNetworkPicker();
  void selectNetworkPickerItem(uint32_t nowMs);
  void renderNetworkPicker();
  // Reorders ota_.config().networks so the user-selected default SSID is at
  // index 0, then re-pushes into NotificationsManager + BookDownloadManager.
  void applyDefaultNetwork();
  void openBookDeleteConfirm(size_t bookIndex);
  void selectBookDeleteConfirmItem(uint32_t nowMs);
  void renderBookDeleteConfirm();
  void openDemoPicker();
  void selectDemoPickerItem(uint32_t nowMs);
  void renderDemoPicker();
  void openModulesPicker();
  void selectModulesPickerItem(uint32_t nowMs);
  void renderModulesPicker();
  // Favorites submenu — long-press on a module in the picker prompts to add;
  // long-press inside Favorites prompts to remove. Persisted as a comma-
  // separated basename list in Preferences.
  void openModulesFavorites();
  void renderModulesFavorites();
  void selectModulesFavoritesItem(uint32_t nowMs);
  void openModuleFavoriteConfirm(const String &name, bool addMode);
  void renderModuleFavoriteConfirm();
  void selectModuleFavoriteConfirmItem(uint32_t nowMs);
  bool isModuleFavorite(const String &name) const;
  void loadModuleFavorites();
  void saveModuleFavorites();
  // Lazy SD-walk cache so re-opening the picker (or swiping between tabs)
  // doesn't pay for another /mods/ scan. Invalidated only when /mods/ might
  // have changed: SD remount, USB-transfer end, starter-pack download.
  const std::vector<String> &cachedModuleList();
  void invalidateModulesListCache();
  // Re-render the current menu screen during a tab-slide animation. Driven
  // from App::update() while tabAnimActive_ is true.
  void tickTabAnimation(uint32_t nowMs);
  // Polled each loop tick — if the background storage worker just finished,
  // re-render whichever Books picker the user is waiting on so the new
  // library data shows immediately.
  void handleStorageScanCompletion();
  // Non-blocking status display + deferred action. Replaces every long
  // `display_.renderStatus(...); delay(N); doSomething();` block on the UI
  // thread. Caller calls displayTransientStatus() and returns; the main
  // loop runs tickTransientStatus() each iteration and fires the deferred
  // action once `nowMs >= transientStatusUntilMs_`. Per the
  // "main loop is sacred" rule — no synchronous delay() in user-facing flows.
  enum class TransientStatusAction : uint8_t {
    None = 0,
    ReturnToMain,
    ReRenderSettings,
    ReturnToRemoteBookPicker,
    ReRenderModulesPicker,
    RetryStorageMountAfterOtaFail,
  };
  void displayTransientStatus(const String &title, const String &line1, const String &line2,
                              uint32_t durationMs, TransientStatusAction action);
  void tickTransientStatus(uint32_t nowMs);
  // Compute the underline X-position for the current tab/animation state.
  // `slotW` is the per-tab slot width on screen.
  int currentTabUnderlineX(int slotW, int slotCount) const;
  int currentTabUnderlineW(int slotW) const;
  // Fullscreen tracker player view. Entered from picker tap and exited by
  // touch. Re-renders frame on a fixed cadence so the channel bars animate.
  void enterModulePlayback(const String &path, uint32_t nowMs);
  void exitModulePlayback(uint32_t nowMs);
  void renderModulePlayerFrame(uint32_t nowMs);
  void renderModulePlayerBars(uint32_t nowMs, const ModPlayer::NowPlaying &np);
  void renderModulePlayerPattern(uint32_t nowMs, const ModPlayer::NowPlaying &np);
  void toggleModulePlayerView(uint32_t nowMs);
  // Picks a random track from /mods/ and starts background playback.
  // Used by the Demo-music shuffle hook on entering Screensaver/DemoPlaying.
  bool startRandomModule(uint32_t nowMs);
  // Event-bus subscribers. Static trampolines unpack `userdata` (set to
  // `this` at subscribe time) and call the member function.
  void onScreensaverExited(const Event& ev);
  static void onScreensaverExitedTrampoline(const Event& ev, void* userdata);
  // Fetches the starter MOD pack (5 tracks) from the gotnull/rsvpnano
  // mods-pack-v1 GitHub release into /mods/. Skips files already present.
  // Renders progress to the display; blocks until done.
  void downloadModStarterPack();
  void enterDemoPlayback(DemoKind kind, uint32_t nowMs);
  void exitDemoPlayback(uint32_t nowMs);
  void renderDemoFrame(uint32_t nowMs);
  void enterCameraStream(uint32_t nowMs);
  void exitCameraStream(uint32_t nowMs);
  void updateCameraStream(uint32_t nowMs);
  bool pollCameraExitTouch(uint32_t nowMs);
  bool ensureCameraBuffers(size_t jpegBytes, int frameWidth, int frameHeight);
  bool fetchCameraSnapshot(uint32_t nowMs);
  bool openCameraMjpegStream();
  void closeCameraMjpegStream();
  bool readCameraMjpegFrame(uint32_t nowMs);
  bool readCameraStreamLine(String &line, uint32_t timeoutMs);
  bool readCameraStreamBytes(uint8_t *dst, size_t length, uint32_t timeoutMs);
  // Byte-state-machine multipart header parser. Reads from the stream
  // client into a fixed stack buffer, looks for the `\r\n\r\n` end-of-
  // headers terminator, and scans the buffered headers for Content-Length.
  // No String allocations per frame — replaces the previous per-line String
  // approach that allocated dozens of times per second at 10 fps.
  bool parseCameraMjpegHeaders(int &contentLength, uint32_t deadlineMs);
  // Skips up to maxToDrop frames already buffered in the TCP receive
  // window so the next decoded/rendered frame is the most-recent one the
  // server produced. Each skip reads the next frame's headers and
  // discards `Content-Length` body bytes. Returns immediately if no data
  // is buffered — never blocks on the network.
  void drainStaleCameraFrames(int maxToDrop);
  bool decodeCameraSnapshot(const uint8_t *data, size_t length);
  String cameraSnapshotUrl() const;
  String cameraStreamUrl() const;
  void cycleNotificationVolume(uint32_t nowMs);
  void pollNotifications(uint32_t nowMs);
  void showNotificationBanner(uint32_t nowMs, const String &title, const String &body);
  void renderNotificationBanner();
  void playNotificationTone(uint32_t maxDurationMs = 0);
  String currentNotificationToneLabel() const;
  void enterPowerOff(uint32_t nowMs);
  void enterSleep(uint32_t nowMs);
  void wakeFromSleep();
  bool restoreSavedBook(uint32_t nowMs);
  bool prepareSavedBookMeta();
  bool ensureCurrentBookLoaded(uint32_t nowMs);
  void saveReadingPosition(bool force = false);
  bool loadBookAtIndex(size_t index, uint32_t nowMs, bool allowLegacyPositionFallback = false);
  String bookPositionKey(const String &bookPath) const;
  String bookWordCountKey(const String &bookPath) const;
  String bookRecentKey(const String &bookPath) const;
  uint32_t nextRecentSequence();
  // Records `dtUs` against the named stage; updates the heartbeat's
  // worst-stage tracker and prints a one-line warning when the stage
  // exceeds `warnUs`. Used to localise screensaver lockups to the
  // specific call (touch, battery, render, flash) that wedged the loop.
  void trackStage(const char *name, uint32_t dtUs, uint32_t warnUs);
  uint32_t bookRecentSequence(const String &bookPath);
  uint32_t bookRecentSequenceByIndex(size_t bookIndex);
  void markBookRecent(const String &bookPath);
  uint32_t savedWordIndexForBook(const String &bookPath, bool allowLegacyFallback = false);
  bool bookProgressPercent(size_t bookIndex, uint8_t &percent);

  struct BookProgressInfo {
    bool valid = false;
    bool hasPosition = false;
    uint32_t recentSequence = 0;
    uint32_t savedWordIndex = 0;
    uint32_t savedWordCount = 0;
  };
  void ensureBookProgressInfoSize();
  const BookProgressInfo &progressInfoFor(size_t bookIndex);
  void invalidateBookProgress(size_t bookIndex);
  std::vector<BookProgressInfo> bookProgressInfo_;
  int findBookIndexByPath(const String &path) const;
  void renderMenu();
  void renderMainMenu();
  void renderSettings();
  void renderTypographyTuning();
  void renderAuthorPicker();
  void renderBookPicker();
  void renderChapterPicker();
  void renderRestartConfirm();
  DisplayManager::LibraryItem libraryItemForBook(size_t bookIndex);
  String chapterMenuLabel(size_t chapterIndex) const;
  String currentChapterLabel() const;
  uint8_t readingProgressPercent() const;
  void renderReaderWord();
  void renderContextPreview();
  void renderWpmFeedback(uint32_t nowMs);
  size_t phantomBeforeCharTarget() const;
  size_t phantomAfterCharTarget() const;
  String collectPhantomBeforeText(size_t currentIndex, size_t charTarget) const;
  String collectPhantomAfterText(size_t currentIndex, size_t charTarget) const;
  String phantomBeforeText() const;
  String phantomAfterText() const;
  bool isParagraphStart(size_t wordIndex) const;
  size_t paragraphStartAtOrBefore(size_t wordIndex) const;
  size_t contextPreviewAnchorIndex(size_t currentIndex) const;
  void invalidateContextPreviewWindow();
  void renderStorageStatus(const char *title, const char *line1, const char *line2,
                           int progressPercent);
  static void handleStorageStatus(void *context, const char *title, const char *line1,
                                  const char *line2, int progressPercent);
  const char *stateName(AppState state) const;
  const char *touchPhaseName(TouchPhase phase) const;

  AppState state_ = AppState::Booting;
  DisplayManager display_;
  ReadingLoop reader_;
  ButtonHandler button_;
  ButtonHandler powerButton_;
  TouchHandler touch_;
  StorageManager storage_;
  UsbMassStorageManager usbTransfer_;
  OtaManager ota_;
  NotificationsManager notifications_;
  AudioManager audio_;
  ModPlayer modPlayer_;
  BookDownloadManager bookDownloader_;
  std::vector<BookDownloadManager::RemoteBook> remoteBooks_;
  size_t remoteBookSelectedIndex_ = 0;
  std::vector<DisplayManager::LibraryItem> remoteBookMenuItems_;
  std::vector<DisplayManager::LibraryItem> networkMenuItems_;
  size_t networkSelectedIndex_ = 0;
  String preferredWifiSsid_;
  size_t demoSelectedIndex_ = 0;
  size_t modulesSelectedIndex_ = 0;
  std::vector<String> modulesMenuItems_;
  // Cached basename list from storage_.listModuleNames(). Refreshed lazily
  // on first picker open after a mount/transfer event; tab swipes between
  // Modules ↔ Favorites reuse it instead of paying for an SD walk every time.
  std::vector<String> modulesListCache_;
  bool modulesListCacheValid_ = false;
  // Tab-slide animation state for tabbed pickers.
  bool tabAnimActive_ = false;
  int tabAnimFromIdx_ = 0;
  int tabAnimToIdx_ = 0;
  uint32_t tabAnimStartMs_ = 0;
  // Persisted module favorites — sorted basenames such as "slumberjack.xm".
  std::vector<String> moduleFavorites_;
  // Rendered Favorites picker (Modules ▸ Favorites). Same shape as the main
  // modules picker (Back + items).
  std::vector<String> modulesFavoritesMenuItems_;
  size_t modulesFavoritesSelectedIndex_ = 0;
  // Confirm-dialog state — same pattern as bookDeleteSelectedIndex_/etc.
  size_t moduleFavoriteConfirmSelectedIndex_ = 0;
  String moduleFavoriteTargetName_;
  // addMode_ = true  → "Favorite this module?"
  // addMode_ = false → "Remove favorite?"
  bool moduleFavoriteAddMode_ = true;
  // One-shot debounce so a long-press doesn't open the confirm twice in a row.
  // Reset on the next touch-Start (same lifecycle as bookLongPressFired_).
  bool moduleLongPressFired_ = false;
  // Transient status / deferred action state. transientStatusUntilMs_ == 0
  // means "no transient status active". Set by displayTransientStatus(),
  // cleared by tickTransientStatus() once the deadline is reached.
  uint32_t transientStatusUntilMs_ = 0;
  TransientStatusAction transientStatusAction_ = TransientStatusAction::None;
  uint32_t modulePlayerLastRenderMs_ = 0;
  // Smoothed channel-bar heights for the now-playing UI. Decayed each frame
  // and bumped to the current channel_info volume so taps + cut-offs read
  // as bars dropping, not jumping. The peak array holds a "ghost" marker
  // that snaps up with the body but lingers for ~0.3 s before decaying, the
  // way a hardware VU meter floats a peak indicator.
  uint8_t modulePlayerBarLevels_[32] = {0};
  uint8_t modulePlayerPeakLevels_[32] = {0};
  uint8_t modulePlayerPeakHoldFrames_[32] = {0};
  // Toggleable now-playing view. 0 = VU bars, 1 = MilkyTracker-style pattern
  // grid. Persists via kPrefModulePlayerView; user toggles with a vertical
  // swipe inside ModulePlaying (tap still exits to the picker).
  enum class ModuleView : uint8_t { Bars = 0, Pattern = 1 };
  ModuleView modulePlayerView_ = ModuleView::Bars;
  // Touch-gesture bookkeeping for the module player so we can tell a tap
  // (= exit) from a vertical swipe (= toggle view).
  bool modulePlayerTouchActive_ = false;
  bool modulePlayerTouchHandled_ = false;
  uint16_t modulePlayerTouchStartX_ = 0;
  uint16_t modulePlayerTouchStartY_ = 0;
  // Scratch buffer for pattern-view cells; sized for kModulePatternRows ×
  // kModulePatternMaxChans (filled by ModPlayer::getPatternWindow each frame).
  std::vector<ModulePatternCell> modulePlayerPatternCells_;
  // Demo-music mode: 0 = Off, 1 = Shuffle (random track on Screensaver/Demo
  // entry), 2 = Picked (always replay the last picked track). Persisted as
  // kPrefDemoMusic.
  uint8_t demoMusicMode_ = 1;
  String demoMusicPickedTrack_;
  Preferences preferences_;
  PausedTouchSession pausedTouch_;
  TouchIntent pausedTouchIntent_ = TouchIntent::None;

  uint32_t bootStartedMs_ = 0;
  uint32_t lastStateLogMs_ = 0;
  // Set when a touch dismisses the screensaver or a demo; subsequent
  // Move/End events on the same finger-down are dropped so the underlying
  // menu doesn't see the dismiss tap as one of its own.
  bool dismissTouchPending_ = false;
  // Slow-stage tracking — reset every heartbeat (~1.5 s) so the printed
  // "worst" reflects the most recent interval, not the lifetime peak.
  uint32_t loopWorstUs_ = 0;
  const char *loopWorstStage_ = "(none)";
  uint32_t heapMinSeen_ = UINT32_MAX;
  uint32_t firmwareValidateAtMs_ = 0;
  bool firmwarePendingVerify_ = false;
  uint32_t lastMenuRefreshMs_ = 0;
  uint32_t lastReaderRefreshMs_ = 0;
  uint32_t nextNotificationPollMs_ = 0;
  uint32_t notificationBannerUntilMs_ = 0;
  String notificationBannerTitle_;
  String notificationBannerBody_;
  uint32_t wpmFeedbackUntilMs_ = 0;
  uint32_t lastProgressSaveMs_ = 0;
  uint32_t lastBatterySampleMs_ = 0;
  size_t lastSavedWordIndex_ = static_cast<size_t>(-1);
  size_t contextPreviewStartIndex_ = 0;
  size_t currentBookIndex_ = 0;
  size_t menuSelectedIndex_ = 0;
  size_t settingsSelectedIndex_ = 0;
  size_t bookPickerSelectedIndex_ = 0;
  size_t authorPickerSelectedIndex_ = 0;
  size_t chapterPickerSelectedIndex_ = 0;
  size_t restartConfirmSelectedIndex_ = 0;
  uint8_t brightnessLevelIndex_ = 4;
  uint8_t readerFontSizeIndex_ = 0;
  uint8_t pacingLongWordLevelIndex_ = 2;
  uint8_t pacingComplexWordLevelIndex_ = 2;
  uint8_t pacingPunctuationLevelIndex_ = 2;
  size_t typographyTuningSelectedIndex_ = 1;
  size_t typographyPreviewSampleIndex_ = 0;
  MenuScreen menuScreen_ = MenuScreen::Main;
  std::vector<String> settingsMenuItems_;
  std::vector<DisplayManager::LibraryItem> bookMenuItems_;
  std::vector<size_t> bookPickerBookIndices_;
  std::vector<DisplayManager::LibraryItem> authorMenuItems_;
  std::vector<String> authorPickerNames_;
  String activeAuthorFilter_;
  bool recentOnlyFilter_ = false;
  std::vector<char> bookPickerLetterAnchors_;
  std::vector<size_t> bookPickerLetterTargets_;
  std::vector<char> authorPickerLetterAnchors_;
  std::vector<size_t> authorPickerLetterTargets_;
  std::vector<DisplayManager::LibraryItem> toneMenuItems_;
  std::vector<String> tonePickerNames_;
  size_t tonePickerSelectedIndex_ = 0;
  bool letterScrubActive_ = false;
  int letterScrubFocusIdx_ = -1;
  // Index into bookPickerBookIndices_ that the long-press delete prompt is
  // about to remove. Reset whenever the prompt is dismissed.
  size_t bookDeleteIndex_ = 0;
  String bookDeleteTitle_;
  size_t bookDeleteSelectedIndex_ = 0;
  bool bookLongPressFired_ = false;
  std::vector<String> chapterMenuItems_;
  std::vector<ChapterMarker> chapterMarkers_;
  std::vector<size_t> paragraphStarts_;
  String currentBookPath_;
  String currentBookTitle_;
  String batteryLabel_;
  bool touchInitialized_ = false;
  bool touchPlayHeld_ = false;
  bool bootButtonReleasedSinceBoot_ = false;
  bool bootButtonLongPressHandled_ = false;
  bool powerButtonReleasedSinceBoot_ = false;
  bool powerButtonLongPressHandled_ = false;
  bool powerOffStarted_ = false;
  bool contextViewVisible_ = false;
  bool contextPreviewWindowValid_ = false;
  bool wpmFeedbackVisible_ = false;
  bool usingStorageBook_ = false;
  bool bookMetaOnly_ = false;
  bool phantomWordsEnabled_ = true;
  bool darkMode_ = true;
  bool nightMode_ = false;
  bool displayFlipped_ = true;
  bool notificationsEnabled_ = true;
  bool soundEnabled_ = true;             // master audio switch — gates all playback
  bool crtShaderEnabled_ = false;        // CRT scanline post-process for demos
  bool chapterChimeEnabled_ = false;     // play a tone when crossing chapter boundary
  bool paragraphChimeEnabled_ = false;   // play a tone when crossing paragraph boundary
  bool pageChimeEnabled_ = false;        // play a tone every kPageWordCount words
  uint8_t notificationVolume_ = 60;
  uint8_t autoPowerOffIndex_ = 0;        // 0=Off; cycles through preset minute values
  uint8_t screensaverIndex_ = 1;         // default 1m; 0=Off, others = minute threshold
  uint32_t lastActivityMs_ = 0;
  // Earliest millis() at which we'll try to auto-remount the SD if it's
  // currently unmounted. 0 disables retry until the first mount attempt
  // sets it.
  uint32_t nextSdRemountAtMs_ = 0;
  AppState screensaverPreviousState_ = AppState::Paused;
  Screensaver screensaver_;
  // Scene architecture — Milestone 1. Order matters: SceneContext holds
  // references to display_/screensaver_/modPlayer_/events_, all of which
  // are declared above this block, so the C++ member-initializer order
  // (declaration order) is correct.
  EventBus events_;
  LegacyScene legacyScene_;
  ScreensaverScene screensaverScene_;
  SceneContext sceneCtx_;
  SceneManager sceneManager_;
  // All four demos live as stack-resident members so swapping is allocation-free.
  // Total state cost ~6 KB; cheaper than the screensaver. DemoKind::None means
  // we are not currently in DemoPlaying state.
  DemoKind currentDemo_ = DemoKind::None;
  Rasterbars demoRasterbars_;
  Starfield demoStarfield_;
  SineScroller demoSineScroller_;
  Plasma demoPlasma_;
  ShadeBobs demoShadeBobs_;
  Vectorball demoVectorball_;
  UnlimitedBobs demoUnlimitedBobs_;
  Pupul demoPupul_;
  OldschoolIntro demoOldschoolIntro_;
  uint8_t *cameraJpegBuffer_ = nullptr;
  size_t cameraJpegCapacity_ = 0;
  uint16_t *cameraFrameBuffer_ = nullptr;
  size_t cameraFrameCapacityPixels_ = 0;
  int cameraFrameWidth_ = 0;
  int cameraFrameHeight_ = 0;
  uint32_t cameraLastFrameMs_ = 0;
  uint32_t cameraLastStatsMs_ = 0;
  uint32_t cameraFramesOk_ = 0;
  uint32_t cameraFramesFailed_ = 0;
  uint32_t cameraFrameSeq_ = 0;
  uint32_t cameraIgnoreTouchUntilMs_ = 0;
  bool cameraSuppressOpeningTouch_ = false;
  bool cameraExitRequested_ = false;
  bool cameraWifiConnected_ = false;
  bool cameraStreamConnected_ = false;
  WiFiClient cameraStreamClient_;
  String notificationTone_;
  std::vector<String> ringtoneNames_;
  DisplayManager::TypographyConfig typographyConfig_;
};
