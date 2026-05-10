#include "app/App.h"

#include <SD_MMC.h>

#include <esp_sleep.h>
#include <esp_log.h>
#include <algorithm>
#include <cstdio>
#include <iterator>
#include <utility>
#include <vector>

#include "board/BoardConfig.h"

#include <esp_ota_ops.h>

#ifndef RSVP_USB_TRANSFER_ENABLED
#define RSVP_USB_TRANSFER_ENABLED 0
#endif

#ifndef RSVP_USB_TRANSFER_AUTO_START
#define RSVP_USB_TRANSFER_AUTO_START 0
#endif

static const char *kAppTag = "app";
constexpr uint32_t kBootSplashMs = 750;
constexpr uint32_t kWpmFeedbackMs = 900;
constexpr uint32_t kPowerOffHoldMs = 1600;
constexpr uint32_t kPowerOffReleaseWaitMs = 4000;
constexpr uint32_t kBatterySampleIntervalMs = 180000;
constexpr uint32_t kTouchPlayHoldMs = 180;
constexpr uint32_t kThemeToggleHoldMs = 900;
constexpr uint16_t kSwipeThresholdPx = 28;
constexpr uint16_t kAxisBiasPx = 8;
constexpr uint16_t kTapSlopPx = 12;
constexpr uint16_t kScrubStepPx = 22;
constexpr int kMaxScrubStepsPerGesture = 96;
constexpr size_t kContextPreviewWindowWords = 288;
constexpr size_t kContextPreviewAnchorLeadWords = 112;
constexpr size_t kContextPreviewMaxParagraphSnapWords = 48;
constexpr uint32_t kProgressSaveIntervalMs = 15000;
constexpr uint32_t kUsbTransferExitHoldMs = 1200;
constexpr uint8_t kBrightnessLevels[] = {40, 55, 70, 85, 100};
constexpr uint8_t kNightBrightnessLevels[] = {35, 40, 45, 50, 55};
constexpr size_t kBrightnessLevelCount = sizeof(kBrightnessLevels) / sizeof(kBrightnessLevels[0]);

namespace
{

  void computeLetterAnchors(const std::vector<DisplayManager::LibraryItem> &items,
                            size_t firstSelectableIndex,
                            std::vector<char> &letters,
                            std::vector<size_t> &targets)
  {
    letters.clear();
    targets.clear();
    char lastLetter = 0;
    for (size_t i = firstSelectableIndex; i < items.size(); ++i)
    {
      const String &t = items[i].title;
      if (t.isEmpty())
        continue;
      char c = t[0];
      if (c >= 'a' && c <= 'z')
        c = static_cast<char>(c - 'a' + 'A');
      if (!(c >= 'A' && c <= 'Z'))
      {
        c = '#';
      }
      if (c == lastLetter)
        continue;
      letters.push_back(c);
      targets.push_back(i);
      lastLetter = c;
    }
  }

  enum MenuItem : size_t
  {
    MenuResume,
    MenuResumeFrom,
    MenuChapters,
    MenuChangeBook,
    MenuDownloadBooks,
    MenuSettings,
    MenuRestart,
#if RSVP_USB_TRANSFER_ENABLED
    MenuUsbTransfer,
#endif
    MenuOtaUpdate,
    MenuPowerOff,
    MenuItemCount,
  };

  constexpr const char *kMenuItems[] = {
      "Resume",
      "Resume from",
      "Chapters",
      "Library",
      "Download books",
      "Settings",
      "Reboot",
#if RSVP_USB_TRANSFER_ENABLED
      "USB transfer",
#endif
      "OTA update",
      "Power off",
  };

  enum SettingsItem : size_t
  {
    SettingsBack,
    SettingsDisplay,
    SettingsTypography,
    SettingsWordPacing,
    SettingsBrightness,
    SettingsTheme,
    SettingsPhantomWords,
    SettingsFontSize,
    SettingsLongWords,
    SettingsComplexWords,
    SettingsPunctuation,
    SettingsReset,
    SettingsItemCount,
  };

  enum TypographyTuningItem : size_t
  {
    TypographyTuningBack,
    TypographyTuningTracking,
    TypographyTuningAnchor,
    TypographyTuningGuideWidth,
    TypographyTuningGuideGap,
    TypographyTuningReset,
    TypographyTuningItemCount,
  };

  enum RestartConfirmItem : size_t
  {
    RestartConfirmNo,
    RestartConfirmYes,
    RestartConfirmItemCount,
  };

  enum BookDeleteOption : size_t {
    BookDeleteCancel = 0,
    BookDeleteYes = 1,
    BookDeleteOptionCount,
  };
  constexpr size_t kBookDeleteConfirmHeaderRows = 1;

  constexpr const char *kRestartConfirmItems[] = {
      "Reboot now?",
      "No",
      "Yes, reboot",
  };

  constexpr size_t kRestartConfirmHeaderRows = 1;
  constexpr size_t kSettingsBackIndex = 0;
  constexpr size_t kSettingsHomeDisplayIndex = 1;
  constexpr size_t kSettingsHomeTypographyIndex = 2;
  constexpr size_t kSettingsHomePacingIndex = 3;
  constexpr size_t kSettingsHomeSoundIndex = 4;
  constexpr size_t kSettingsHomeNotificationsIndex = 5;
  constexpr size_t kSettingsHomeToneIndex = 6;
  constexpr size_t kSettingsHomeVolumeIndex = 7;
  constexpr size_t kSettingsHomeReadingSoundsIndex = 8;
  constexpr size_t kSettingsHomeRemountSdIndex = 9;
  constexpr size_t kSettingsHomeNetworkIndex = 10;
  constexpr size_t kSettingsHomeScreensaverIndex = 11;
  constexpr size_t kSettingsHomeDemosIndex = 12;
  // Demo picker layout: Back at 0, then one row per demo. Order must match
  // the dispatch in selectDemoPickerItem.
  constexpr size_t kDemoPickerBackIndex = 0;
  constexpr size_t kDemoPickerRasterbarsIndex = 1;
  constexpr size_t kDemoPickerStarfieldIndex = 2;
  constexpr size_t kDemoPickerSineScrollerIndex = 3;
  constexpr size_t kDemoPickerPlasmaIndex = 4;
  constexpr size_t kDemoPickerShadeBobsIndex = 5;
  constexpr size_t kDemoPickerVectorballIndex = 6;
  constexpr size_t kDemoPickerItemCount = 7;
  constexpr size_t kSettingsReadingSoundsChapterIndex = 1;
  constexpr size_t kSettingsReadingSoundsParagraphIndex = 2;
  constexpr size_t kSettingsReadingSoundsPageIndex = 3;
  constexpr size_t kSettingsDisplayThemeIndex = 1;
  constexpr size_t kSettingsDisplayBrightnessIndex = 2;
  constexpr size_t kSettingsDisplayPhantomWordsIndex = 3;
  constexpr size_t kSettingsDisplayFontSizeIndex = 4;
  constexpr size_t kSettingsDisplayTypographyIndex = 5;
  constexpr size_t kSettingsDisplayFlipIndex = 6;
  constexpr size_t kSettingsDisplayAutoPowerOffIndex = 7;
  constexpr size_t kSettingsPacingLongWordsIndex = 1;
  constexpr size_t kSettingsPacingComplexityIndex = 2;
  constexpr size_t kSettingsPacingPunctuationIndex = 3;
  constexpr size_t kSettingsPacingResetIndex = 4;

  constexpr size_t kBookPickerBackIndex = 0;
  constexpr size_t kChapterPickerBackIndex = 0;
  constexpr size_t kChapterPickerFallbackIndex = 1;
  constexpr size_t kAuthorPickerBackIndex = 0;
  constexpr size_t kAuthorPickerAllBooksIndex = 1;
  constexpr size_t kAuthorPickerFirstAuthorIndex = 2;
  constexpr const char *kAuthorAllBooksLabel = "All books";
  constexpr const char *kAuthorUnknownLabel = "Unknown";
  constexpr const char *kPrefsNamespace = "rsvp";
  constexpr const char *kPrefBookPath = "book";
  constexpr const char *kPrefAppState = "appst";
  constexpr const char *kPrefLegacyWordIndex = "word";
  constexpr const char *kPrefWpm = "wpm";
  constexpr const char *kPrefBrightness = "bright";
  constexpr const char *kPrefDarkMode = "dark";
  constexpr const char *kPrefDisplayFlip = "flip";
  constexpr const char *kPrefNotifications = "notif";
  constexpr const char *kPrefNotifTone = "ntone";
  constexpr const char *kPrefNotifVolume = "nvol";
  constexpr const char *kPrefNotifLastTs = "nlast";
  constexpr const char *kPrefSoundEnabled = "snden";
  constexpr const char *kPrefChapterChime = "chchm";
  constexpr const char *kPrefParagraphChime = "pgchm";
  constexpr const char *kPrefPageChime = "pgnchm";
  constexpr const char *kPrefAutoPowerOff = "apoff";
  constexpr const char *kPrefPreferredWifi = "wifipref";
  constexpr const char *kPrefScreensaver = "scrnsvr";
  // Index 0 disables; others are minutes of inactivity before the dots
  // screensaver kicks in. Same cycle pattern as auto-off.
  constexpr uint16_t kScreensaverMinutes[] = {0, 1, 5, 15, 30};
  constexpr size_t kScreensaverOptionCount =
      sizeof(kScreensaverMinutes) / sizeof(kScreensaverMinutes[0]);
  // Index 0 disables auto-off; rest are minute-thresholds. Bumping the cycle
  // is one-tap, and Off is in the cycle so the user can disable from the
  // Settings menu without a separate toggle.
  constexpr uint16_t kAutoPowerOffMinutes[] = {0, 5, 15, 30, 60, 120};
  constexpr size_t kAutoPowerOffOptionCount =
      sizeof(kAutoPowerOffMinutes) / sizeof(kAutoPowerOffMinutes[0]);
  // RSVP has no inherent page concept; this is the average paperback page
  // size in words and lets us emit a "page" chime at predictable intervals.
  constexpr size_t kPageWordCount = 250;
  constexpr uint32_t kNotificationPollIntervalMs = 5UL * 60UL * 1000UL;
  constexpr uint32_t kNotificationBannerVisibleMs = 6000;
  constexpr uint32_t kNotificationFirstPollDelayMs = 30UL * 1000UL;
  constexpr uint8_t kNotificationVolumeStep = 5;
  constexpr const char *kBuiltinNokiaRtttl =
      "Nokia:d=4,o=5,b=180:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a";
  constexpr const char *kPrefNightMode = "night";
  constexpr const char *kPrefPhantomWords = "phantom_on";
  constexpr const char *kPrefReaderFontSize = "font_size";
  constexpr const char *kPrefPacingLong = "pace_len";
  constexpr const char *kPrefPacingComplex = "pace_cpx";
  constexpr const char *kPrefPacingPunctuation = "pace_pnc";
  constexpr const char *kPrefTypographyTracking = "type_trk";
  constexpr const char *kPrefTypographyAnchor = "type_anc";
  constexpr const char *kPrefTypographyGuideWidth = "type_wid";
  constexpr const char *kPrefTypographyGuideGap = "type_gap";
  constexpr const char *kPrefRecentSeq = "seq";
  constexpr const char *kReaderFontSizeLabels[] = {"Large", "Medium", "Small"};
  constexpr size_t kReaderFontSizeCount =
      sizeof(kReaderFontSizeLabels) / sizeof(kReaderFontSizeLabels[0]);
  constexpr size_t kPhantomBeforeCharTargets[] = {64, 96, 144};
  constexpr size_t kPhantomAfterCharTargets[] = {96, 144, 208};
  constexpr uint32_t kNoSavedWordIndex = 0xFFFFFFFFUL;
  constexpr uint8_t kPacingScalePercents[] = {60, 80, 100, 125, 150};
  constexpr const char *kPacingScaleLabels[] = {"VLow", "Low", "Bal", "High", "Max"};
  constexpr size_t kPacingLevelCount = sizeof(kPacingScalePercents) / sizeof(kPacingScalePercents[0]);
  constexpr uint8_t kDefaultPacingLevelIndex = 2;
  constexpr int8_t kTypographyTrackingMin = -2;
  constexpr int8_t kTypographyTrackingMax = 3;
  constexpr uint8_t kTypographyAnchorMin = 30;
  constexpr uint8_t kTypographyAnchorMax = 40;
  constexpr uint8_t kTypographyGuideWidthMin = 12;
  constexpr uint8_t kTypographyGuideWidthMax = 30;
  constexpr uint8_t kTypographyGuideWidthStep = 2;
  constexpr uint8_t kTypographyGuideGapMin = 2;
  constexpr uint8_t kTypographyGuideGapMax = 8;
  constexpr const char *kTypographyPreviewWords[] = {
      "minimum",
      "encyclopaedia",
      "state-of-the-art",
      "HTTP/2",
      "well-known",
      "rhythms",
      "illumination",
      "WAVEFORM",
      "I",
  };
  constexpr size_t kTypographyPreviewWordCount =
      sizeof(kTypographyPreviewWords) / sizeof(kTypographyPreviewWords[0]);

  void logApp(const char *message)
  {
    ESP_LOGI(kAppTag, "%s", message);
    Serial.printf("[app] %s\n", message);
  }

  String displayNameForPath(const String &path)
  {
    const int separator = path.lastIndexOf('/');
    String name = separator >= 0 ? path.substring(separator + 1) : path;
    String lowered = name;
    lowered.toLowerCase();
    if (lowered.endsWith(".txt"))
    {
      name.remove(name.length() - 4);
    }
    if (lowered.endsWith(".rsvp"))
    {
      name.remove(name.length() - 5);
    }
    return name;
  }

  uint32_t hashBookPath(const String &path)
  {
    uint32_t hash = 2166136261UL;
    for (size_t i = 0; i < path.length(); ++i)
    {
      hash ^= static_cast<uint8_t>(path[i]);
      hash *= 16777619UL;
    }
    return hash;
  }

  int clampIntSetting(int value, int minValue, int maxValue)
  {
    return std::max(minValue, std::min(maxValue, value));
  }

  int nextCyclicSetting(int value, int minValue, int maxValue, int step = 1)
  {
    step = std::max(1, step);
    const int normalized = clampIntSetting(value, minValue, maxValue);
    int next = normalized + step;
    if (next > maxValue)
    {
      next = minValue;
    }
    return next;
  }

  DisplayManager::TypographyConfig defaultTypographyConfig()
  {
    return DisplayManager::TypographyConfig();
  }

} // namespace

App::App() : button_(BoardConfig::PIN_BOOT_BUTTON), powerButton_(BoardConfig::PIN_PWR_BUTTON) {}

void App::begin()
{
#ifdef RSVP_BUILD_TAG
  Serial.printf("[boot] rsvpnano build %s\n", RSVP_BUILD_TAG);
#else
  Serial.println("[boot] rsvpnano build (no tag)");
#endif
  BoardConfig::begin();
  button_.begin();
  powerButton_.begin();
  bootButtonReleasedSinceBoot_ = !button_.isHeld();
  bootButtonLongPressHandled_ = false;
  powerButtonReleasedSinceBoot_ = !powerButton_.isHeld();
  powerButtonLongPressHandled_ = false;
  storage_.setStatusCallback(&App::handleStorageStatus, this);
  ota_.setStatusCallback(&App::handleStorageStatus, this);
  notifications_.setStatusCallback(&App::handleStorageStatus, this);
  bookDownloader_.setStatusCallback(&App::handleStorageStatus, this);
  preferences_.begin(kPrefsNamespace, false);
  brightnessLevelIndex_ = preferences_.getUChar(kPrefBrightness, brightnessLevelIndex_);
  if (brightnessLevelIndex_ >= kBrightnessLevelCount)
  {
    brightnessLevelIndex_ = kBrightnessLevelCount - 1;
  }
  phantomWordsEnabled_ = preferences_.getBool(kPrefPhantomWords, phantomWordsEnabled_);
  readerFontSizeIndex_ = preferences_.getUChar(kPrefReaderFontSize, readerFontSizeIndex_);
  if (readerFontSizeIndex_ >= kReaderFontSizeCount)
  {
    readerFontSizeIndex_ = 0;
  }
  pacingLongWordLevelIndex_ =
      preferences_.getUChar(kPrefPacingLong, pacingLongWordLevelIndex_);
  if (pacingLongWordLevelIndex_ >= kPacingLevelCount)
  {
    pacingLongWordLevelIndex_ = kDefaultPacingLevelIndex;
  }
  pacingComplexWordLevelIndex_ =
      preferences_.getUChar(kPrefPacingComplex, pacingComplexWordLevelIndex_);
  if (pacingComplexWordLevelIndex_ >= kPacingLevelCount)
  {
    pacingComplexWordLevelIndex_ = kDefaultPacingLevelIndex;
  }
  pacingPunctuationLevelIndex_ =
      preferences_.getUChar(kPrefPacingPunctuation, pacingPunctuationLevelIndex_);
  if (pacingPunctuationLevelIndex_ >= kPacingLevelCount)
  {
    pacingPunctuationLevelIndex_ = kDefaultPacingLevelIndex;
  }
  typographyConfig_ = defaultTypographyConfig();
  typographyConfig_.trackingPx = static_cast<int8_t>(clampIntSetting(
      preferences_.getChar(kPrefTypographyTracking, typographyConfig_.trackingPx),
      kTypographyTrackingMin, kTypographyTrackingMax));
  typographyConfig_.anchorPercent = static_cast<uint8_t>(clampIntSetting(
      preferences_.getUChar(kPrefTypographyAnchor, typographyConfig_.anchorPercent),
      kTypographyAnchorMin, kTypographyAnchorMax));
  typographyConfig_.guideHalfWidth = static_cast<uint8_t>(clampIntSetting(
      preferences_.getUChar(kPrefTypographyGuideWidth, typographyConfig_.guideHalfWidth),
      kTypographyGuideWidthMin, kTypographyGuideWidthMax));
  typographyConfig_.guideGap = static_cast<uint8_t>(clampIntSetting(
      preferences_.getUChar(kPrefTypographyGuideGap, typographyConfig_.guideGap),
      kTypographyGuideGapMin, kTypographyGuideGapMax));
  darkMode_ = preferences_.getBool(kPrefDarkMode, darkMode_);
  nightMode_ = preferences_.getBool(kPrefNightMode, nightMode_);
  displayFlipped_ = preferences_.getBool(kPrefDisplayFlip, BoardConfig::UI_ROTATED_180);
  display_.setUiRotated(displayFlipped_);
  touch_.setUiRotated(displayFlipped_);
  notificationsEnabled_ = preferences_.getBool(kPrefNotifications, true);
  notifications_.setEnabled(notificationsEnabled_);
  notificationVolume_ = preferences_.getUChar(kPrefNotifVolume, notificationVolume_);
  if (notificationVolume_ > 100)
    notificationVolume_ = 100;
  // Rescue users who got stuck on 0% from the old wrap-to-zero cycle. Muting
  // lives on the Notifications toggle now, not on volume.
  if (notificationVolume_ == 0)
  {
    notificationVolume_ = 60;
    preferences_.putUChar(kPrefNotifVolume, notificationVolume_);
  }
  audio_.setVolumePercent(notificationVolume_);
  notificationTone_ = preferences_.getString(kPrefNotifTone, "");
  notifications_.setLastSeenTs(preferences_.getUInt(kPrefNotifLastTs, 0));
  soundEnabled_ = preferences_.getBool(kPrefSoundEnabled, soundEnabled_);
  chapterChimeEnabled_ = preferences_.getBool(kPrefChapterChime, chapterChimeEnabled_);
  paragraphChimeEnabled_ = preferences_.getBool(kPrefParagraphChime, paragraphChimeEnabled_);
  pageChimeEnabled_ = preferences_.getBool(kPrefPageChime, pageChimeEnabled_);
  autoPowerOffIndex_ = preferences_.getUChar(kPrefAutoPowerOff, autoPowerOffIndex_);
  if (autoPowerOffIndex_ >= kAutoPowerOffOptionCount) autoPowerOffIndex_ = 0;
  preferredWifiSsid_ = preferences_.getString(kPrefPreferredWifi, "");
  screensaverIndex_ = preferences_.getUChar(kPrefScreensaver, screensaverIndex_);
  if (screensaverIndex_ >= kScreensaverOptionCount) screensaverIndex_ = 0;
  lastActivityMs_ = millis();
  applyDisplayPreferences(0, false);
  applyTypographySettings(0, false);
  applyPacingSettings();
  bootStartedMs_ = millis();
  lastStateLogMs_ = bootStartedMs_;

  logApp("Initializing hardware modules");
  const bool displayReady = display_.begin();
  updateBatteryStatus(bootStartedMs_, true);

  if (displayReady)
  {
    display_.renderCenteredWord("READY");
    logApp("Display init ok");
  }
  else
  {
    ESP_LOGE(kAppTag, "Display init failed");
    Serial.println("[app] Display init failed");
  }

  touchInitialized_ = touch_.begin();

#if RSVP_USB_TRANSFER_ENABLED && RSVP_USB_TRANSFER_AUTO_START
  state_ = AppState::Booting;
  Serial.println("[app] USB transfer auto-start active");
  enterUsbTransfer(millis());
  return;
#endif

  display_.renderProgress("SD", "Loading books", "Use SD converter for EPUB", 0);
  const bool storageReady = storage_.begin();
  storage_.listBooks();
  if (!storageReady) {
    // Schedule the first auto-remount attempt 30 s after boot. Subsequent
    // retries land every 30 s while the device is in Paused/Menu and SD is
    // still unmounted — see the auto-remount block in update().
    nextSdRemountAtMs_ = millis() + 30UL * 1000UL;
  }

  // WiFi/OTA config: prefer SD's wifi.json (single source of truth), persist a
  // cached copy in NVS so OTA / Notifications / Downloads still work when the
  // SD is missing. Networks are stored as a single delimited string —
  // ssid<US>password<RS>ssid<US>password... — so an arbitrary list survives
  // reboots without juggling numbered keys.
  constexpr const char *kPrefWifiNetworks = "wifinets";
  constexpr const char *kPrefFirmwareUrl = "fwurl";
  constexpr const char *kPrefNotifUrl = "nfurl";
  constexpr const char *kPrefNotifTok = "nftok";
  constexpr char kNetFieldSep = '\x1F';   // unit separator
  constexpr char kNetRecordSep = '\x1E';  // record separator
  auto serializeNetworks = [&](const std::vector<OtaManager::Network> &nets) {
    String out;
    for (size_t i = 0; i < nets.size(); ++i) {
      if (i > 0) out += kNetRecordSep;
      out += nets[i].ssid;
      out += kNetFieldSep;
      out += nets[i].password;
    }
    return out;
  };
  auto deserializeNetworks = [&](const String &s) {
    std::vector<OtaManager::Network> out;
    int cursor = 0;
    while (cursor <= static_cast<int>(s.length())) {
      int rec = s.indexOf(kNetRecordSep, cursor);
      if (rec < 0) rec = s.length();
      const String entry = s.substring(cursor, rec);
      const int sep = entry.indexOf(kNetFieldSep);
      if (sep > 0) {
        OtaManager::Network n;
        n.ssid = entry.substring(0, sep);
        n.password = entry.substring(sep + 1);
        if (!n.ssid.isEmpty()) out.push_back(n);
      }
      if (rec >= static_cast<int>(s.length())) break;
      cursor = rec + 1;
    }
    return out;
  };
  bool haveNetConfig = false;
  if (storageReady && ota_.loadConfigFromSd())
  {
    const auto &cfg = ota_.config();
    preferences_.putString(kPrefWifiNetworks, serializeNetworks(cfg.networks));
    preferences_.putString(kPrefFirmwareUrl, cfg.firmwareUrl);
    preferences_.putString(kPrefNotifUrl, cfg.notificationsUrl);
    preferences_.putString(kPrefNotifTok, cfg.notificationsToken);
    haveNetConfig = true;
  }
  else
  {
    OtaManager::Config cached;
    cached.networks = deserializeNetworks(preferences_.getString(kPrefWifiNetworks, ""));
    cached.firmwareUrl = preferences_.getString(kPrefFirmwareUrl, "");
    cached.notificationsUrl = preferences_.getString(kPrefNotifUrl, "");
    cached.notificationsToken = preferences_.getString(kPrefNotifTok, "");
    if (!cached.networks.empty() && !cached.firmwareUrl.isEmpty())
    {
      ota_.setConfig(cached);
      Serial.printf("[app] using cached WiFi config from NVS (%u networks)\n",
                    static_cast<unsigned>(cached.networks.size()));
      haveNetConfig = true;
    }
  }
  if (haveNetConfig)
  {
    applyDefaultNetwork();
  }
  if (storageReady)
  {
    ringtoneNames_ = storage_.listRingtoneNames();
  }
  nextNotificationPollMs_ = millis() + kNotificationFirstPollDelayMs;
  const uint16_t savedWpm = preferences_.getUShort(kPrefWpm, reader_.wpm());
  reader_.setWpm(savedWpm);
  display_.setCurrentWpm(savedWpm);

  const uint8_t savedAppState = preferences_.getUChar(kPrefAppState,
                                                      static_cast<uint8_t>(AppState::Paused));
  const bool deferContentLoad =
      (savedAppState == static_cast<uint8_t>(AppState::Menu));

  bool haveBook = false;
  if (storageReady)
  {
    if (deferContentLoad)
    {
      haveBook = prepareSavedBookMeta();
    }
    else
    {
      haveBook = restoreSavedBook(bootStartedMs_);
    }
  }

  if (haveBook && !deferContentLoad)
  {
    usingStorageBook_ = true;
    bookMetaOnly_ = false;
  }
  else if (!haveBook)
  {
    usingStorageBook_ = false;
    bookMetaOnly_ = false;
    chapterMarkers_.clear();
    paragraphStarts_.clear();
    currentBookPath_ = "";
    currentBookTitle_ = storageReady && storage_.bookCount() > 0 ? "Pick a book" : "Demo";
    reader_.begin(bootStartedMs_);
    if (storageReady && storage_.bookCount() > 0)
    {
      Serial.printf("[app] %u books available; awaiting selection\n",
                    static_cast<unsigned int>(storage_.bookCount()));
    }
    else
    {
      Serial.println("[app] using built-in demo text");
    }
  }

  Serial.printf("[app] WPM=%u interval=%lu ms\n", reader_.wpm(),
                static_cast<unsigned long>(reader_.wordIntervalMs()));

  state_ = AppState::Booting;
  Serial.println("[app] READY splash active");

  const esp_partition_t *running = esp_ota_get_running_partition();
  if (running != nullptr)
  {
    esp_ota_img_states_t imgState;
    if (esp_ota_get_state_partition(running, &imgState) == ESP_OK &&
        imgState == ESP_OTA_IMG_PENDING_VERIFY)
    {
      firmwarePendingVerify_ = true;
      firmwareValidateAtMs_ = bootStartedMs_ + 60000;
      Serial.println("[ota] firmware pending verify; will mark valid in 60s if stable");
    }
  }
}

void App::update(uint32_t nowMs)
{
  button_.update(nowMs);
  powerButton_.update(nowMs);
  if (button_.isHeld() || powerButton_.isHeld())
  {
    lastActivityMs_ = nowMs;
  }
  handleBootButton(nowMs);
  handlePowerButton(nowMs);
  if (powerOffStarted_)
  {
    return;
  }

  // Screensaver: enter the dots animation after the configured idle window.
  // Cheaper than auto-off and the user might just be staring at a single
  // word — so it gates only on touch / button input, not on Playing state.
  if (screensaverIndex_ > 0 && screensaverIndex_ < kScreensaverOptionCount &&
      state_ != AppState::Screensaver && state_ != AppState::DemoPlaying &&
      state_ != AppState::Booting && state_ != AppState::UsbTransfer &&
      state_ != AppState::Sleeping && !powerOffStarted_)
  {
    const uint32_t idleLimitMs =
        static_cast<uint32_t>(kScreensaverMinutes[screensaverIndex_]) * 60UL * 1000UL;
    if (idleLimitMs > 0 && (nowMs - lastActivityMs_) >= idleLimitMs)
    {
      Serial.printf("[screensaver] idle %lu ms, entering\n",
                    static_cast<unsigned long>(nowMs - lastActivityMs_));
      screensaverPreviousState_ = state_;
      screensaver_.begin(nowMs);
      setState(AppState::Screensaver, nowMs);
    }
  }
  if (state_ == AppState::Screensaver)
  {
    // Min interval = 60fps target. The renderScreensaverFrame call blocks
    // for the full SPI flush (~12 ms after native-stripe rewrite), so this
    // throttle now actually engages and caps real frame rate at 60.
    constexpr uint32_t kScreensaverMinIntervalMs = 16;
    static uint32_t sLastScreensaverMs = 0;
    if (nowMs - sLastScreensaverMs >= kScreensaverMinIntervalMs) {
      sLastScreensaverMs = nowMs;
      display_.renderScreensaverFrame(screensaver_);
    }
  }
  if (state_ == AppState::DemoPlaying)
  {
    renderDemoFrame(nowMs);
  }

  // Auto-power-off: enterPowerOff after the configured idle window. Active
  // reading (state_ == Playing) and ongoing button holds count as activity, so
  // someone reading slowly never gets cut off mid-paragraph.
  if (autoPowerOffIndex_ > 0 && autoPowerOffIndex_ < kAutoPowerOffOptionCount &&
      state_ != AppState::Playing && state_ != AppState::Booting &&
      state_ != AppState::UsbTransfer && state_ != AppState::Sleeping &&
      !powerOffStarted_)
  {
    const uint32_t idleLimitMs =
        static_cast<uint32_t>(kAutoPowerOffMinutes[autoPowerOffIndex_]) * 60UL * 1000UL;
    if (idleLimitMs > 0 && (nowMs - lastActivityMs_) >= idleLimitMs)
    {
      Serial.printf("[autoff] idle %lu ms >= %lu ms, powering off\n",
                    static_cast<unsigned long>(nowMs - lastActivityMs_),
                    static_cast<unsigned long>(idleLimitMs));
      enterPowerOff(nowMs);
      return;
    }
  }

  if (firmwarePendingVerify_ && nowMs >= firmwareValidateAtMs_)
  {
    firmwarePendingVerify_ = false;
    if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK)
    {
      Serial.println("[ota] firmware marked valid (rollback cancelled)");
    }
    else
    {
      Serial.println("[ota] failed to mark firmware valid");
    }
  }

  const bool bannerActive = notificationBannerUntilMs_ != 0;

  if (!bannerActive && state_ == AppState::Menu && menuScreen_ == MenuScreen::Main &&
      (nowMs - lastMenuRefreshMs_) >= 33)
  {
    renderMainMenu();
    lastMenuRefreshMs_ = nowMs;
  }

  if (!bannerActive && state_ == AppState::Paused &&
      (nowMs - lastReaderRefreshMs_) >= 33)
  {
    renderReaderWord();
    lastReaderRefreshMs_ = nowMs;
  }

  // notifications_.poll() is fully synchronous and may spend up to ~60 s in
  // WiFi connect retries. If we let it start within the screensaver-trigger
  // window, the blocking call freezes the entire render loop right as the
  // screensaver activates — looks like a lockup, and on user-reported repros
  // it required a power-cycle to recover. Defer the poll if a screensaver
  // activation is imminent; the next interval will pick it up.
  bool screensaverImminent = false;
  if (screensaverIndex_ > 0 && screensaverIndex_ < kScreensaverOptionCount) {
    constexpr uint32_t kNotifPreEmptGuardMs = 45UL * 1000UL;
    const uint32_t idleLimitMs =
        static_cast<uint32_t>(kScreensaverMinutes[screensaverIndex_]) * 60UL * 1000UL;
    const uint32_t idleSoFar = nowMs - lastActivityMs_;
    if (idleLimitMs > 0 && idleSoFar + kNotifPreEmptGuardMs >= idleLimitMs) {
      screensaverImminent = true;
    }
  }
  if (notificationsEnabled_ && nowMs >= nextNotificationPollMs_ &&
      (state_ == AppState::Paused || state_ == AppState::Menu) &&
      !screensaverImminent)
  {
    pollNotifications(nowMs);
    nextNotificationPollMs_ = nowMs + kNotificationPollIntervalMs;
  }

  // Auto-remount SD if the boot mount failed. SD often fails its first attempt
  // immediately after a power glitch / OTA reboot but mounts cleanly a few
  // seconds later. We only retry while idle (Paused/Menu) so we don't stall
  // a render loop or interrupt reading.
  if (nextSdRemountAtMs_ != 0 && !storage_.isMounted() &&
      nowMs >= nextSdRemountAtMs_ &&
      (state_ == AppState::Paused || state_ == AppState::Menu))
  {
    Serial.println("[storage] auto-remount: retrying SD mount");
    if (storage_.begin()) {
      Serial.println("[storage] auto-remount: SUCCESS, refreshing library");
      storage_.listBooks();
      bookProgressInfo_.clear();
      // One-shot: don't keep retrying once we're back. If it later fails for
      // another reason, the user can hit Settings → Remount SD.
      nextSdRemountAtMs_ = 0;
      // If we were sitting on the demo splash because there was no library,
      // re-render the menu to surface the now-available book picker.
      if (state_ == AppState::Menu) renderMenu();
    } else {
      // Back off so we don't burn battery hammering an absent card.
      nextSdRemountAtMs_ = nowMs + 30UL * 1000UL;
    }
  }

  // Banner now stays up until the user taps to dismiss; no time-based clear.

  const bool batteryChanged = updateBatteryStatus(nowMs);
  updateState(nowMs);
  updateReader(nowMs);
  handleTouch(nowMs);
  updateWpmFeedback(nowMs);
  maybeSaveReadingPosition(nowMs);

  if (batteryChanged && (state_ == AppState::Paused || state_ == AppState::Playing))
  {
    if (contextViewVisible_)
    {
      renderContextPreview();
    }
    else if (wpmFeedbackVisible_)
    {
      renderWpmFeedback(nowMs);
    }
    else
    {
      renderReaderWord();
    }
  }
  else if (batteryChanged && state_ == AppState::Menu)
  {
    renderMenu();
  }

  if (nowMs - lastStateLogMs_ > 1500)
  {
    lastStateLogMs_ = nowMs;
    ESP_LOGI(kAppTag, "state=%s", stateName(state_));
    Serial.printf("[app] state=%s ms=%lu\n", stateName(state_),
                  static_cast<unsigned long>(nowMs));
  }
}

const char *App::stateName(AppState state) const
{
  switch (state)
  {
  case AppState::Booting:
    return "Booting";
  case AppState::Paused:
    return "Paused";
  case AppState::Playing:
    return "Playing";
  case AppState::Menu:
    return "Menu";
  case AppState::UsbTransfer:
    return "UsbTransfer";
  case AppState::Sleeping:
    return "Sleeping";
  case AppState::Screensaver:
    return "Screensaver";
  case AppState::DemoPlaying:
    return "DemoPlaying";
  }
  return "Unknown";
}

const char *App::touchPhaseName(TouchPhase phase) const
{
  switch (phase)
  {
  case TouchPhase::Start:
    return "Start";
  case TouchPhase::Move:
    return "Move";
  case TouchPhase::End:
    return "End";
  }
  return "Unknown";
}

void App::setState(AppState nextState, uint32_t nowMs)
{
  if (nextState == state_)
  {
    return;
  }

  const AppState previousState = state_;

  if (nextState != AppState::Paused)
  {
    pausedTouch_.active = false;
    pausedTouchIntent_ = TouchIntent::None;
    contextViewVisible_ = false;
    invalidateContextPreviewWindow();
    wpmFeedbackVisible_ = false;
  }
  if (nextState != AppState::Playing)
  {
    touchPlayHeld_ = false;
  }

  state_ = nextState;

  if (state_ == AppState::Paused || state_ == AppState::Menu)
  {
    preferences_.putUChar(kPrefAppState, static_cast<uint8_t>(state_));
  }

  switch (state_)
  {
  case AppState::Paused:
    renderReaderWord();
    break;
  case AppState::Playing:
    reader_.start(nowMs);
    renderReaderWord();
    break;
  case AppState::Menu:
    renderMenu();
    break;
  case AppState::UsbTransfer:
    display_.renderStatus("USB", "Preparing SD", "Eject when done");
    break;
  case AppState::Sleeping:
    display_.renderCenteredWord("SLEEP");
    break;
  case AppState::Booting:
    display_.renderCenteredWord("READY");
    break;
  case AppState::Screensaver:
    // First frame is rendered by the periodic update() tick — don't double-paint.
    break;
  case AppState::DemoPlaying:
    // Same: first frame comes from the next update() tick.
    break;
  }

  if (state_ == AppState::Paused && previousState == AppState::Playing)
  {
    saveReadingPosition(true);
  }

  ESP_LOGI(kAppTag, "state -> %s", stateName(state_));
  Serial.printf("[app] state -> %s at %lu ms\n", stateName(state_),
                static_cast<unsigned long>(nowMs));
}

void App::updateState(uint32_t nowMs)
{
  if (state_ == AppState::Booting)
  {
    if (nowMs - bootStartedMs_ < kBootSplashMs)
    {
      return;
    }

    if (touchPlayHeld_)
    {
      setState(AppState::Playing, nowMs);
      return;
    }
    const uint8_t savedState = preferences_.getUChar(kPrefAppState,
                                                     static_cast<uint8_t>(AppState::Paused));
    if (savedState == static_cast<uint8_t>(AppState::Menu))
    {
      menuScreen_ = MenuScreen::Main;
      menuSelectedIndex_ = MenuResume;
      setState(AppState::Menu, nowMs);
    }
    else
    {
      setState(AppState::Paused, nowMs);
    }
    return;
  }

  if (state_ == AppState::UsbTransfer)
  {
    updateUsbTransfer(nowMs);
    return;
  }

  if (state_ == AppState::Menu || state_ == AppState::Sleeping ||
      state_ == AppState::Screensaver || state_ == AppState::DemoPlaying)
  {
    // These states are exited by direct input only; don't auto-bounce back to
    // Paused/Playing. (Forgetting to add DemoPlaying here was the bug that
    // made every demo flash for 2 ms and immediately drop to Paused.)
    return;
  }

  if (touchPlayHeld_)
  {
    setState(AppState::Playing, nowMs);
    return;
  }

  setState(AppState::Paused, nowMs);
}

void App::updateReader(uint32_t nowMs)
{
  if (state_ != AppState::Playing)
  {
    return;
  }

  const size_t previousWordIndex = reader_.currentIndex();
  if (!reader_.update(nowMs))
  {
    // Reader didn't advance. If it's parked on the last word of the current
    // file AND this is part N of a multi-part book, auto-chain to part N+1.
    if (reader_.isAtEnd() && currentBookPath_.indexOf(".part") > 0)
    {
      String basePath = currentBookPath_;
      const int partAt = basePath.lastIndexOf(".part");
      const int dotRsvp = basePath.lastIndexOf(".rsvp");
      if (partAt > 0 && dotRsvp > partAt)
      {
        int currentPart = 0;
        for (int i = partAt + 5; i < dotRsvp && isDigit(basePath[i]); ++i)
        {
          currentPart = currentPart * 10 + (basePath[i] - '0');
        }
        const String nextPath = basePath.substring(0, partAt) + ".part" +
                                String(currentPart + 1) + ".rsvp";
        const int nextIndex = findBookIndexByPath(nextPath);
        if (nextIndex >= 0)
        {
          Serial.printf("[reader] end of part %d, chaining to %s\n", currentPart,
                        nextPath.c_str());
          loadBookAtIndex(static_cast<size_t>(nextIndex), nowMs, false);
          // Loading the next part resets currentIndex_ to 0; let the normal
          // advance machinery take it from there.
        }
      }
    }
    return;
  }

  // Chapter / paragraph chimes — fire once per crossing. We compare the
  // current index against the previous one and ask the chapter / paragraph
  // tables whether we just crossed a boundary, instead of polling each frame
  // (which would re-trigger every render until the index moves on).
  const size_t newIndex = reader_.currentIndex();
  if (newIndex != previousWordIndex && soundEnabled_)
  {
    const bool crossedChapter =
        chapterChimeEnabled_ &&
        std::any_of(chapterMarkers_.begin(), chapterMarkers_.end(),
                    [&](const ChapterMarker &m) {
                      return m.wordIndex > previousWordIndex && m.wordIndex <= newIndex;
                    });
    const bool crossedParagraph =
        paragraphChimeEnabled_ &&
        std::any_of(paragraphStarts_.begin(), paragraphStarts_.end(),
                    [&](size_t idx) { return idx > previousWordIndex && idx <= newIndex; });
    // "Page" boundaries are every kPageWordCount words. We fire a chime when
    // the floor-divided page index changes between previous and new word.
    const bool crossedPage =
        pageChimeEnabled_ &&
        (previousWordIndex / kPageWordCount) != (newIndex / kPageWordCount);
    if (crossedChapter || crossedParagraph || crossedPage)
    {
      // Three distinct timbres so chapter/page/paragraph are distinguishable
      // by ear. Chapter is highest, page mid, paragraph lowest.
      audio_.setVolumePercent(notificationVolume_);
      const char *rtttl = crossedChapter ? "chap:d=8,o=6,b=180:e,g"
                          : crossedPage  ? "page:d=8,o=6,b=180:d"
                                         : "para:d=16,o=6,b=180:c";
      audio_.playRtttl(rtttl);
    }
  }

  renderReaderWord();
}

void App::maybeSaveReadingPosition(uint32_t nowMs)
{
  if (!usingStorageBook_ || currentBookPath_.isEmpty() || state_ != AppState::Playing)
  {
    return;
  }

  if (nowMs - lastProgressSaveMs_ < kProgressSaveIntervalMs)
  {
    return;
  }

  lastProgressSaveMs_ = nowMs;
  saveReadingPosition(false);
}

void App::handleBootButton(uint32_t nowMs)
{
  if (state_ == AppState::UsbTransfer || state_ == AppState::Sleeping || powerOffStarted_)
  {
    return;
  }

  if (!bootButtonReleasedSinceBoot_)
  {
    if (!button_.isHeld())
    {
      bootButtonReleasedSinceBoot_ = true;
    }
    return;
  }

  if (button_.isHeld() && !bootButtonLongPressHandled_ &&
      button_.heldDurationMs(nowMs) >= kThemeToggleHoldMs)
  {
    bootButtonLongPressHandled_ = true;
    cycleThemeMode(nowMs);
    return;
  }

  if (!button_.wasReleasedEvent())
  {
    return;
  }

  if (bootButtonLongPressHandled_)
  {
    bootButtonLongPressHandled_ = false;
    return;
  }

  if (button_.lastHoldDurationMs() < kThemeToggleHoldMs)
  {
    cycleBrightness();
  }
}

void App::handlePowerButton(uint32_t nowMs)
{
  if (!powerButtonReleasedSinceBoot_)
  {
    if (!powerButton_.isHeld())
    {
      powerButtonReleasedSinceBoot_ = true;
    }
    return;
  }

  if (state_ == AppState::UsbTransfer || powerOffStarted_)
  {
    return;
  }

  if (powerButton_.isHeld() && nowMs - powerButton_.lastEdgeMs() >= kPowerOffHoldMs)
  {
    powerButtonLongPressHandled_ = true;
    enterPowerOff(nowMs);
    return;
  }

  if (!powerButton_.wasReleasedEvent())
  {
    return;
  }

  if (powerButtonLongPressHandled_)
  {
    powerButtonLongPressHandled_ = false;
    return;
  }

  toggleMenuFromPowerButton(nowMs);
}

void App::toggleMenuFromPowerButton(uint32_t nowMs)
{
  if (state_ == AppState::Booting || state_ == AppState::UsbTransfer ||
      state_ == AppState::Sleeping)
  {
    return;
  }

  if (state_ == AppState::Menu)
  {
    if (menuScreen_ == MenuScreen::Main)
    {
      // Load full book content if we only have metadata, otherwise the reader's
      // loadedWords_ is empty and wordAt() falls back to the built-in demo word
      // array — chip stays right (from metadata) but words are wrong.
      if (bookMetaOnly_)
      {
        ensureCurrentBookLoaded(nowMs);
      }
      setState(AppState::Paused, nowMs);
    }
    else
    {
      menuScreen_ = MenuScreen::Main;
      renderMainMenu();
    }
    return;
  }

  openMainMenu(nowMs);
}

void App::openMainMenu(uint32_t nowMs)
{
  pausedTouch_.active = false;
  pausedTouchIntent_ = TouchIntent::None;
  touchPlayHeld_ = false;
  menuScreen_ = MenuScreen::Main;
  menuSelectedIndex_ = MenuResume;
  wpmFeedbackVisible_ = false;
  contextViewVisible_ = false;
  if (state_ == AppState::Playing)
  {
    saveReadingPosition(true);
  }
  setState(AppState::Menu, nowMs);
}

uint8_t App::currentBrightnessPercent() const
{
  return nightMode_ ? kNightBrightnessLevels[brightnessLevelIndex_]
                    : kBrightnessLevels[brightnessLevelIndex_];
}

void App::applyDisplayPreferences(uint32_t nowMs, bool rerender)
{
  display_.setDarkMode(darkMode_);
  display_.setNightMode(nightMode_);
  display_.setBrightnessPercent(currentBrightnessPercent());

  if (!rerender)
  {
    return;
  }

  if (state_ == AppState::Menu)
  {
    if (menuScreen_ == MenuScreen::SettingsHome || menuScreen_ == MenuScreen::SettingsDisplay ||
        menuScreen_ == MenuScreen::SettingsPacing || menuScreen_ == MenuScreen::SettingsReadingSounds)
    {
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    }
    renderMenu();
    return;
  }

  if (state_ == AppState::Paused || state_ == AppState::Playing)
  {
    if (contextViewVisible_)
    {
      renderContextPreview();
    }
    else if (wpmFeedbackVisible_)
    {
      renderWpmFeedback(nowMs);
    }
    else
    {
      renderReaderWord();
    }
    return;
  }

  if (state_ == AppState::Booting)
  {
    display_.renderCenteredWord("READY");
  }
}

void App::applyTypographySettings(uint32_t nowMs, bool rerender)
{
  display_.setTypographyConfig(typographyConfig_);

  Serial.printf("[typography] track=%d anchor=%u guideWidth=%u guideGap=%u\n",
                static_cast<int>(typographyConfig_.trackingPx),
                static_cast<unsigned int>(typographyConfig_.anchorPercent),
                static_cast<unsigned int>(typographyConfig_.guideHalfWidth),
                static_cast<unsigned int>(typographyConfig_.guideGap));

  if (!rerender)
  {
    return;
  }

  if (state_ == AppState::Menu)
  {
    renderMenu();
    return;
  }

  if (state_ == AppState::Paused || state_ == AppState::Playing)
  {
    if (contextViewVisible_)
    {
      renderContextPreview();
    }
    else if (wpmFeedbackVisible_)
    {
      renderWpmFeedback(nowMs);
    }
    else
    {
      renderReaderWord();
    }
  }
}

void App::cycleBrightness()
{
  brightnessLevelIndex_ = static_cast<uint8_t>((brightnessLevelIndex_ + 1) % kBrightnessLevelCount);
  preferences_.putUChar(kPrefBrightness, brightnessLevelIndex_);
  const uint8_t percent = currentBrightnessPercent();
  Serial.printf("[display] brightness level %u/%u (%u%%)\n",
                static_cast<unsigned int>(brightnessLevelIndex_ + 1),
                static_cast<unsigned int>(kBrightnessLevelCount),
                static_cast<unsigned int>(percent));
  applyDisplayPreferences(millis());
}

void App::cycleThemeMode(uint32_t nowMs)
{
  if (nightMode_)
  {
    nightMode_ = false;
    darkMode_ = true;
  }
  else if (darkMode_)
  {
    darkMode_ = false;
  }
  else
  {
    darkMode_ = true;
    nightMode_ = true;
  }

  preferences_.putBool(kPrefDarkMode, darkMode_);
  preferences_.putBool(kPrefNightMode, nightMode_);
  Serial.printf("[display] theme=%s\n", themeModeLabel().c_str());
  applyDisplayPreferences(nowMs);
}

void App::toggleDisplayFlip(uint32_t nowMs)
{
  (void)nowMs;
  displayFlipped_ = !displayFlipped_;
  preferences_.putBool(kPrefDisplayFlip, displayFlipped_);
  display_.setUiRotated(displayFlipped_);
  touch_.setUiRotated(displayFlipped_);
  rebuildSettingsMenuItems();
  renderSettings();
  Serial.printf("[display] flipped=%d\n", displayFlipped_ ? 1 : 0);
}

void App::togglePhantomWords(uint32_t nowMs)
{
  phantomWordsEnabled_ = !phantomWordsEnabled_;
  preferences_.putBool(kPrefPhantomWords, phantomWordsEnabled_);
  Serial.printf("[display] phantom words=%s\n", phantomWordsLabel().c_str());
  applyDisplayPreferences(nowMs);
}

void App::cycleReaderFontSize(uint32_t nowMs)
{
  readerFontSizeIndex_ = static_cast<uint8_t>((readerFontSizeIndex_ + 1) % kReaderFontSizeCount);
  preferences_.putUChar(kPrefReaderFontSize, readerFontSizeIndex_);
  Serial.printf("[display] font size=%s\n", readerFontSizeLabel().c_str());
  applyDisplayPreferences(nowMs);
}

bool App::updateBatteryStatus(uint32_t nowMs, bool force)
{
  // Battery sampling toggles shared board hardware; avoid doing that during active reading.
  if (!force && state_ == AppState::Playing)
  {
    return false;
  }

  if (!force && nowMs - lastBatterySampleMs_ < kBatterySampleIntervalMs)
  {
    return false;
  }

  lastBatterySampleMs_ = nowMs;

  BoardConfig::BatteryStatus status;
  String nextLabel;
  if (BoardConfig::readBatteryStatus(status))
  {
    nextLabel = String(status.percent) + "%";
  }
  else
  {
    nextLabel = "";
  }

  if (nextLabel == batteryLabel_)
  {
    return false;
  }

  batteryLabel_ = nextLabel;
  display_.setBatteryLabel(batteryLabel_);
  if (!batteryLabel_.isEmpty())
  {
    Serial.printf("[power] battery %.2f V (%u%%)\n", status.voltage,
                  static_cast<unsigned int>(status.percent));
  }
  else
  {
    Serial.println("[power] battery not detected");
  }
  return true;
}

void App::updateWpmFeedback(uint32_t nowMs)
{
  if (!wpmFeedbackVisible_ || state_ != AppState::Paused)
  {
    return;
  }

  if (nowMs < wpmFeedbackUntilMs_)
  {
    return;
  }

  wpmFeedbackVisible_ = false;
  renderReaderWord();
}

void App::handleTouch(uint32_t nowMs)
{
  if (!touchInitialized_)
  {
    return;
  }

  if (state_ == AppState::Booting || state_ == AppState::UsbTransfer ||
      state_ == AppState::Sleeping)
  {
    touch_.cancel();
    pausedTouch_.active = false;
    pausedTouchIntent_ = TouchIntent::None;
    touchPlayHeld_ = false;
    return;
  }

  TouchEvent ev;
  if (!touch_.poll(ev))
  {
    return;
  }
  // Any touch counts as activity for the auto-power-off timer.
  lastActivityMs_ = nowMs;

  // Screensaver: any touch dismisses it. Restore the previous state and let
  // the periodic refresh repaint the underlying screen.
  if (state_ == AppState::Screensaver)
  {
    Serial.println("[screensaver] dismissed by touch");
    setState(screensaverPreviousState_, nowMs);
    if (state_ == AppState::Menu)
    {
      renderMenu();
    }
    else
    {
      renderReaderWord();
    }
    return;
  }
  // Demo: same lifecycle. Any touch ends playback and returns to the picker.
  if (state_ == AppState::DemoPlaying)
  {
    Serial.println("[demo] dismissed by touch");
    exitDemoPlayback(nowMs);
    return;
  }

  Serial.printf("[touch] phase=%s touched=%u x=%u y=%u gesture=%u state=%s\n",
                touchPhaseName(ev.phase), ev.touched ? 1 : 0, ev.x, ev.y, ev.gesture,
                stateName(state_));

  // Notification banner consumes any tap. The first End event dismisses it and
  // we don't pass the gesture to the underlying state, so the user doesn't
  // accidentally trigger the menu/reader behind it.
  if (notificationBannerUntilMs_ != 0)
  {
    if (ev.phase == TouchPhase::End)
    {
      notificationBannerUntilMs_ = 0;
      notificationBannerTitle_ = "";
      notificationBannerBody_ = "";
      pausedTouch_.active = false;
      pausedTouchIntent_ = TouchIntent::None;
      if (state_ == AppState::Menu)
      {
        renderMenu();
      }
      else
      {
        renderReaderWord();
      }
    }
    return;
  }

  if (state_ == AppState::Menu)
  {
    applyMenuTouchGesture(ev, nowMs);
  }
  else
  {
    applyPausedTouchGesture(ev, nowMs);
  }
}

void App::applyPausedTouchGesture(const TouchEvent &event, uint32_t nowMs)
{
  if (event.phase == TouchPhase::End && touchPlayHeld_)
  {
    touchPlayHeld_ = false;
    pausedTouch_.active = false;
    pausedTouchIntent_ = TouchIntent::None;
    setState(AppState::Paused, nowMs);
    return;
  }

  if (state_ == AppState::Playing)
  {
    return;
  }

  if (event.phase == TouchPhase::Start)
  {
    pausedTouch_.active = true;
    pausedTouchIntent_ = TouchIntent::None;
    invalidateContextPreviewWindow();
    pausedTouch_.startX = event.x;
    pausedTouch_.startY = event.y;
    pausedTouch_.lastX = event.x;
    pausedTouch_.lastY = event.y;
    pausedTouch_.startMs = nowMs;
    pausedTouch_.lastMs = nowMs;
    pausedTouch_.startWordIndex = reader_.currentIndex();
    pausedTouch_.scrubStepsApplied = 0;
    return;
  }

  if (!pausedTouch_.active)
  {
    return;
  }

  pausedTouch_.lastX = event.x;
  pausedTouch_.lastY = event.y;
  pausedTouch_.lastMs = nowMs;

  const int deltaX = static_cast<int>(pausedTouch_.lastX) - static_cast<int>(pausedTouch_.startX);
  const int deltaY = static_cast<int>(pausedTouch_.lastY) - static_cast<int>(pausedTouch_.startY);
  const int absDeltaX = abs(deltaX);
  const int absDeltaY = abs(deltaY);
  const uint32_t pressDurationMs = nowMs - pausedTouch_.startMs;
  const bool ended = event.phase == TouchPhase::End;
  const bool tapLike = absDeltaX <= static_cast<int>(kTapSlopPx) &&
                       absDeltaY <= static_cast<int>(kTapSlopPx);

  if (!ended && pausedTouchIntent_ == TouchIntent::None &&
      pressDurationMs >= kTouchPlayHoldMs && tapLike)
  {
    touchPlayHeld_ = true;
    pausedTouchIntent_ = TouchIntent::PlayHold;
    wpmFeedbackVisible_ = false;
    setState(AppState::Playing, nowMs);
    return;
  }

  if (pausedTouchIntent_ == TouchIntent::None)
  {
    if (absDeltaX >= static_cast<int>(kSwipeThresholdPx) &&
        absDeltaX > absDeltaY + static_cast<int>(kAxisBiasPx))
    {
      pausedTouchIntent_ = TouchIntent::Scrub;
    }
    else if (absDeltaY >= static_cast<int>(kSwipeThresholdPx) &&
             absDeltaY > absDeltaX + static_cast<int>(kAxisBiasPx))
    {
      pausedTouchIntent_ = TouchIntent::Wpm;
    }
  }

  if (pausedTouchIntent_ == TouchIntent::Scrub)
  {
    applyScrubTarget(scrubStepsForDrag(deltaX));
    if (ended)
    {
      pausedTouch_.active = false;
      pausedTouchIntent_ = TouchIntent::None;
      saveReadingPosition(true);
    }
    return;
  }

  if (pausedTouchIntent_ == TouchIntent::Wpm)
  {
    if (!ended)
    {
      return;
    }

    const int wpmDelta = (deltaY < 0) ? 1 : -1;
    reader_.adjustWpm(wpmDelta);
    preferences_.putUShort(kPrefWpm, reader_.wpm());
    display_.setCurrentWpm(reader_.wpm());
    renderWpmFeedback(nowMs);
    Serial.printf("[app] WPM=%u interval=%lu ms\n", reader_.wpm(),
                  static_cast<unsigned long>(reader_.wordIntervalMs()));
    pausedTouch_.active = false;
    pausedTouchIntent_ = TouchIntent::None;
    return;
  }

  if (ended)
  {
    pausedTouch_.active = false;
    pausedTouchIntent_ = TouchIntent::None;
  }

  // Paused taps are intentionally ignored unless they become part of the UX.
}

int App::scrubStepsForDrag(int deltaX) const
{
  const int absDeltaX = abs(deltaX);
  if (absDeltaX < static_cast<int>(kSwipeThresholdPx))
  {
    return 0;
  }

  int steps = 1 + ((absDeltaX - static_cast<int>(kSwipeThresholdPx)) /
                   static_cast<int>(kScrubStepPx));
  steps = std::min(steps, kMaxScrubStepsPerGesture);

  return (deltaX > 0) ? steps : -steps;
}

void App::applyScrubTarget(int targetSteps)
{
  if (targetSteps == pausedTouch_.scrubStepsApplied)
  {
    return;
  }

  reader_.seekRelative(pausedTouch_.startWordIndex, targetSteps);
  pausedTouch_.scrubStepsApplied = targetSteps;
  wpmFeedbackVisible_ = false;
  renderContextPreview();
  Serial.printf("[app] scrub target=%d word=%s\n", targetSteps, reader_.currentWord().c_str());
}

void App::applyMenuTouchGesture(const TouchEvent &event, uint32_t nowMs)
{
  if (event.phase == TouchPhase::Start)
  {
    pausedTouch_.active = true;
    pausedTouchIntent_ = TouchIntent::None;
    pausedTouch_.startX = event.x;
    pausedTouch_.startY = event.y;
    pausedTouch_.lastX = event.x;
    pausedTouch_.lastY = event.y;
    pausedTouch_.startMs = nowMs;
    pausedTouch_.lastMs = nowMs;
    bookLongPressFired_ = false;
    return;
  }

  if (!pausedTouch_.active)
  {
    return;
  }

  pausedTouch_.lastX = event.x;
  pausedTouch_.lastY = event.y;
  pausedTouch_.lastMs = nowMs;

  // Long-press on a book row in the BookPicker opens the delete-confirm screen.
  // Threshold is 800 ms with little movement so it doesn't fire on a slow swipe.
  // Skip if the touch started in (or has activated) the right-edge letter
  // strip — that's the alphabet scrubber, not a row hold.
  const int letterStripStartX =
      BoardConfig::DISPLAY_WIDTH - DisplayManager::kLibraryLetterStripWidth;
  const bool touchInLetterStrip = pausedTouch_.startX >= letterStripStartX;
  if (!bookLongPressFired_ && menuScreen_ == MenuScreen::BookPicker &&
      event.phase != TouchPhase::End && (nowMs - pausedTouch_.startMs) >= 800 &&
      !letterScrubActive_ && !touchInLetterStrip)
  {
    const int dx = abs(static_cast<int>(event.x) - static_cast<int>(pausedTouch_.startX));
    const int dy = abs(static_cast<int>(event.y) - static_cast<int>(pausedTouch_.startY));
    if (dx <= static_cast<int>(kTapSlopPx) && dy <= static_cast<int>(kTapSlopPx) &&
        bookPickerSelectedIndex_ >= 1 &&
        bookPickerSelectedIndex_ - 1 < bookPickerBookIndices_.size())
    {
      bookLongPressFired_ = true;
      pausedTouch_.active = false;
      const size_t storageIndex = bookPickerBookIndices_[bookPickerSelectedIndex_ - 1];
      openBookDeleteConfirm(storageIndex);
      return;
    }
  }

  const bool inPicker =
      menuScreen_ == MenuScreen::AuthorPicker || menuScreen_ == MenuScreen::BookPicker;
  if (inPicker)
  {
    const int stripStartX = BoardConfig::DISPLAY_WIDTH - DisplayManager::kLibraryLetterStripWidth;
    const bool startInStrip = pausedTouch_.startX >= stripStartX;
    if (startInStrip && !letterScrubActive_ &&
        (nowMs - pausedTouch_.startMs) >= 350)
    {
      letterScrubActive_ = true;
      const std::vector<char> &lettersInit =
          (menuScreen_ == MenuScreen::AuthorPicker) ? authorPickerLetterAnchors_
                                                    : bookPickerLetterAnchors_;
      letterScrubFocusIdx_ =
          DisplayManager::libraryLetterAtY(lettersInit, pausedTouch_.startY);
      if (letterScrubFocusIdx_ < 0)
        letterScrubFocusIdx_ = 0;
    }
    if (letterScrubActive_)
    {
      const std::vector<char> &letters =
          (menuScreen_ == MenuScreen::AuthorPicker) ? authorPickerLetterAnchors_
                                                    : bookPickerLetterAnchors_;
      const std::vector<size_t> &targets =
          (menuScreen_ == MenuScreen::AuthorPicker) ? authorPickerLetterTargets_
                                                    : bookPickerLetterTargets_;
      const int currentFocus =
          letterScrubFocusIdx_ >= 0 ? letterScrubFocusIdx_ : 0;
      const int idx =
          DisplayManager::libraryScrubLetterAtY(letters, event.y, currentFocus);
      if (idx >= 0 && static_cast<size_t>(idx) < targets.size())
      {
        letterScrubFocusIdx_ = idx;
        if (menuScreen_ == MenuScreen::AuthorPicker)
        {
          authorPickerSelectedIndex_ = targets[idx];
          renderAuthorPicker();
        }
        else
        {
          bookPickerSelectedIndex_ = targets[idx];
          renderBookPicker();
        }
      }
      if (event.phase == TouchPhase::End)
      {
        letterScrubActive_ = false;
        letterScrubFocusIdx_ = -1;
        pausedTouch_.active = false;
        renderMenu();
      }
      return;
    }
  }

  if (event.phase != TouchPhase::End)
  {
    return;
  }

  pausedTouch_.active = false;

  const int deltaX = static_cast<int>(pausedTouch_.lastX) - static_cast<int>(pausedTouch_.startX);
  const int deltaY = static_cast<int>(pausedTouch_.lastY) - static_cast<int>(pausedTouch_.startY);
  const int absDeltaX = abs(deltaX);
  const int absDeltaY = abs(deltaY);

  if (menuScreen_ == MenuScreen::TypographyTuning &&
      absDeltaX >= static_cast<int>(kSwipeThresholdPx) &&
      absDeltaX > absDeltaY + static_cast<int>(kAxisBiasPx))
  {
    cycleTypographyPreviewSample(deltaX < 0 ? 1 : -1);
    return;
  }

  if (absDeltaY >= static_cast<int>(kSwipeThresholdPx) &&
      absDeltaY > absDeltaX + static_cast<int>(kAxisBiasPx))
  {
    moveMenuSelection(deltaY < 0 ? -1 : 1);
    return;
  }

  if (absDeltaX <= static_cast<int>(kTapSlopPx) && absDeltaY <= static_cast<int>(kTapSlopPx))
  {
    const int stripX = BoardConfig::DISPLAY_WIDTH - DisplayManager::kLibraryLetterStripWidth;
    if (pausedTouch_.startX >= stripX)
    {
      if (menuScreen_ == MenuScreen::AuthorPicker && !authorPickerLetterAnchors_.empty())
      {
        const int letterIdx = DisplayManager::libraryLetterAtY(authorPickerLetterAnchors_,
                                                               pausedTouch_.startY);
        if (letterIdx >= 0 &&
            static_cast<size_t>(letterIdx) < authorPickerLetterTargets_.size())
        {
          authorPickerSelectedIndex_ = authorPickerLetterTargets_[letterIdx];
          renderAuthorPicker();
          return;
        }
      }
      else if (menuScreen_ == MenuScreen::BookPicker && !bookPickerLetterAnchors_.empty())
      {
        const int letterIdx = DisplayManager::libraryLetterAtY(bookPickerLetterAnchors_,
                                                               pausedTouch_.startY);
        if (letterIdx >= 0 &&
            static_cast<size_t>(letterIdx) < bookPickerLetterTargets_.size())
        {
          bookPickerSelectedIndex_ = bookPickerLetterTargets_[letterIdx];
          renderBookPicker();
          return;
        }
      }
    }
    selectMenuItem(nowMs);
  }
}

void App::moveMenuSelection(int direction)
{
  if (direction == 0)
  {
    return;
  }

  size_t *selectedIndex = &menuSelectedIndex_;
  size_t itemCount = MenuItemCount;
  if (menuScreen_ == MenuScreen::SettingsHome || menuScreen_ == MenuScreen::SettingsDisplay ||
      menuScreen_ == MenuScreen::SettingsPacing || menuScreen_ == MenuScreen::SettingsReadingSounds)
  {
    selectedIndex = &settingsSelectedIndex_;
    itemCount = settingsMenuItems_.size();
  }
  else if (menuScreen_ == MenuScreen::TypographyTuning)
  {
    selectedIndex = &typographyTuningSelectedIndex_;
    itemCount = TypographyTuningItemCount;
  }
  else if (menuScreen_ == MenuScreen::BookPicker)
  {
    selectedIndex = &bookPickerSelectedIndex_;
    itemCount = bookMenuItems_.size();
  }
  else if (menuScreen_ == MenuScreen::AuthorPicker)
  {
    selectedIndex = &authorPickerSelectedIndex_;
    itemCount = authorMenuItems_.size();
  }
  else if (menuScreen_ == MenuScreen::ChapterPicker)
  {
    selectedIndex = &chapterPickerSelectedIndex_;
    itemCount = chapterMenuItems_.size();
  }
  else if (menuScreen_ == MenuScreen::TonePicker)
  {
    selectedIndex = &tonePickerSelectedIndex_;
    itemCount = toneMenuItems_.size();
  }
  else if (menuScreen_ == MenuScreen::RemoteBookPicker)
  {
    selectedIndex = &remoteBookSelectedIndex_;
    itemCount = remoteBookMenuItems_.size();
  }
  else if (menuScreen_ == MenuScreen::NetworkPicker)
  {
    selectedIndex = &networkSelectedIndex_;
    itemCount = networkMenuItems_.size();
  }
  else if (menuScreen_ == MenuScreen::BookDeleteConfirm)
  {
    selectedIndex = &bookDeleteSelectedIndex_;
    itemCount = BookDeleteOptionCount;
  }
  else if (menuScreen_ == MenuScreen::RestartConfirm)
  {
    selectedIndex = &restartConfirmSelectedIndex_;
    itemCount = RestartConfirmItemCount;
  }
  else if (menuScreen_ == MenuScreen::DemoPicker)
  {
    selectedIndex = &demoSelectedIndex_;
    itemCount = kDemoPickerItemCount;
  }

  if (itemCount == 0)
  {
    return;
  }

  const int next = static_cast<int>(*selectedIndex) + direction;
  if (next < 0)
  {
    *selectedIndex = itemCount - 1;
  }
  else if (next >= static_cast<int>(itemCount))
  {
    *selectedIndex = 0;
  }
  else
  {
    *selectedIndex = static_cast<size_t>(next);
  }

  renderMenu();
  if (menuScreen_ == MenuScreen::SettingsHome || menuScreen_ == MenuScreen::SettingsDisplay ||
      menuScreen_ == MenuScreen::SettingsPacing || menuScreen_ == MenuScreen::SettingsReadingSounds)
  {
    Serial.printf("[settings] selected=%s\n", settingsMenuItems_[settingsSelectedIndex_].c_str());
  }
  else if (menuScreen_ == MenuScreen::TypographyTuning)
  {
    Serial.printf("[typography] selected=%s\n", typographyTuningLabel().c_str());
  }
  else if (menuScreen_ == MenuScreen::BookPicker)
  {
    Serial.printf("[book-picker] selected=%s\n",
                  bookMenuItems_[bookPickerSelectedIndex_].title.c_str());
  }
  else if (menuScreen_ == MenuScreen::AuthorPicker)
  {
    Serial.printf("[author-picker] selected=%s\n",
                  authorMenuItems_[authorPickerSelectedIndex_].title.c_str());
  }
  else if (menuScreen_ == MenuScreen::ChapterPicker)
  {
    Serial.printf("[chapter-picker] selected=%s\n",
                  chapterMenuItems_[chapterPickerSelectedIndex_].c_str());
  }
  else if (menuScreen_ == MenuScreen::RestartConfirm)
  {
    Serial.printf("[restart] selected=%s\n",
                  kRestartConfirmItems[restartConfirmSelectedIndex_ + kRestartConfirmHeaderRows]);
  }
  else
  {
    Serial.printf("[menu] selected=%s\n", kMenuItems[menuSelectedIndex_]);
  }
}

void App::selectMenuItem(uint32_t nowMs)
{
  if (menuScreen_ == MenuScreen::SettingsHome || menuScreen_ == MenuScreen::SettingsDisplay ||
      menuScreen_ == MenuScreen::SettingsPacing || menuScreen_ == MenuScreen::SettingsReadingSounds)
  {
    selectSettingsItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::TypographyTuning)
  {
    selectTypographyTuningItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::AuthorPicker)
  {
    selectAuthorPickerItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::BookPicker)
  {
    selectBookPickerItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::ChapterPicker)
  {
    selectChapterPickerItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::TonePicker)
  {
    selectTonePickerItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::RemoteBookPicker)
  {
    selectRemoteBookPickerItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::NetworkPicker)
  {
    selectNetworkPickerItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::BookDeleteConfirm)
  {
    selectBookDeleteConfirmItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::RestartConfirm)
  {
    selectRestartConfirmItem(nowMs);
    return;
  }
  if (menuScreen_ == MenuScreen::DemoPicker)
  {
    selectDemoPickerItem(nowMs);
    return;
  }

  switch (menuSelectedIndex_)
  {
  case MenuResume:
    if (bookMetaOnly_)
    {
      ensureCurrentBookLoaded(nowMs);
    }
    setState(AppState::Paused, nowMs);
    return;
  case MenuRestart:
    openRestartConfirm();
    return;
  case MenuOtaUpdate:
    runOtaUpdate(nowMs);
    return;
  case MenuPowerOff:
    enterPowerOff(nowMs);
    return;
#if RSVP_USB_TRANSFER_ENABLED
  case MenuUsbTransfer:
    enterUsbTransfer(nowMs);
    return;
#endif
  case MenuChapters:
    openChapterPicker();
    return;
  case MenuChangeBook:
    activeAuthorFilter_ = "";
    recentOnlyFilter_ = false;
    openAuthorPicker();
    return;
  case MenuDownloadBooks:
    openRemoteBookPicker(nowMs);
    return;
  case MenuResumeFrom:
    activeAuthorFilter_ = "";
    recentOnlyFilter_ = true;
    openBookPicker();
    return;
  case MenuSettings:
    openSettings();
    return;
  default:
    return;
  }
}

void App::openSettings()
{
  settingsSelectedIndex_ = kSettingsHomeDisplayIndex;
  menuScreen_ = MenuScreen::SettingsHome;
  rebuildSettingsMenuItems();
  renderSettings();
}

void App::selectSettingsItem(uint32_t nowMs)
{
  if (settingsMenuItems_.empty())
  {
    openSettings();
    return;
  }

  if (menuScreen_ == MenuScreen::SettingsHome)
  {
    switch (settingsSelectedIndex_)
    {
    case kSettingsBackIndex:
      menuScreen_ = MenuScreen::Main;
      renderMainMenu();
      return;
    case kSettingsHomeDisplayIndex:
      settingsSelectedIndex_ = kSettingsDisplayThemeIndex;
      menuScreen_ = MenuScreen::SettingsDisplay;
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsHomeTypographyIndex:
      openTypographyTuning();
      return;
    case kSettingsHomePacingIndex:
      settingsSelectedIndex_ = kSettingsPacingLongWordsIndex;
      menuScreen_ = MenuScreen::SettingsPacing;
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsHomeSoundIndex:
      soundEnabled_ = !soundEnabled_;
      preferences_.putBool(kPrefSoundEnabled, soundEnabled_);
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsHomeNotificationsIndex:
      toggleNotificationsEnabled(nowMs);
      return;
    case kSettingsHomeToneIndex:
      openTonePicker();
      return;
    case kSettingsHomeVolumeIndex:
      cycleNotificationVolume(nowMs);
      return;
    case kSettingsHomeReadingSoundsIndex:
      settingsSelectedIndex_ = kSettingsReadingSoundsChapterIndex;
      menuScreen_ = MenuScreen::SettingsReadingSounds;
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsHomeNetworkIndex:
      openNetworkPicker();
      return;
    case kSettingsHomeScreensaverIndex:
      screensaverIndex_ =
          static_cast<uint8_t>((screensaverIndex_ + 1) % kScreensaverOptionCount);
      preferences_.putUChar(kPrefScreensaver, screensaverIndex_);
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsHomeDemosIndex:
      openDemoPicker();
      return;
    case kSettingsHomeRemountSdIndex:
    {
      display_.renderStatus("SD", "Unmounting", "");
      storage_.end();
      delay(100);
      display_.renderStatus("SD", "Remounting", "");
      const bool ok = storage_.begin();
      if (ok)
      {
        storage_.listBooks();
        bookProgressInfo_.clear();
        display_.renderStatus("SD", "Mounted", String(static_cast<unsigned>(storage_.bookCount())).c_str());
      }
      else
      {
        display_.renderStatus("SD", "Mount failed", "Reseat the card");
      }
      delay(900);
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    }
    default:
      return;
    }
  }

  if (menuScreen_ == MenuScreen::SettingsReadingSounds)
  {
    switch (settingsSelectedIndex_)
    {
    case kSettingsBackIndex:
      settingsSelectedIndex_ = kSettingsHomeReadingSoundsIndex;
      menuScreen_ = MenuScreen::SettingsHome;
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsReadingSoundsChapterIndex:
      chapterChimeEnabled_ = !chapterChimeEnabled_;
      preferences_.putBool(kPrefChapterChime, chapterChimeEnabled_);
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsReadingSoundsParagraphIndex:
      paragraphChimeEnabled_ = !paragraphChimeEnabled_;
      preferences_.putBool(kPrefParagraphChime, paragraphChimeEnabled_);
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsReadingSoundsPageIndex:
      pageChimeEnabled_ = !pageChimeEnabled_;
      preferences_.putBool(kPrefPageChime, pageChimeEnabled_);
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    default:
      return;
    }
  }

  if (menuScreen_ == MenuScreen::SettingsDisplay)
  {
    switch (settingsSelectedIndex_)
    {
    case kSettingsBackIndex:
      settingsSelectedIndex_ = kSettingsHomeDisplayIndex;
      menuScreen_ = MenuScreen::SettingsHome;
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    case kSettingsDisplayThemeIndex:
      cycleThemeMode(nowMs);
      return;
    case kSettingsDisplayBrightnessIndex:
      cycleBrightness();
      return;
    case kSettingsDisplayPhantomWordsIndex:
      togglePhantomWords(nowMs);
      return;
    case kSettingsDisplayFontSizeIndex:
      cycleReaderFontSize(nowMs);
      return;
    case kSettingsDisplayTypographyIndex:
      openTypographyTuning();
      return;
    case kSettingsDisplayFlipIndex:
      toggleDisplayFlip(nowMs);
      return;
    case kSettingsDisplayAutoPowerOffIndex:
      autoPowerOffIndex_ = (autoPowerOffIndex_ + 1) % kAutoPowerOffOptionCount;
      preferences_.putUChar(kPrefAutoPowerOff, autoPowerOffIndex_);
      lastActivityMs_ = nowMs;  // start counting fresh
      rebuildSettingsMenuItems();
      renderSettings();
      return;
    default:
      return;
    }
  }

  if (menuScreen_ != MenuScreen::SettingsPacing)
  {
    return;
  }

  switch (settingsSelectedIndex_)
  {
  case kSettingsBackIndex:
    settingsSelectedIndex_ = kSettingsHomePacingIndex;
    menuScreen_ = MenuScreen::SettingsHome;
    rebuildSettingsMenuItems();
    renderSettings();
    return;
  case kSettingsPacingLongWordsIndex:
    pacingLongWordLevelIndex_ =
        static_cast<uint8_t>((pacingLongWordLevelIndex_ + 1) % kPacingLevelCount);
    preferences_.putUChar(kPrefPacingLong, pacingLongWordLevelIndex_);
    break;
  case kSettingsPacingComplexityIndex:
    pacingComplexWordLevelIndex_ =
        static_cast<uint8_t>((pacingComplexWordLevelIndex_ + 1) % kPacingLevelCount);
    preferences_.putUChar(kPrefPacingComplex, pacingComplexWordLevelIndex_);
    break;
  case kSettingsPacingPunctuationIndex:
    pacingPunctuationLevelIndex_ =
        static_cast<uint8_t>((pacingPunctuationLevelIndex_ + 1) % kPacingLevelCount);
    preferences_.putUChar(kPrefPacingPunctuation, pacingPunctuationLevelIndex_);
    break;
  case kSettingsPacingResetIndex:
    pacingLongWordLevelIndex_ = kDefaultPacingLevelIndex;
    pacingComplexWordLevelIndex_ = kDefaultPacingLevelIndex;
    pacingPunctuationLevelIndex_ = kDefaultPacingLevelIndex;
    preferences_.putUChar(kPrefPacingLong, pacingLongWordLevelIndex_);
    preferences_.putUChar(kPrefPacingComplex, pacingComplexWordLevelIndex_);
    preferences_.putUChar(kPrefPacingPunctuation, pacingPunctuationLevelIndex_);
    break;
  default:
    return;
  }

  applyPacingSettings();
  rebuildSettingsMenuItems();
  renderSettings();
}

void App::openTypographyTuning()
{
  if (typographyTuningSelectedIndex_ >= TypographyTuningItemCount)
  {
    typographyTuningSelectedIndex_ = TypographyTuningTracking;
  }
  if (typographyTuningSelectedIndex_ == TypographyTuningBack)
  {
    typographyTuningSelectedIndex_ = TypographyTuningTracking;
  }
  menuScreen_ = MenuScreen::TypographyTuning;
  renderTypographyTuning();
}

void App::selectTypographyTuningItem(uint32_t nowMs)
{
  switch (typographyTuningSelectedIndex_)
  {
  case TypographyTuningBack:
    settingsSelectedIndex_ = kSettingsHomeTypographyIndex;
    menuScreen_ = MenuScreen::SettingsHome;
    rebuildSettingsMenuItems();
    renderSettings();
    return;
  case TypographyTuningTracking:
    typographyConfig_.trackingPx = static_cast<int8_t>(
        nextCyclicSetting(typographyConfig_.trackingPx, kTypographyTrackingMin,
                          kTypographyTrackingMax));
    preferences_.putChar(kPrefTypographyTracking, typographyConfig_.trackingPx);
    break;
  case TypographyTuningAnchor:
    typographyConfig_.anchorPercent = static_cast<uint8_t>(
        nextCyclicSetting(typographyConfig_.anchorPercent, kTypographyAnchorMin,
                          kTypographyAnchorMax));
    preferences_.putUChar(kPrefTypographyAnchor, typographyConfig_.anchorPercent);
    break;
  case TypographyTuningGuideWidth:
    typographyConfig_.guideHalfWidth = static_cast<uint8_t>(nextCyclicSetting(
        typographyConfig_.guideHalfWidth, kTypographyGuideWidthMin,
        kTypographyGuideWidthMax, kTypographyGuideWidthStep));
    preferences_.putUChar(kPrefTypographyGuideWidth, typographyConfig_.guideHalfWidth);
    break;
  case TypographyTuningGuideGap:
    typographyConfig_.guideGap = static_cast<uint8_t>(nextCyclicSetting(
        typographyConfig_.guideGap, kTypographyGuideGapMin, kTypographyGuideGapMax));
    preferences_.putUChar(kPrefTypographyGuideGap, typographyConfig_.guideGap);
    break;
  case TypographyTuningReset:
    typographyConfig_ = defaultTypographyConfig();
    preferences_.putChar(kPrefTypographyTracking, typographyConfig_.trackingPx);
    preferences_.putUChar(kPrefTypographyAnchor, typographyConfig_.anchorPercent);
    preferences_.putUChar(kPrefTypographyGuideWidth, typographyConfig_.guideHalfWidth);
    preferences_.putUChar(kPrefTypographyGuideGap, typographyConfig_.guideGap);
    break;
  default:
    return;
  }

  applyTypographySettings(nowMs);
}

void App::cycleTypographyPreviewSample(int direction)
{
  if (kTypographyPreviewWordCount == 0 || direction == 0)
  {
    return;
  }

  const int current = static_cast<int>(typographyPreviewSampleIndex_);
  int next = current + direction;
  if (next < 0)
  {
    next = static_cast<int>(kTypographyPreviewWordCount) - 1;
  }
  else if (next >= static_cast<int>(kTypographyPreviewWordCount))
  {
    next = 0;
  }
  typographyPreviewSampleIndex_ = static_cast<size_t>(next);
  renderTypographyTuning();
}

void App::rebuildSettingsMenuItems()
{
  settingsMenuItems_.clear();
  settingsMenuItems_.reserve(SettingsItemCount);
  if (menuScreen_ == MenuScreen::SettingsHome)
  {
    settingsMenuItems_.push_back("Back");
    settingsMenuItems_.push_back("Display");
    settingsMenuItems_.push_back("Typography tune");
    settingsMenuItems_.push_back("Word pacing");
    settingsMenuItems_.push_back(String("Sound: ") + (soundEnabled_ ? "On" : "Off"));
    settingsMenuItems_.push_back(String("Notifications: ") +
                                 (notificationsEnabled_ ? "On" : "Off"));
    settingsMenuItems_.push_back(String("Tone (") + currentNotificationToneLabel() + ")");
    settingsMenuItems_.push_back(String("Volume: ") + String(notificationVolume_) + "%");
    settingsMenuItems_.push_back("Reading sounds");
    settingsMenuItems_.push_back("Remount SD");
    String netLabel = "Network";
    if (!preferredWifiSsid_.isEmpty())
    {
      netLabel += ": " + preferredWifiSsid_;
    }
    else if (!ota_.config().networks.empty())
    {
      netLabel += ": " + ota_.config().networks.front().ssid;
    }
    settingsMenuItems_.push_back(netLabel);
    String savLabel;
    if (screensaverIndex_ == 0 || screensaverIndex_ >= kScreensaverOptionCount) {
      savLabel = "Off";
    } else {
      const uint16_t mins = kScreensaverMinutes[screensaverIndex_];
      savLabel = (mins >= 60) ? (String(mins / 60) + "h") : (String(mins) + "m");
    }
    settingsMenuItems_.push_back(String("Screensaver: ") + savLabel);
    settingsMenuItems_.push_back("Demos");
  }
  else if (menuScreen_ == MenuScreen::SettingsReadingSounds)
  {
    settingsMenuItems_.push_back("Back");
    settingsMenuItems_.push_back(String("End of chapter: ") +
                                 (chapterChimeEnabled_ ? "On" : "Off"));
    settingsMenuItems_.push_back(String("End of paragraph: ") +
                                 (paragraphChimeEnabled_ ? "On" : "Off"));
    settingsMenuItems_.push_back(String("End of page: ") +
                                 (pageChimeEnabled_ ? "On" : "Off"));
  }
  else if (menuScreen_ == MenuScreen::SettingsDisplay)
  {
    settingsMenuItems_.push_back("Back");
    settingsMenuItems_.push_back("Theme: " + themeModeLabel());
    settingsMenuItems_.push_back("Brightness: " + String(currentBrightnessPercent()) + "%");
    settingsMenuItems_.push_back("Phantom words: " + phantomWordsLabel());
    settingsMenuItems_.push_back("Font size: " + readerFontSizeLabel());
    settingsMenuItems_.push_back("Typography tune");
    settingsMenuItems_.push_back(String("Flip display: ") + (displayFlipped_ ? "On" : "Off"));
    String autoLabel;
    if (autoPowerOffIndex_ == 0 || autoPowerOffIndex_ >= kAutoPowerOffOptionCount)
    {
      autoLabel = "Off";
    }
    else
    {
      const uint16_t mins = kAutoPowerOffMinutes[autoPowerOffIndex_];
      autoLabel = (mins >= 60) ? (String(mins / 60) + "h") : (String(mins) + "m");
    }
    settingsMenuItems_.push_back(String("Auto off: ") + autoLabel);
  }
  else if (menuScreen_ == MenuScreen::SettingsPacing || menuScreen_ == MenuScreen::SettingsReadingSounds)
  {
    settingsMenuItems_.push_back("Back");
    settingsMenuItems_.push_back("Long words: " + pacingLevelLabel(pacingLongWordLevelIndex_));
    settingsMenuItems_.push_back("Complexity: " + pacingLevelLabel(pacingComplexWordLevelIndex_));
    settingsMenuItems_.push_back("Punctuation: " + pacingLevelLabel(pacingPunctuationLevelIndex_));
    settingsMenuItems_.push_back("Reset pacing");
  }

  if (settingsSelectedIndex_ >= settingsMenuItems_.size())
  {
    settingsSelectedIndex_ = kSettingsBackIndex;
  }
}

void App::applyPacingSettings()
{
  ReadingLoop::PacingConfig pacingConfig;
  pacingConfig.longWordScalePercent = kPacingScalePercents[pacingLongWordLevelIndex_];
  pacingConfig.complexWordScalePercent = kPacingScalePercents[pacingComplexWordLevelIndex_];
  pacingConfig.punctuationScalePercent = kPacingScalePercents[pacingPunctuationLevelIndex_];
  reader_.setPacingConfig(pacingConfig);

  Serial.printf("[settings] pacing long=%s complexity=%s punctuation=%s\n",
                pacingLevelLabel(pacingLongWordLevelIndex_).c_str(),
                pacingLevelLabel(pacingComplexWordLevelIndex_).c_str(),
                pacingLevelLabel(pacingPunctuationLevelIndex_).c_str());
}

String App::pacingLevelLabel(uint8_t levelIndex) const
{
  if (levelIndex >= kPacingLevelCount)
  {
    levelIndex = kDefaultPacingLevelIndex;
  }
  return kPacingScaleLabels[levelIndex];
}

String App::themeModeLabel() const
{
  if (nightMode_)
  {
    return "Night";
  }
  return darkMode_ ? "Dark" : "Light";
}

String App::phantomWordsLabel() const { return phantomWordsEnabled_ ? "On" : "Off"; }

String App::readerFontSizeLabel() const
{
  uint8_t levelIndex = readerFontSizeIndex_;
  if (levelIndex >= kReaderFontSizeCount)
  {
    levelIndex = 0;
  }
  return kReaderFontSizeLabels[levelIndex];
}

String App::typographyTuningLabel() const
{
  switch (typographyTuningSelectedIndex_)
  {
  case TypographyTuningBack:
    return "Back";
  case TypographyTuningTracking:
    return "Tracking";
  case TypographyTuningAnchor:
    return "Anchor";
  case TypographyTuningGuideWidth:
    return "Guide width";
  case TypographyTuningGuideGap:
    return "Guide gap";
  case TypographyTuningReset:
    return "Reset";
  default:
    return "Typography";
  }
}

String App::typographyTuningValueLabel() const
{
  switch (typographyTuningSelectedIndex_)
  {
  case TypographyTuningBack:
    return "tap to exit";
  case TypographyTuningTracking:
    return String(typographyConfig_.trackingPx >= 0 ? "+" : "") +
           String(static_cast<int>(typographyConfig_.trackingPx)) + " px";
  case TypographyTuningAnchor:
    return String(static_cast<unsigned int>(typographyConfig_.anchorPercent)) + "%";
  case TypographyTuningGuideWidth:
    return String(static_cast<unsigned int>(typographyConfig_.guideHalfWidth)) + " px";
  case TypographyTuningGuideGap:
    return String(static_cast<unsigned int>(typographyConfig_.guideGap)) + " px";
  case TypographyTuningReset:
    return "tap to reset";
  default:
    return "";
  }
}

void App::openAuthorPicker()
{
  authorMenuItems_.clear();
  authorPickerNames_.clear();

  const size_t count = storage_.bookCount();
  std::vector<String> authors;
  std::vector<size_t> counts;
  authors.reserve(count);
  counts.reserve(count);

  for (size_t i = 0; i < count; ++i)
  {
    if ((i & 0x07) == 0)
    {
      yield();
    }
    String name = storage_.bookAuthorName(i);
    name.trim();
    if (name.isEmpty())
    {
      name = kAuthorUnknownLabel;
    }
    bool found = false;
    for (size_t j = 0; j < authors.size(); ++j)
    {
      if (authors[j].equalsIgnoreCase(name))
      {
        counts[j] += 1;
        found = true;
        break;
      }
    }
    if (!found)
    {
      authors.push_back(name);
      counts.push_back(1);
    }
  }

  std::vector<size_t> order;
  order.reserve(authors.size());
  for (size_t i = 0; i < authors.size(); ++i)
  {
    order.push_back(i);
  }
  std::sort(order.begin(), order.end(), [&](size_t a, size_t b)
            {
    String left = authors[a];
    String right = authors[b];
    left.toLowerCase();
    right.toLowerCase();
    return left < right; });

  DisplayManager::LibraryItem backItem;
  backItem.title = "Back";
  authorMenuItems_.push_back(backItem);
  authorPickerNames_.push_back("");

  DisplayManager::LibraryItem allBooks;
  allBooks.title = kAuthorAllBooksLabel;
  allBooks.subtitle = "";
  allBooks.badges.push_back(String(static_cast<unsigned int>(count)) + " books");
  authorMenuItems_.push_back(allBooks);
  authorPickerNames_.push_back("");

  for (size_t idx : order)
  {
    DisplayManager::LibraryItem item;
    item.title = authors[idx];
    item.subtitle = "";
    item.badges.push_back(String(static_cast<unsigned int>(counts[idx])) +
                          (counts[idx] == 1 ? " book" : " books"));
    authorMenuItems_.push_back(item);
    authorPickerNames_.push_back(authors[idx]);
  }

  menuScreen_ = MenuScreen::AuthorPicker;
  if (authorPickerSelectedIndex_ >= authorMenuItems_.size())
  {
    authorPickerSelectedIndex_ = authorMenuItems_.empty() ? 0 : authorMenuItems_.size() - 1;
  }
  if (authorMenuItems_.size() > kAuthorPickerAllBooksIndex &&
      authorPickerSelectedIndex_ == kAuthorPickerBackIndex)
  {
    authorPickerSelectedIndex_ = kAuthorPickerAllBooksIndex;
  }
  computeLetterAnchors(authorMenuItems_, kAuthorPickerFirstAuthorIndex,
                       authorPickerLetterAnchors_, authorPickerLetterTargets_);
  renderAuthorPicker();
}

void App::renderAuthorPicker()
{
  const int focus = (letterScrubActive_ && menuScreen_ == MenuScreen::AuthorPicker)
                        ? letterScrubFocusIdx_
                        : -1;
  display_.renderLibrary(authorMenuItems_, authorPickerSelectedIndex_,
                         authorPickerLetterAnchors_, focus);
}

void App::selectAuthorPickerItem(uint32_t nowMs)
{
  (void)nowMs;
  if (authorMenuItems_.empty() || authorPickerSelectedIndex_ == kAuthorPickerBackIndex)
  {
    activeAuthorFilter_ = "";
    menuScreen_ = MenuScreen::Main;
    renderMainMenu();
    return;
  }
  if (authorPickerSelectedIndex_ == kAuthorPickerAllBooksIndex)
  {
    activeAuthorFilter_ = "";
    openBookPicker();
    return;
  }
  if (authorPickerSelectedIndex_ >= authorPickerNames_.size())
  {
    renderAuthorPicker();
    return;
  }
  openBookPickerForAuthor(authorPickerNames_[authorPickerSelectedIndex_]);
}

void App::openBookPickerForAuthor(const String &author)
{
  activeAuthorFilter_ = author;
  openBookPicker();
}

void App::openBookPicker()
{
  bookMenuItems_.clear();
  bookPickerBookIndices_.clear();
  DisplayManager::LibraryItem bookBackItem;
  bookBackItem.title = "Back";
  bookMenuItems_.push_back(bookBackItem);

  const size_t count = storage_.bookCount();
  std::vector<size_t> sortedBookIndices;
  sortedBookIndices.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    // Library shows only books that originated from an EPUB (have a sibling
    // .epub file on the SD). Direct .rsvp drops are hidden — keeps the picker
    // focused on user-curated books and on those downloaded via the in-app
    // GitHub fetch path, both of which leave their .epub source on disk.
    if (!storage_.wasConvertedFromEpub(i))
    {
      continue;
    }
    if (recentOnlyFilter_)
    {
      if (bookRecentSequenceByIndex(i) == 0)
      {
        continue;
      }
    }
    else if (!activeAuthorFilter_.isEmpty())
    {
      String authorName = storage_.bookAuthorName(i);
      authorName.trim();
      if (authorName.isEmpty())
      {
        authorName = kAuthorUnknownLabel;
      }
      if (!authorName.equalsIgnoreCase(activeAuthorFilter_))
      {
        continue;
      }
    }
    sortedBookIndices.push_back(i);
  }

  std::stable_sort(sortedBookIndices.begin(), sortedBookIndices.end(),
                   [this](size_t leftIndex, size_t rightIndex)
                   {
                     const bool leftCurrent =
                         usingStorageBook_ && leftIndex == currentBookIndex_;
                     const bool rightCurrent =
                         usingStorageBook_ && rightIndex == currentBookIndex_;
                     if (leftCurrent != rightCurrent)
                     {
                       return leftCurrent;
                     }

                     const uint32_t leftRecent = bookRecentSequenceByIndex(leftIndex);
                     const uint32_t rightRecent = bookRecentSequenceByIndex(rightIndex);
                     const bool leftHasRecent = leftRecent > 0;
                     const bool rightHasRecent = rightRecent > 0;
                     if (leftHasRecent != rightHasRecent)
                     {
                       return leftHasRecent;
                     }
                     if (leftRecent != rightRecent)
                     {
                       return leftRecent > rightRecent;
                     }

                     return false;
                   });

  size_t pickerYieldCounter = 0;
  for (size_t bookIndex : sortedBookIndices)
  {
    if ((pickerYieldCounter++ & 0x07) == 0)
    {
      yield();
    }
    bookPickerBookIndices_.push_back(bookIndex);
    bookMenuItems_.push_back(libraryItemForBook(bookIndex));
  }

  if (count == 0)
  {
    Serial.println("[book-picker] No SD books available");
  }

  menuScreen_ = MenuScreen::BookPicker;
  bookPickerSelectedIndex_ = kBookPickerBackIndex;
  if (usingStorageBook_)
  {
    for (size_t row = 0; row < bookPickerBookIndices_.size(); ++row)
    {
      if (bookPickerBookIndices_[row] == currentBookIndex_)
      {
        bookPickerSelectedIndex_ = row + 1;
        break;
      }
    }
  }
  computeLetterAnchors(bookMenuItems_, 1, bookPickerLetterAnchors_, bookPickerLetterTargets_);
  renderBookPicker();
}

void App::selectBookPickerItem(uint32_t nowMs)
{
  if (bookPickerSelectedIndex_ == kBookPickerBackIndex || bookMenuItems_.size() <= 1)
  {
    if (!activeAuthorFilter_.isEmpty())
    {
      activeAuthorFilter_ = "";
      openAuthorPicker();
      return;
    }
    if (recentOnlyFilter_)
    {
      recentOnlyFilter_ = false;
    }
    menuScreen_ = MenuScreen::Main;
    renderMainMenu();
    return;
  }

  const size_t rowIndex = bookPickerSelectedIndex_ - 1;
  if (rowIndex >= bookPickerBookIndices_.size())
  {
    renderBookPicker();
    return;
  }

  const size_t bookIndex = bookPickerBookIndices_[rowIndex];
  saveReadingPosition(true);

  String loadingTitle = storage_.bookDisplayName(bookIndex);
  if (loadingTitle.isEmpty())
  {
    loadingTitle = "Book";
  }
  display_.renderProgress("Loading", loadingTitle.c_str(), "Reading from SD", 0);

  if (!loadBookAtIndex(bookIndex, nowMs))
  {
    Serial.println("[book-picker] Failed to load selected book");
    renderBookPicker();
    return;
  }

  Serial.printf("[book-picker] loaded ok: title=%s words=%u currentIndex=%u currentWord=\"%s\"\n",
                currentBookTitle_.c_str(), static_cast<unsigned>(reader_.wordCount()),
                static_cast<unsigned>(reader_.currentIndex()), reader_.currentWord().c_str());
  menuScreen_ = MenuScreen::Main;
  setState(AppState::Paused, nowMs);
  saveReadingPosition(true);
  // Force an immediate reader render rather than waiting for the next periodic
  // tick — without this the picker UI stays on screen until the next 33 ms
  // refresh, and any guard inside renderReaderWord (empty word, banner, etc.)
  // leaves the screen showing whatever was last drawn.
  renderReaderWord();
}

void App::openChapterPicker()
{
  chapterMenuItems_.clear();
  chapterMenuItems_.push_back("Back");

  if (chapterMarkers_.empty())
  {
    chapterMenuItems_.push_back("Start of book");
    chapterPickerSelectedIndex_ = kChapterPickerFallbackIndex;
    Serial.println("[chapter-picker] No chapter markers found; showing start fallback");
  }
  else
  {
    for (size_t i = 0; i < chapterMarkers_.size(); ++i)
    {
      chapterMenuItems_.push_back(chapterMenuLabel(i));
    }

    size_t selectedChapter = 0;
    const size_t currentWordIndex = reader_.currentIndex();
    for (size_t i = 0; i < chapterMarkers_.size(); ++i)
    {
      if (chapterMarkers_[i].wordIndex <= currentWordIndex)
      {
        selectedChapter = i;
      }
    }
    chapterPickerSelectedIndex_ = selectedChapter + 1;
  }

  menuScreen_ = MenuScreen::ChapterPicker;
  renderChapterPicker();
}

void App::selectChapterPickerItem(uint32_t nowMs)
{
  if (chapterPickerSelectedIndex_ == kChapterPickerBackIndex || chapterMenuItems_.size() <= 1)
  {
    menuScreen_ = MenuScreen::Main;
    renderMainMenu();
    return;
  }

  if (chapterMarkers_.empty())
  {
    reader_.seekTo(0);
    menuScreen_ = MenuScreen::Main;
    setState(AppState::Paused, nowMs);
    saveReadingPosition(true);
    Serial.println("[chapter-picker] jumped to start of book");
    return;
  }

  const size_t chapterIndex = chapterPickerSelectedIndex_ - 1;
  if (chapterIndex >= chapterMarkers_.size())
  {
    renderChapterPicker();
    return;
  }

  reader_.seekTo(chapterMarkers_[chapterIndex].wordIndex);
  menuScreen_ = MenuScreen::Main;
  setState(AppState::Paused, nowMs);
  saveReadingPosition(true);
  Serial.printf("[chapter-picker] jumped to %s at word %u\n",
                chapterMarkers_[chapterIndex].title.c_str(),
                static_cast<unsigned int>(chapterMarkers_[chapterIndex].wordIndex));
}

void App::openRestartConfirm()
{
  restartConfirmSelectedIndex_ = RestartConfirmNo;
  menuScreen_ = MenuScreen::RestartConfirm;
  renderRestartConfirm();
}

void App::selectRestartConfirmItem(uint32_t nowMs)
{
  (void)nowMs;
  if (restartConfirmSelectedIndex_ != RestartConfirmYes)
  {
    menuScreen_ = MenuScreen::Main;
    renderMainMenu();
    return;
  }

  // Save current reading position so the boot path can restore it, then
  // hand off to esp_restart. The boot flow already reads kPrefBookPath +
  // kPrefBookPosition and seeks the reader, so the user lands exactly where
  // they were.
  saveReadingPosition(true);
  display_.renderStatus("Rebooting", "Saving place", "");
  delay(150);  // give the LCD a chance to flush before the chip resets
  Serial.println("[reboot] user requested device reboot, position saved");
  ESP.restart();
}

void App::enterUsbTransfer(uint32_t nowMs)
{
  Serial.println("[app] entering USB transfer mode");
  saveReadingPosition(true);
  pausedTouch_.active = false;
  pausedTouchIntent_ = TouchIntent::None;
  wpmFeedbackVisible_ = false;
  setState(AppState::UsbTransfer, nowMs);

  storage_.end();
  if (!usbTransfer_.begin(true))
  {
    Serial.printf("[app] USB transfer failed: %s\n", usbTransfer_.statusMessage());
    display_.renderStatus("USB", "SD not ready", "Returning");
    const bool storageReady = storage_.begin();
    if (storageReady)
    {
      storage_.listBooks();
    }
    setState(AppState::Paused, nowMs);
    return;
  }

  const uint64_t sizeMb = usbTransfer_.cardSizeBytes() / (1024ULL * 1024ULL);
  Serial.printf("[app] USB transfer active (%llu MB). Eject from computer when finished.\n",
                sizeMb);
  display_.renderStatus("USB", "Copy books now", "Eject then hold BOOT");
}

void App::updateUsbTransfer(uint32_t nowMs)
{
  if (!usbTransfer_.active())
  {
    return;
  }

  bool serialExitRequested = false;
  static String serialBuffer;
  while (Serial.available() > 0)
  {
    const char c = static_cast<char>(Serial.read());
    if (c == '\r')
    {
      continue;
    }
    if (c == '\n')
    {
      serialBuffer.trim();
      if (serialBuffer.equalsIgnoreCase("EXIT-MSC"))
      {
        serialExitRequested = true;
        Serial.println("[app] EXIT-MSC received over serial");
      }
      serialBuffer = "";
      continue;
    }
    serialBuffer += c;
    if (serialBuffer.length() > 64)
    {
      serialBuffer = "";
    }
  }

  const bool bootExitRequested =
      button_.isHeld() && nowMs - button_.lastEdgeMs() >= kUsbTransferExitHoldMs;
  if (!usbTransfer_.ejected() && !bootExitRequested && !serialExitRequested)
  {
    return;
  }

  if ((bootExitRequested || serialExitRequested) && !usbTransfer_.ejected())
  {
    Serial.println("[app] leaving USB transfer; assuming host writes are flushed");
  }

  exitUsbTransfer(nowMs);
}

void App::exitUsbTransfer(uint32_t nowMs)
{
  Serial.println("[app] USB transfer ejected; remounting SD");
  display_.renderStatus("USB", "Remounting SD", "");
  usbTransfer_.end();

  const bool storageReady = storage_.begin();
  if (storageReady)
  {
    storage_.listBooks();
    const int refreshedBookIndex = findBookIndexByPath(currentBookPath_);
    if (refreshedBookIndex >= 0)
    {
      currentBookIndex_ = static_cast<size_t>(refreshedBookIndex);
    }
    else if (storage_.bookCount() > 0)
    {
      loadBookAtIndex(0, nowMs);
    }
  }
  else
  {
    Serial.println("[app] SD remount failed after USB transfer");
  }

  menuScreen_ = MenuScreen::Main;
  setState(AppState::Paused, nowMs);
}

void App::pollNotifications(uint32_t nowMs)
{
  if (!notifications_.poll())
  {
    return;
  }
  auto pending = notifications_.drainPending();
  if (pending.empty())
    return;
  preferences_.putUInt(kPrefNotifLastTs, notifications_.lastSeenTs());
  // Show only the most recent one as a banner (queue collapses to most-recent so
  // the user always sees the freshest news).
  const auto &latest = pending.back();
  showNotificationBanner(nowMs, latest.title, latest.body);
}

void App::showNotificationBanner(uint32_t nowMs, const String &title, const String &body)
{
  notificationBannerTitle_ = title;
  notificationBannerBody_ = body;
  // Sentinel: any non-zero value means "banner active". We dismiss explicitly on tap.
  notificationBannerUntilMs_ = nowMs == 0 ? 1 : nowMs;
  display_.renderStatus(title.isEmpty() ? "Notification" : title,
                        body, "Tap to dismiss");
  playNotificationTone();
}

void App::playNotificationTone(uint32_t maxDurationMs)
{
  // Master mute — Sound: Off in Settings short-circuits every audio path
  // (volume preview, notifications, reading-sound chimes).
  if (!soundEnabled_)
    return;
  if (notificationVolume_ == 0)
    return;
  audio_.setVolumePercent(notificationVolume_);
  if (notificationTone_.isEmpty())
  {
    audio_.playRtttl(kBuiltinNokiaRtttl, maxDurationMs);
    return;
  }
  String lowered = notificationTone_;
  lowered.toLowerCase();
  if (lowered.endsWith(".wav"))
  {
    if (!audio_.playWavFromSd(storage_.ringtonePath(notificationTone_), maxDurationMs))
    {
      audio_.playRtttl(kBuiltinNokiaRtttl, maxDurationMs); // fallback
    }
    return;
  }
  String rtttl;
  if (storage_.loadRingtone(notificationTone_, rtttl))
  {
    audio_.playRtttl(rtttl, maxDurationMs);
  }
  else
  {
    audio_.playRtttl(kBuiltinNokiaRtttl, maxDurationMs);
  }
}

String App::currentNotificationToneLabel() const
{
  if (notificationTone_.isEmpty())
    return "Nokia (built-in)";
  // Strip the extension for the menu label.
  const int dot = notificationTone_.lastIndexOf('.');
  if (dot > 0)
    return notificationTone_.substring(0, dot);
  return notificationTone_;
}

namespace
{

  constexpr size_t kTonePickerBackIndex = 0;
  constexpr size_t kTonePickerBuiltinIndex = 1;
  constexpr size_t kTonePickerFirstSdIndex = 2;

  String toneFormatBadgeForFilename(const String &filename)
  {
    String lowered = filename;
    lowered.toLowerCase();
    if (lowered.endsWith(".wav"))
      return "WAV";
    if (lowered.endsWith(".rtttl"))
      return "RTTTL";
    return "?";
  }

  String toneDisplayNameForFilename(const String &filename)
  {
    const int dot = filename.lastIndexOf('.');
    if (dot > 0)
      return filename.substring(0, dot);
    return filename;
  }

} // namespace

void App::openTonePicker()
{
  ringtoneNames_ = storage_.listRingtoneNames();

  toneMenuItems_.clear();
  tonePickerNames_.clear();

  DisplayManager::LibraryItem backItem;
  backItem.title = "Back";
  toneMenuItems_.push_back(backItem);
  tonePickerNames_.push_back("");

  DisplayManager::LibraryItem builtIn;
  builtIn.title = "Nokia";
  builtIn.subtitle = "Built-in";
  builtIn.badges.push_back("RTTTL");
  toneMenuItems_.push_back(builtIn);
  tonePickerNames_.push_back(""); // empty filename = built-in fallback

  for (const String &filename : ringtoneNames_)
  {
    DisplayManager::LibraryItem item;
    item.title = toneDisplayNameForFilename(filename);
    item.subtitle = "";
    item.badges.push_back(toneFormatBadgeForFilename(filename));
    toneMenuItems_.push_back(item);
    tonePickerNames_.push_back(filename);
  }

  // Highlight whichever tone is currently selected.
  tonePickerSelectedIndex_ = notificationTone_.isEmpty() ? kTonePickerBuiltinIndex : 0;
  for (size_t i = kTonePickerFirstSdIndex; i < tonePickerNames_.size(); ++i)
  {
    if (tonePickerNames_[i] == notificationTone_)
    {
      tonePickerSelectedIndex_ = i;
      break;
    }
  }

  menuScreen_ = MenuScreen::TonePicker;
  renderTonePicker();
}

void App::renderTonePicker()
{
  display_.renderLibrary(toneMenuItems_, tonePickerSelectedIndex_);
}

void App::selectTonePickerItem(uint32_t nowMs)
{
  (void)nowMs;
  if (tonePickerSelectedIndex_ == kTonePickerBackIndex ||
      tonePickerSelectedIndex_ >= tonePickerNames_.size())
  {
    settingsSelectedIndex_ = kSettingsHomeToneIndex;
    menuScreen_ = MenuScreen::SettingsHome;
    rebuildSettingsMenuItems();
    renderSettings();
    return;
  }
  notificationTone_ = tonePickerNames_[tonePickerSelectedIndex_];
  preferences_.putString(kPrefNotifTone, notificationTone_);
  // Pure select-and-return: no playback. Sound only fires on volume change
  // (UI click) and on actual notifications — never as menu noise.
  settingsSelectedIndex_ = kSettingsHomeToneIndex;
  menuScreen_ = MenuScreen::SettingsHome;
  rebuildSettingsMenuItems();
  renderSettings();
}

namespace
{
constexpr size_t kRemoteBookPickerBackIndex = 0;
constexpr size_t kRemoteBookPickerFirstBookIndex = 1;
}  // namespace

void App::applyDefaultNetwork()
{
  // Reorder ota_.config().networks so the user's preferred SSID is index 0,
  // then push the new order into the other managers. WiFi connect attempts
  // walk the list head-first, so this is what makes "default" mean default.
  OtaManager::Config cfg = ota_.config();
  if (!preferredWifiSsid_.isEmpty())
  {
    auto it = std::find_if(cfg.networks.begin(), cfg.networks.end(),
                           [this](const OtaManager::Network &n) {
                             return n.ssid == preferredWifiSsid_;
                           });
    if (it != cfg.networks.end() && it != cfg.networks.begin())
    {
      OtaManager::Network preferred = *it;
      cfg.networks.erase(it);
      cfg.networks.insert(cfg.networks.begin(), preferred);
      ota_.setConfig(cfg);
    }
  }
  notifications_.configure(cfg.networks, cfg.notificationsUrl, cfg.notificationsToken);
  bookDownloader_.configure(cfg.networks, "ccpaging", "books");
}

void App::openNetworkPicker()
{
  networkMenuItems_.clear();
  const auto &cfg = ota_.config();
  DisplayManager::LibraryItem backItem;
  backItem.title = "Back";
  networkMenuItems_.push_back(backItem);
  size_t selectedAt = 0;
  for (size_t i = 0; i < cfg.networks.size(); ++i)
  {
    DisplayManager::LibraryItem item;
    item.title = cfg.networks[i].ssid;
    if (cfg.networks[i].ssid == preferredWifiSsid_ ||
        (preferredWifiSsid_.isEmpty() && i == 0))
    {
      item.badges.push_back("DEFAULT");
      selectedAt = i + 1;  // +1 for the Back row
    }
    networkMenuItems_.push_back(item);
  }
  networkSelectedIndex_ = selectedAt > 0 ? selectedAt : 0;
  menuScreen_ = MenuScreen::NetworkPicker;
  renderNetworkPicker();
}

void App::renderNetworkPicker()
{
  display_.renderLibrary(networkMenuItems_, networkSelectedIndex_);
}

void App::selectNetworkPickerItem(uint32_t nowMs)
{
  (void)nowMs;
  if (networkSelectedIndex_ == 0)
  {
    settingsSelectedIndex_ = kSettingsHomeNetworkIndex;
    menuScreen_ = MenuScreen::SettingsHome;
    rebuildSettingsMenuItems();
    renderSettings();
    return;
  }
  const auto &cfg = ota_.config();
  const size_t netIdx = networkSelectedIndex_ - 1;
  if (netIdx >= cfg.networks.size()) return;
  preferredWifiSsid_ = cfg.networks[netIdx].ssid;
  preferences_.putString(kPrefPreferredWifi, preferredWifiSsid_);
  applyDefaultNetwork();
  // Rebuild the picker so the DEFAULT badge reflects the new pick, then bounce
  // back to Settings so the user sees it land.
  settingsSelectedIndex_ = kSettingsHomeNetworkIndex;
  menuScreen_ = MenuScreen::SettingsHome;
  rebuildSettingsMenuItems();
  renderSettings();
}

void App::openRemoteBookPicker(uint32_t nowMs)
{
  (void)nowMs;
  display_.renderProgress("Books", "Connecting WiFi", "ccpaging/books", 5);
  remoteBooks_.clear();
  remoteBookMenuItems_.clear();
  if (!bookDownloader_.listAvailable(remoteBooks_))
  {
    display_.renderStatus("Books", "Index failed",
                          bookDownloader_.lastError().c_str());
    delay(1500);
    menuScreen_ = MenuScreen::Main;
    renderMainMenu();
    return;
  }

  DisplayManager::LibraryItem backItem;
  backItem.title = "Back";
  remoteBookMenuItems_.push_back(backItem);
  for (const auto &book : remoteBooks_)
  {
    DisplayManager::LibraryItem item;
    item.title = book.displayName;
    if (book.sizeBytes > 0)
    {
      item.badges.push_back(String(book.sizeBytes / 1024) + "K");
    }
    remoteBookMenuItems_.push_back(item);
  }
  remoteBookSelectedIndex_ = remoteBooks_.empty() ? kRemoteBookPickerBackIndex
                                                  : kRemoteBookPickerFirstBookIndex;
  menuScreen_ = MenuScreen::RemoteBookPicker;
  renderRemoteBookPicker();
}

void App::renderRemoteBookPicker()
{
  display_.renderLibrary(remoteBookMenuItems_, remoteBookSelectedIndex_);
}

void App::selectRemoteBookPickerItem(uint32_t nowMs)
{
  if (remoteBookSelectedIndex_ == kRemoteBookPickerBackIndex)
  {
    menuScreen_ = MenuScreen::Main;
    renderMainMenu();
    return;
  }
  const size_t bookIdx = remoteBookSelectedIndex_ - kRemoteBookPickerFirstBookIndex;
  if (bookIdx >= remoteBooks_.size())
  {
    return;
  }
  const auto &book = remoteBooks_[bookIdx];
  // Drop the original directory prefix so everything lands flat under /books.
  String filename = book.path;
  const int slash = filename.lastIndexOf('/');
  if (slash >= 0) filename = filename.substring(slash + 1);
  const String destPath = String("/books/") + filename;

  if (!bookDownloader_.download(book, destPath))
  {
    display_.renderStatus("Books", "Download failed",
                          bookDownloader_.lastError().c_str());
    delay(1500);
    renderRemoteBookPicker();
    return;
  }
  display_.renderProgress("Books", "Converting", filename.c_str(), 95);
  String rsvpPath;
  if (!storage_.ensureEpubConverted(destPath, rsvpPath))
  {
    display_.renderStatus("Books", "Convert failed", filename.c_str());
    delay(1500);
    renderRemoteBookPicker();
    return;
  }
  // If the converted .rsvp is too big to fit in RAM, split it into multiple
  // sibling files (.part1.rsvp / .part2.rsvp / …). Each part lands as its own
  // entry in the library so the user can read GEB-sized books in chunks.
  storage_.splitOversizedRsvp(rsvpPath);
  // Refresh in-memory book list so the picker can find the new title(s).
  storage_.listBooks();
  bookProgressInfo_.clear();
  display_.renderStatus("Books", "Saved", filename.c_str());
  delay(900);
  menuScreen_ = MenuScreen::Main;
  renderMainMenu();
  (void)nowMs;
}

void App::openBookDeleteConfirm(size_t bookIndex)
{
  bookDeleteIndex_ = bookIndex;
  bookDeleteTitle_ = storage_.bookDisplayName(bookIndex);
  bookDeleteSelectedIndex_ = BookDeleteCancel;  // default to safe option
  menuScreen_ = MenuScreen::BookDeleteConfirm;
  renderBookDeleteConfirm();
}

void App::renderBookDeleteConfirm()
{
  std::vector<String> items;
  items.reserve(BookDeleteOptionCount + 1);
  // Header row shows the title so the user knows exactly what they're nuking.
  items.push_back(bookDeleteTitle_.isEmpty() ? String("Delete book?") : bookDeleteTitle_);
  items.push_back(String("No"));
  items.push_back(String("Yes, delete"));
  display_.renderMenu(items, bookDeleteSelectedIndex_ + kBookDeleteConfirmHeaderRows);
}

void App::selectBookDeleteConfirmItem(uint32_t nowMs)
{
  (void)nowMs;
  if (bookDeleteSelectedIndex_ != BookDeleteYes)
  {
    menuScreen_ = MenuScreen::BookPicker;
    renderBookPicker();
    return;
  }
  display_.renderStatus("Library", "Deleting", bookDeleteTitle_.c_str());
  if (storage_.deleteBookAtIndex(bookDeleteIndex_))
  {
    bookProgressInfo_.clear();
    // If we just deleted the currently-loaded book, drop reader state so the
    // user lands cleanly on Library next time.
    if (usingStorageBook_ && currentBookIndex_ == bookDeleteIndex_)
    {
      usingStorageBook_ = false;
      bookMetaOnly_ = false;
      currentBookPath_ = "";
      currentBookTitle_ = "";
    }
  }
  // Re-open the picker so the row is gone from the list.
  if (!activeAuthorFilter_.isEmpty())
  {
    openBookPickerForAuthor(activeAuthorFilter_);
  }
  else
  {
    openBookPicker();
  }
}

void App::openDemoPicker()
{
  menuScreen_ = MenuScreen::DemoPicker;
  if (demoSelectedIndex_ >= kDemoPickerItemCount) demoSelectedIndex_ = 0;
  renderDemoPicker();
}

void App::renderDemoPicker()
{
  std::vector<String> items;
  items.reserve(kDemoPickerItemCount);
  items.push_back("Back");
  items.push_back("Rasterbars");
  items.push_back("Starfield");
  items.push_back("Sine-scroller");
  items.push_back("Plasma");
  items.push_back("ShadeBobs");
  items.push_back("Vectorball");
  // All demo rows drill in to fullscreen playback; Back returns to settings.
  std::vector<bool> chevrons(items.size(), true);
  chevrons[kDemoPickerBackIndex] = false;
  display_.renderMenu(items, demoSelectedIndex_, chevrons);
}

void App::selectDemoPickerItem(uint32_t nowMs)
{
  switch (demoSelectedIndex_)
  {
  case kDemoPickerBackIndex:
    settingsSelectedIndex_ = kSettingsHomeDemosIndex;
    menuScreen_ = MenuScreen::SettingsHome;
    rebuildSettingsMenuItems();
    renderSettings();
    return;
  case kDemoPickerRasterbarsIndex:
    enterDemoPlayback(DemoKind::Rasterbars, nowMs);
    return;
  case kDemoPickerStarfieldIndex:
    enterDemoPlayback(DemoKind::Starfield, nowMs);
    return;
  case kDemoPickerSineScrollerIndex:
    enterDemoPlayback(DemoKind::SineScroller, nowMs);
    return;
  case kDemoPickerPlasmaIndex:
    enterDemoPlayback(DemoKind::Plasma, nowMs);
    return;
  case kDemoPickerShadeBobsIndex:
    enterDemoPlayback(DemoKind::ShadeBobs, nowMs);
    return;
  case kDemoPickerVectorballIndex:
    enterDemoPlayback(DemoKind::Vectorball, nowMs);
    return;
  default:
    return;
  }
}

void App::enterDemoPlayback(DemoKind kind, uint32_t nowMs)
{
  currentDemo_ = kind;
  // Re-use screensaver's previous-state plumbing so the touch handler can
  // restore us to wherever we were (in this case the settings menu picker).
  screensaverPreviousState_ = AppState::Menu;
  switch (kind) {
    case DemoKind::Rasterbars:    demoRasterbars_.begin(nowMs); break;
    case DemoKind::Starfield:     demoStarfield_.begin(nowMs); break;
    case DemoKind::SineScroller:  demoSineScroller_.begin(nowMs); break;
    case DemoKind::Plasma:        demoPlasma_.begin(nowMs); break;
    case DemoKind::ShadeBobs:     demoShadeBobs_.begin(nowMs); break;
    case DemoKind::Vectorball:    demoVectorball_.begin(nowMs); break;
    case DemoKind::None:          return;
  }
  setState(AppState::DemoPlaying, nowMs);
}

void App::exitDemoPlayback(uint32_t nowMs)
{
  currentDemo_ = DemoKind::None;
  setState(AppState::Menu, nowMs);
  // Land back on the demo picker so a quick re-tap selects another effect.
  menuScreen_ = MenuScreen::DemoPicker;
  renderDemoPicker();
  (void)nowMs;
}

void App::renderDemoFrame(uint32_t nowMs)
{
  // Per-demo throttle. Demos with heavier compose advertise their cap here
  // honestly — pretending to target 60 fps when SPI+compose don't fit just
  // burns CPU.
  static uint32_t sLastFrameMs = 0;
  uint32_t minIntervalMs = 16;  // 60 fps default
  if (currentDemo_ == DemoKind::Plasma) minIntervalMs = 25;  // 40 fps cap
  if (nowMs - sLastFrameMs < minIntervalMs) return;
  sLastFrameMs = nowMs;

  switch (currentDemo_) {
    case DemoKind::Rasterbars:
      demoRasterbars_.tick(nowMs);
      display_.renderRasterbarsFrame(demoRasterbars_);
      break;
    case DemoKind::Starfield:
      demoStarfield_.tick(nowMs);
      display_.renderStarfieldFrame(demoStarfield_);
      break;
    case DemoKind::SineScroller:
      demoSineScroller_.tick(nowMs);
      display_.renderSineScrollerFrame(demoSineScroller_);
      break;
    case DemoKind::Plasma:
      demoPlasma_.tick(nowMs);
      display_.renderPlasmaFrame(demoPlasma_);
      break;
    case DemoKind::ShadeBobs:
      demoShadeBobs_.tick(nowMs);
      display_.renderShadeBobsFrame(demoShadeBobs_);
      break;
    case DemoKind::Vectorball:
      demoVectorball_.tick(nowMs);
      demoVectorball_.sortBalls();
      demoVectorball_.paintFramebuffer();
      display_.renderVectorballFrame(demoVectorball_);
      break;
    case DemoKind::None:
      break;
  }
}

void App::cycleNotificationVolume(uint32_t nowMs)
{
  (void)nowMs;
  // Cycle 20→40→60→80→100→20. Muting is handled by the Notifications On/Off
  // toggle — without a min floor, a single accidental tap from 100% silently
  // lands the user on 0% with no audible feedback that the device is muted.
  constexpr uint8_t kMinAudibleVolume = 20;
  uint8_t next = notificationVolume_ + kNotificationVolumeStep;
  if (next > 100)
    next = kMinAudibleVolume;
  if (next < kMinAudibleVolume)
    next = kMinAudibleVolume;
  notificationVolume_ = next;
  preferences_.putUChar(kPrefNotifVolume, notificationVolume_);
  audio_.setVolumePercent(notificationVolume_);
  rebuildSettingsMenuItems();
  renderSettings();
  if (notificationVolume_ > 0)
  {
    playNotificationTone();
  }
}

void App::toggleNotificationsEnabled(uint32_t nowMs)
{
  (void)nowMs;
  notificationsEnabled_ = !notificationsEnabled_;
  preferences_.putBool(kPrefNotifications, notificationsEnabled_);
  notifications_.setEnabled(notificationsEnabled_);
  rebuildSettingsMenuItems();
  renderSettings();
  Serial.printf("[notif] enabled=%d\n", notificationsEnabled_ ? 1 : 0);
}

void App::renderNotificationBanner()
{
  if (notificationBannerUntilMs_ == 0)
    return;
  display_.renderStatus(
      notificationBannerTitle_.isEmpty() ? "Notification" : notificationBannerTitle_,
      notificationBannerBody_, "");
}

void App::runOtaUpdate(uint32_t nowMs)
{
  saveReadingPosition(true);
  display_.renderProgress("OTA", "Starting update", "Reading wifi.json", 0);
  if (!ota_.loadConfigFromSd())
  {
    display_.renderStatus("OTA needs setup",
                          "Create /wifi.json on SD",
                          "ssid, password, url fields");
    delay(4000);
    menuScreen_ = MenuScreen::Main;
    setState(AppState::Menu, nowMs);
    return;
  }
  // Tear down the SD bus cleanly before kicking the OTA. ota_.runUpdate()
  // ends in ESP.restart() on success; if SD_MMC is still mounted at that
  // point, the new firmware can fail its first mount attempt (see the
  // attempt-4 retry-with-delay path in StorageManager::begin()). Unmount
  // here so the bus comes up cleanly post-reboot.
  storage_.end();
  delay(50);
  const bool ok = ota_.runUpdate();
  if (!ok)
  {
    display_.renderStatus("OTA failed", ota_.lastError().c_str(), "Returning to menu");
    delay(2500);
    // Re-mount SD since runUpdate() returned without restarting — we still
    // need the library back. Use a status overlay so a slow remount is visible.
    display_.renderStatus("SD", "Re-mounting after OTA fail", "");
    if (storage_.begin()) {
      storage_.listBooks();
    }
    menuScreen_ = MenuScreen::Main;
    setState(AppState::Menu, nowMs);
  }
}

void App::enterPowerOff(uint32_t nowMs)
{
  if (powerOffStarted_)
  {
    return;
  }

  powerOffStarted_ = true;
  Serial.println("[app] powering off; hold PWR to start again");
  saveReadingPosition(true);
  pausedTouch_.active = false;
  pausedTouchIntent_ = TouchIntent::None;
  touchPlayHeld_ = false;
  contextViewVisible_ = false;
  wpmFeedbackVisible_ = false;
  menuScreen_ = MenuScreen::Main;
  state_ = AppState::Sleeping;

  display_.renderStatus("OFF", "Release PWR", "Hold PWR to start");
  delay(300);

  storage_.end();
  touch_.end();
  touchInitialized_ = false;
  Serial.flush();

  BoardConfig::releaseBatteryPowerHold();

  const uint32_t waitStartMs = millis();
  while (powerButton_.isHeld() && millis() - waitStartMs < kPowerOffReleaseWaitMs)
  {
    powerButton_.update(millis());
    delay(10);
  }

  display_.prepareForSleep();
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(BoardConfig::PIN_PWR_BUTTON), 0);
  esp_deep_sleep_start();
}

void App::enterSleep(uint32_t nowMs)
{
  Serial.println("[app] entering light sleep; press BOOT to wake");
  saveReadingPosition(true);
  setState(AppState::Sleeping, nowMs);
  Serial.flush();
  delay(200);

  display_.prepareForSleep();
  storage_.end();
  touch_.end();
  touchInitialized_ = false;

  BoardConfig::lightSleepUntilBootButton();
  wakeFromSleep();
}

void App::wakeFromSleep()
{
  const uint32_t nowMs = millis();
  Serial.println("[app] woke from light sleep");

  BoardConfig::begin();
  button_.begin();
  powerButton_.begin();
  bootButtonReleasedSinceBoot_ = !button_.isHeld();
  bootButtonLongPressHandled_ = false;
  powerButtonReleasedSinceBoot_ = !powerButton_.isHeld();
  powerButtonLongPressHandled_ = false;
  powerOffStarted_ = false;
  updateBatteryStatus(nowMs, true);
  storage_.setStatusCallback(&App::handleStorageStatus, this);
  pausedTouch_.active = false;
  pausedTouchIntent_ = TouchIntent::None;
  wpmFeedbackVisible_ = false;
  menuScreen_ = MenuScreen::Main;
  lastStateLogMs_ = nowMs;
  state_ = AppState::Paused;

  const bool displayReady = display_.wakeFromSleep();
  if (displayReady)
  {
    renderReaderWord();
  }

  touchInitialized_ = touch_.begin();
  const bool storageReady = storage_.begin();
  if (storageReady)
  {
    storage_.listBooks();
  }
}

bool App::restoreSavedBook(uint32_t nowMs)
{
  const String savedPath = preferences_.getString(kPrefBookPath, "");
  if (savedPath.isEmpty())
  {
    return false;
  }

  const int bookIndex = findBookIndexByPath(savedPath);
  if (bookIndex < 0)
  {
    Serial.printf("[app] saved book not found: %s\n", savedPath.c_str());
    return false;
  }

  if (!loadBookAtIndex(static_cast<size_t>(bookIndex), nowMs, true))
  {
    return false;
  }

  Serial.printf("[app] restored %s at word %u\n", savedPath.c_str(),
                static_cast<unsigned int>(reader_.currentIndex()));
  return true;
}

bool App::prepareSavedBookMeta()
{
  const String savedPath = preferences_.getString(kPrefBookPath, "");
  if (savedPath.isEmpty())
  {
    return false;
  }
  const int idx = findBookIndexByPath(savedPath);
  if (idx < 0)
  {
    Serial.printf("[app] saved book not found for meta-only: %s\n", savedPath.c_str());
    return false;
  }
  currentBookIndex_ = static_cast<size_t>(idx);
  currentBookPath_ = savedPath;
  currentBookTitle_ = storage_.bookDisplayName(currentBookIndex_);
  if (currentBookTitle_.isEmpty())
  {
    currentBookTitle_ = "Resume";
  }
  bookMetaOnly_ = true;
  usingStorageBook_ = false;
  Serial.printf("[app] meta-only restored: %s (idx=%u)\n", currentBookPath_.c_str(),
                static_cast<unsigned int>(currentBookIndex_));
  return true;
}

bool App::ensureCurrentBookLoaded(uint32_t nowMs)
{
  if (usingStorageBook_)
  {
    return true;
  }
  if (!bookMetaOnly_ || currentBookPath_.isEmpty())
  {
    return false;
  }
  if (loadBookAtIndex(currentBookIndex_, nowMs, true))
  {
    bookMetaOnly_ = false;
    return true;
  }
  return false;
}

void App::saveReadingPosition(bool force)
{
  if (!usingStorageBook_ || currentBookPath_.isEmpty())
  {
    return;
  }

  const size_t wordIndex = reader_.currentIndex();
  if (!force && wordIndex == lastSavedWordIndex_)
  {
    return;
  }

  preferences_.putString(kPrefBookPath, currentBookPath_);
  preferences_.putUInt(bookPositionKey(currentBookPath_).c_str(), static_cast<uint32_t>(wordIndex));
  preferences_.putUInt(bookWordCountKey(currentBookPath_).c_str(),
                       static_cast<uint32_t>(reader_.wordCount()));
  preferences_.putUInt(kPrefLegacyWordIndex, static_cast<uint32_t>(wordIndex));
  preferences_.putUShort(kPrefWpm, reader_.wpm());
  invalidateBookProgress(currentBookIndex_);
  markBookRecent(currentBookPath_);
  lastSavedWordIndex_ = wordIndex;
  Serial.printf("[app] saved position word=%u book=%s\n", static_cast<unsigned int>(wordIndex),
                currentBookPath_.c_str());
}

bool App::loadBookAtIndex(size_t index, uint32_t nowMs, bool allowLegacyPositionFallback)
{
  // Free the previous book's word vector NOW so parseFile has the full PSRAM
  // budget. Otherwise both old and new vectors coexist during parse, and a
  // second large-book load OOM-crashes mid-parse (PSRAM tops out at 8 MB).
  reader_.releaseWords();
  // Lazy split — if the .rsvp at this index is too big to load whole, run
  // splitOversizedRsvp now (with progress UI) and reindex so the user sees
  // the part files in the library. Threshold matches the splitter's internal
  // byte-size heuristic (~1.8 MB ≈ 200k words).
  constexpr size_t kSplitByteThreshold = 1800000;
  const String pathAtIndex = storage_.bookPath(index);
  if (!pathAtIndex.isEmpty() && pathAtIndex.indexOf(".part") < 0)
  {
    File probe = SD_MMC.open(pathAtIndex);
    if (probe && !probe.isDirectory() && probe.size() > kSplitByteThreshold)
    {
      const uint32_t origSize = probe.size();
      probe.close();
      Serial.printf("[lazy-split] %s is %lu bytes, splitting...\n",
                    pathAtIndex.c_str(), static_cast<unsigned long>(origSize));
      display_.renderProgress("Book", "Too big — splitting", "this may take a minute", 5);
      const bool splitOk = storage_.splitOversizedRsvp(pathAtIndex);
      Serial.printf("[lazy-split] splitOversizedRsvp returned %d\n", splitOk ? 1 : 0);
      Serial.printf("[lazy-split] original still exists? %d\n",
                    SD_MMC.exists(pathAtIndex) ? 1 : 0);
      storage_.refreshBooks();
      bookProgressInfo_.clear();
      // After split the original is gone; locate part 1 by name.
      String basePath = pathAtIndex;
      if (basePath.endsWith(".rsvp")) basePath = basePath.substring(0, basePath.length() - 5);
      const String part1Path = basePath + ".part1.rsvp";
      Serial.printf("[lazy-split] looking for %s\n", part1Path.c_str());
      Serial.printf("[lazy-split] part1 exists on SD? %d\n",
                    SD_MMC.exists(part1Path) ? 1 : 0);
      int newIndex = findBookIndexByPath(part1Path);
      Serial.printf("[lazy-split] findBookIndexByPath returned %d\n", newIndex);
      if (newIndex < 0)
      {
        // List all paths in storage for diagnostics
        for (size_t i = 0; i < storage_.bookCount(); ++i)
        {
          const String p = storage_.bookPath(i);
          if (p.indexOf("part") >= 0 || p.indexOf("odel") >= 0 || p.indexOf("scher") >= 0)
          {
            Serial.printf("[lazy-split]   storage[%u]: %s\n",
                          static_cast<unsigned>(i), p.c_str());
          }
        }
        display_.renderStatus("Book", "Split failed",
                              splitOk ? "part1 not indexed" : "splitter returned false");
        delay(2500);
        return false;
      }
      index = static_cast<size_t>(newIndex);
    }
    else if (probe)
    {
      probe.close();
    }
  }

  BookContent book;
  String loadedPath;
  size_t loadedIndex = index;
  if (!storage_.loadBookContent(index, book, &loadedPath, &loadedIndex))
  {
    return false;
  }

  chapterMarkers_ = std::move(book.chapters);
  paragraphStarts_ = std::move(book.paragraphStarts);
  reader_.setWords(std::move(book.words), nowMs);
  currentBookIndex_ = loadedIndex;
  currentBookPath_ = loadedPath;
  currentBookTitle_ = book.title.isEmpty() ? displayNameForPath(loadedPath) : book.title;
  lastSavedWordIndex_ = static_cast<size_t>(-1);
  usingStorageBook_ = true;
  bookMetaOnly_ = false;

  std::vector<float> chapterFractions;
  const size_t totalWords = reader_.wordCount();
  if (totalWords > 0)
  {
    chapterFractions.reserve(chapterMarkers_.size());
    for (const ChapterMarker &marker : chapterMarkers_)
    {
      chapterFractions.push_back(static_cast<float>(marker.wordIndex) /
                                 static_cast<float>(totalWords));
    }
  }
  display_.setChapterFractions(chapterFractions);
  preferences_.putString(kPrefBookPath, currentBookPath_);
  preferences_.putUInt(bookWordCountKey(currentBookPath_).c_str(),
                       static_cast<uint32_t>(reader_.wordCount()));
  markBookRecent(currentBookPath_);

  const uint32_t savedWordIndex =
      savedWordIndexForBook(currentBookPath_, allowLegacyPositionFallback);
  if (savedWordIndex != kNoSavedWordIndex)
  {
    reader_.seekTo(savedWordIndex);
    lastSavedWordIndex_ = reader_.currentIndex();
    Serial.printf("[app] restored book position word=%u key=%s\n",
                  static_cast<unsigned int>(reader_.currentIndex()),
                  bookPositionKey(currentBookPath_).c_str());
  }

  lastProgressSaveMs_ = nowMs;
  Serial.printf("[app] loaded SD book[%u/%u]: %s (%u chapters, %u paragraphs)\n",
                static_cast<unsigned int>(loadedIndex + 1),
                static_cast<unsigned int>(storage_.bookCount()), loadedPath.c_str(),
                static_cast<unsigned int>(chapterMarkers_.size()),
                static_cast<unsigned int>(paragraphStarts_.size()));
  return true;
}

String App::bookPositionKey(const String &bookPath) const
{
  char key[10];
  std::snprintf(key, sizeof(key), "p%08lx", static_cast<unsigned long>(hashBookPath(bookPath)));
  return String(key);
}

String App::bookWordCountKey(const String &bookPath) const
{
  char key[10];
  std::snprintf(key, sizeof(key), "c%08lx", static_cast<unsigned long>(hashBookPath(bookPath)));
  return String(key);
}

String App::bookRecentKey(const String &bookPath) const
{
  char key[10];
  std::snprintf(key, sizeof(key), "r%08lx", static_cast<unsigned long>(hashBookPath(bookPath)));
  return String(key);
}

uint32_t App::nextRecentSequence()
{
  uint32_t sequence = preferences_.getUInt(kPrefRecentSeq, 0);
  if (sequence == 0xFFFFFFFEUL)
  {
    sequence = 0;
  }
  ++sequence;
  preferences_.putUInt(kPrefRecentSeq, sequence);
  return sequence;
}

uint32_t App::bookRecentSequence(const String &bookPath)
{
  return preferences_.getUInt(bookRecentKey(bookPath).c_str(), 0);
}

uint32_t App::bookRecentSequenceByIndex(size_t bookIndex)
{
  return progressInfoFor(bookIndex).recentSequence;
}

void App::markBookRecent(const String &bookPath)
{
  if (bookPath.isEmpty())
  {
    return;
  }

  preferences_.putUInt(bookRecentKey(bookPath).c_str(), nextRecentSequence());
  const int idx = findBookIndexByPath(bookPath);
  if (idx >= 0)
  {
    invalidateBookProgress(static_cast<size_t>(idx));
  }
}

void App::ensureBookProgressInfoSize()
{
  const size_t count = storage_.bookCount();
  if (bookProgressInfo_.size() != count)
  {
    bookProgressInfo_.assign(count, BookProgressInfo{});
  }
}

void App::invalidateBookProgress(size_t bookIndex)
{
  if (bookIndex < bookProgressInfo_.size())
  {
    bookProgressInfo_[bookIndex] = BookProgressInfo{};
  }
}

const App::BookProgressInfo &App::progressInfoFor(size_t bookIndex)
{
  static BookProgressInfo empty;
  ensureBookProgressInfoSize();
  if (bookIndex >= bookProgressInfo_.size())
  {
    return empty;
  }
  BookProgressInfo &info = bookProgressInfo_[bookIndex];
  if (info.valid)
  {
    return info;
  }
  const String path = storage_.bookPath(bookIndex);
  if (path.isEmpty())
  {
    info.valid = true;
    return info;
  }
  info.recentSequence = preferences_.getUInt(bookRecentKey(path).c_str(), 0);
  const String posKey = bookPositionKey(path);
  const String countKey = bookWordCountKey(path);
  if (preferences_.isKey(posKey.c_str()) && preferences_.isKey(countKey.c_str()))
  {
    info.savedWordIndex = preferences_.getUInt(posKey.c_str(), 0);
    info.savedWordCount = preferences_.getUInt(countKey.c_str(), 0);
    info.hasPosition = true;
  }
  info.valid = true;
  return info;
}

uint32_t App::savedWordIndexForBook(const String &bookPath, bool allowLegacyFallback)
{
  const String key = bookPositionKey(bookPath);
  if (preferences_.isKey(key.c_str()))
  {
    return preferences_.getUInt(key.c_str(), 0);
  }

  if (allowLegacyFallback && preferences_.isKey(kPrefLegacyWordIndex))
  {
    const uint32_t legacyWordIndex = preferences_.getUInt(kPrefLegacyWordIndex, 0);
    preferences_.putUInt(key.c_str(), legacyWordIndex);
    Serial.printf("[app] migrated legacy position word=%u to key=%s\n",
                  static_cast<unsigned int>(legacyWordIndex), key.c_str());
    return legacyWordIndex;
  }

  return kNoSavedWordIndex;
}

bool App::bookProgressPercent(size_t bookIndex, uint8_t &percent)
{
  size_t wordIndex = 0;
  size_t wordCount = 0;

  if (usingStorageBook_ && bookIndex == currentBookIndex_)
  {
    wordIndex = reader_.currentIndex();
    wordCount = reader_.wordCount();
  }
  else
  {
    const BookProgressInfo &info = progressInfoFor(bookIndex);
    if (!info.hasPosition)
    {
      return false;
    }
    wordIndex = info.savedWordIndex;
    wordCount = info.savedWordCount;
  }

  if (wordCount <= 1)
  {
    return false;
  }

  wordIndex = std::min(wordIndex, wordCount - 1);
  const size_t progress = (wordIndex * static_cast<size_t>(100)) / (wordCount - 1);
  percent = static_cast<uint8_t>(std::min(static_cast<size_t>(100), progress));
  return true;
}

int App::findBookIndexByPath(const String &path) const
{
  for (size_t i = 0; i < storage_.bookCount(); ++i)
  {
    if (storage_.bookPath(i) == path)
    {
      return static_cast<int>(i);
    }
  }
  return -1;
}

void App::renderMenu()
{
  if (menuScreen_ == MenuScreen::SettingsHome || menuScreen_ == MenuScreen::SettingsDisplay ||
      menuScreen_ == MenuScreen::SettingsPacing || menuScreen_ == MenuScreen::SettingsReadingSounds)
  {
    renderSettings();
  }
  else if (menuScreen_ == MenuScreen::TypographyTuning)
  {
    renderTypographyTuning();
  }
  else if (menuScreen_ == MenuScreen::BookPicker)
  {
    renderBookPicker();
  }
  else if (menuScreen_ == MenuScreen::AuthorPicker)
  {
    renderAuthorPicker();
  }
  else if (menuScreen_ == MenuScreen::ChapterPicker)
  {
    renderChapterPicker();
  }
  else if (menuScreen_ == MenuScreen::TonePicker)
  {
    renderTonePicker();
  }
  else if (menuScreen_ == MenuScreen::RemoteBookPicker)
  {
    renderRemoteBookPicker();
  }
  else if (menuScreen_ == MenuScreen::NetworkPicker)
  {
    renderNetworkPicker();
  }
  else if (menuScreen_ == MenuScreen::BookDeleteConfirm)
  {
    renderBookDeleteConfirm();
  }
  else if (menuScreen_ == MenuScreen::RestartConfirm)
  {
    renderRestartConfirm();
  }
  else if (menuScreen_ == MenuScreen::DemoPicker)
  {
    renderDemoPicker();
  }
  else
  {
    renderMainMenu();
  }
}

void App::renderMainMenu()
{
  String accentTitle;
  std::vector<String> accentChips;
  const bool haveBookContext =
      (usingStorageBook_ || bookMetaOnly_) && !currentBookTitle_.isEmpty();
  if (haveBookContext)
  {
    accentTitle = currentBookTitle_;

    size_t total = 0;
    size_t cur = 0;
    if (usingStorageBook_)
    {
      total = reader_.wordCount();
      cur = reader_.currentIndex();
    }
    else
    {
      const BookProgressInfo &info = progressInfoFor(currentBookIndex_);
      total = info.savedWordCount;
      cur = info.savedWordIndex;
    }
    if (total > 0 && cur <= total)
    {
      const uint32_t pct = (static_cast<uint64_t>(cur) * 100ULL) / total;
      if (pct > 0)
      {
        accentChips.push_back(String(pct) + "%");
      }
    }
    const uint16_t wpm = reader_.wpm();
    if (total > cur && wpm > 0)
    {
      const uint32_t remainingWords = static_cast<uint32_t>(total - cur);
      const uint32_t totalMin = (remainingWords + wpm / 2) / wpm;
      const uint32_t hours = totalMin / 60;
      const uint32_t mins = totalMin % 60;
      String t;
      if (hours > 0)
      {
        t = String(hours) + "h";
        if (hours < 10 && mins > 0)
        {
          t += String(mins) + "m";
        }
      }
      else
      {
        t = String(std::max<uint32_t>(1, mins)) + "m";
      }
      accentChips.push_back(t);
    }
  }
  std::vector<bool> chevrons(MenuItemCount, false);
  chevrons[MenuResumeFrom] = true;
  chevrons[MenuChapters] = true;
  chevrons[MenuChangeBook] = true;
  chevrons[MenuDownloadBooks] = true;
  chevrons[MenuSettings] = true;
  chevrons[MenuRestart] = true;
  display_.renderMenuWithAccent(kMenuItems, MenuItemCount, menuSelectedIndex_, MenuResume,
                                accentTitle, accentChips, chevrons);
}

void App::renderSettings()
{
  if (settingsMenuItems_.empty())
  {
    rebuildSettingsMenuItems();
  }
  std::vector<bool> chevrons(settingsMenuItems_.size(), false);
  if (menuScreen_ == MenuScreen::SettingsHome)
  {
    if (settingsMenuItems_.size() > kSettingsHomeDisplayIndex)
    {
      chevrons[kSettingsHomeDisplayIndex] = true;
    }
    if (settingsMenuItems_.size() > kSettingsHomeTypographyIndex)
    {
      chevrons[kSettingsHomeTypographyIndex] = true;
    }
    if (settingsMenuItems_.size() > kSettingsHomePacingIndex)
    {
      chevrons[kSettingsHomePacingIndex] = true;
    }
    if (settingsMenuItems_.size() > kSettingsHomeToneIndex)
    {
      chevrons[kSettingsHomeToneIndex] = true;
    }
    if (settingsMenuItems_.size() > kSettingsHomeReadingSoundsIndex)
    {
      chevrons[kSettingsHomeReadingSoundsIndex] = true;
    }
    if (settingsMenuItems_.size() > kSettingsHomeNetworkIndex)
    {
      chevrons[kSettingsHomeNetworkIndex] = true;
    }
    if (settingsMenuItems_.size() > kSettingsHomeDemosIndex)
    {
      chevrons[kSettingsHomeDemosIndex] = true;
    }
  }
  else if (menuScreen_ == MenuScreen::SettingsDisplay)
  {
    if (settingsMenuItems_.size() > kSettingsDisplayTypographyIndex)
    {
      chevrons[kSettingsDisplayTypographyIndex] = true;
    }
  }
  display_.renderMenu(settingsMenuItems_, settingsSelectedIndex_, chevrons);
}

void App::renderTypographyTuning()
{
  if (kTypographyPreviewWordCount == 0)
  {
    display_.renderStatus("Typography", "No samples", "");
    return;
  }

  if (typographyPreviewSampleIndex_ >= kTypographyPreviewWordCount)
  {
    typographyPreviewSampleIndex_ = 0;
  }
  if (typographyTuningSelectedIndex_ >= TypographyTuningItemCount)
  {
    typographyTuningSelectedIndex_ = TypographyTuningTracking;
  }

  const size_t index = typographyPreviewSampleIndex_;
  const size_t beforeIndex =
      index == 0 ? kTypographyPreviewWordCount - 1 : index - 1;
  const size_t afterIndex =
      (index + 1 >= kTypographyPreviewWordCount) ? 0 : index + 1;
  const String line1 = typographyTuningLabel() + ": " + typographyTuningValueLabel();
  const String title =
      "Typography " + String(static_cast<unsigned int>(index + 1)) + "/" +
      String(static_cast<unsigned int>(kTypographyPreviewWordCount));
  String line2 = "Tap change  L/R sample";
  if (typographyTuningSelectedIndex_ == TypographyTuningBack)
  {
    line2 = "Tap exit  L/R sample";
  }
  else if (typographyTuningSelectedIndex_ == TypographyTuningReset)
  {
    line2 = "Tap reset  L/R sample";
  }

  display_.renderTypographyPreview(kTypographyPreviewWords[beforeIndex],
                                   kTypographyPreviewWords[index],
                                   kTypographyPreviewWords[afterIndex],
                                   readerFontSizeIndex_, title, line1, line2);
}

void App::renderBookPicker()
{
  const int focus = (letterScrubActive_ && menuScreen_ == MenuScreen::BookPicker)
                        ? letterScrubFocusIdx_
                        : -1;
  display_.renderLibrary(bookMenuItems_, bookPickerSelectedIndex_, bookPickerLetterAnchors_,
                         focus);
}

void App::renderChapterPicker()
{
  display_.renderMenu(chapterMenuItems_, chapterPickerSelectedIndex_);
}

void App::renderRestartConfirm()
{
  std::vector<String> items;
  items.reserve(sizeof(kRestartConfirmItems) / sizeof(kRestartConfirmItems[0]));
  for (const char *item : kRestartConfirmItems)
  {
    items.push_back(item);
  }

  display_.renderMenu(items, restartConfirmSelectedIndex_ + kRestartConfirmHeaderRows);
}

DisplayManager::LibraryItem App::libraryItemForBook(size_t bookIndex)
{
  DisplayManager::LibraryItem item;
  item.title = storage_.bookDisplayName(bookIndex);
  item.subtitle = storage_.bookAuthorName(bookIndex);

  uint32_t words = 0;
  uint32_t chapters = 0;
  if (storage_.bookStats(bookIndex, words, chapters))
  {
    if (words > 0)
    {
      item.wordCount = static_cast<int32_t>(words);
      const uint16_t wpm = reader_.wpm();
      if (wpm > 0)
      {
        const uint32_t totalMinutes = (words + wpm / 2) / wpm;
        const uint32_t hours = totalMinutes / 60;
        const uint32_t minutes = totalMinutes % 60;
        String timeBadge;
        if (hours > 0)
        {
          timeBadge = String(hours) + "h";
          if (hours < 10 && minutes > 0)
          {
            timeBadge += String(minutes) + "m";
          }
        }
        else
        {
          timeBadge = String(std::max<uint32_t>(1, minutes)) + "m";
        }
        item.badges.push_back(timeBadge);
      }
    }
    if (chapters > 0)
    {
      item.chapterCount = static_cast<int32_t>(chapters);
    }
  }

  uint8_t percent = 0;
  if (bookProgressPercent(bookIndex, percent))
  {
    item.progressPercent = static_cast<int8_t>(percent);
  }

  if (item.subtitle.isEmpty() && usingStorageBook_ && bookIndex == currentBookIndex_)
  {
    item.subtitle = "Current book";
  }

  return item;
}

String App::chapterMenuLabel(size_t chapterIndex) const
{
  if (chapterIndex >= chapterMarkers_.size())
  {
    return "";
  }

  String label = String(chapterIndex + 1) + " " + chapterMarkers_[chapterIndex].title;
  if (label.length() > 36)
  {
    label = label.substring(0, 36) + "...";
  }

  const size_t currentIndex = reader_.currentIndex();
  const size_t startIndex = chapterMarkers_[chapterIndex].wordIndex;
  const size_t endIndex = (chapterIndex + 1 < chapterMarkers_.size())
                              ? chapterMarkers_[chapterIndex + 1].wordIndex
                              : reader_.wordCount();
  if (currentIndex >= startIndex && currentIndex < endIndex)
  {
    label += " *";
  }
  return label;
}

String App::currentChapterLabel() const
{
  if (chapterMarkers_.empty())
  {
    return currentBookTitle_.isEmpty() ? "Start" : currentBookTitle_;
  }

  size_t currentChapter = 0;
  const size_t currentIndex = reader_.currentIndex();
  for (size_t i = 0; i < chapterMarkers_.size(); ++i)
  {
    if (chapterMarkers_[i].wordIndex <= currentIndex)
    {
      currentChapter = i;
    }
  }

  return chapterMarkers_[currentChapter].title;
}

uint8_t App::readingProgressPercent() const
{
  const size_t count = reader_.wordCount();
  if (count <= 1)
  {
    return 0;
  }

  const size_t index = std::min(reader_.currentIndex(), count - 1);
  const size_t percent = (index * 100UL) / (count - 1);
  return static_cast<uint8_t>(std::min(static_cast<size_t>(100), percent));
}

size_t App::phantomBeforeCharTarget() const
{
  uint8_t levelIndex = readerFontSizeIndex_;
  if (levelIndex >= kReaderFontSizeCount)
  {
    levelIndex = 0;
  }
  return kPhantomBeforeCharTargets[levelIndex];
}

size_t App::phantomAfterCharTarget() const
{
  uint8_t levelIndex = readerFontSizeIndex_;
  if (levelIndex >= kReaderFontSizeCount)
  {
    levelIndex = 0;
  }
  return kPhantomAfterCharTargets[levelIndex];
}

String App::collectPhantomBeforeText(size_t currentIndex, size_t charTarget) const
{
  if (currentIndex == 0 || charTarget == 0)
  {
    return "";
  }

  size_t startIndex = currentIndex;
  size_t totalChars = 0;
  while (startIndex > 0 && totalChars < charTarget)
  {
    --startIndex;
    const String word = reader_.wordAt(startIndex);
    totalChars += word.length();
    if (startIndex + 1 < currentIndex)
    {
      ++totalChars;
    }
  }

  String text;
  for (size_t index = startIndex; index < currentIndex; ++index)
  {
    if (!text.isEmpty())
    {
      text += ' ';
    }
    text += reader_.wordAt(index);
  }
  return text;
}

String App::collectPhantomAfterText(size_t currentIndex, size_t charTarget) const
{
  const size_t wordCount = reader_.wordCount();
  if (wordCount == 0 || currentIndex + 1 >= wordCount || charTarget == 0)
  {
    return "";
  }

  size_t endIndex = currentIndex + 1;
  size_t totalChars = 0;
  while (endIndex < wordCount && totalChars < charTarget)
  {
    const String word = reader_.wordAt(endIndex);
    totalChars += word.length();
    if (endIndex > currentIndex + 1)
    {
      ++totalChars;
    }
    ++endIndex;
  }

  String text;
  for (size_t index = currentIndex + 1; index < endIndex; ++index)
  {
    if (!text.isEmpty())
    {
      text += ' ';
    }
    text += reader_.wordAt(index);
  }
  return text;
}

String App::phantomBeforeText() const
{
  const size_t wordCount = reader_.wordCount();
  if (wordCount == 0)
  {
    return "";
  }

  const size_t currentIndex = std::min(reader_.currentIndex(), wordCount - 1);
  return collectPhantomBeforeText(currentIndex, phantomBeforeCharTarget());
}

String App::phantomAfterText() const
{
  const size_t wordCount = reader_.wordCount();
  if (wordCount == 0)
  {
    return "";
  }

  const size_t currentIndex = std::min(reader_.currentIndex(), wordCount - 1);
  return collectPhantomAfterText(currentIndex, phantomAfterCharTarget());
}

void App::renderReaderWord()
{
  contextViewVisible_ = false;
  const String currentWord = reader_.currentWord();
  // Skip rendering when the reader is in a transient empty state — without
  // this, the user briefly sees just the anchor-guide marks with nothing
  // between them on certain menu→reader transitions.
  if (currentWord.isEmpty())
  {
    // The render loop calls this every periodic refresh; without this guard
    // the log floods at ~kPausedRefreshIntervalMs while the reader sits in
    // its transient empty state (e.g. meta-only-loaded, no SD). Log once per
    // (idx, count) pair instead of every call.
    static uint32_t sLastIdx = UINT32_MAX;
    static uint32_t sLastCount = UINT32_MAX;
    const uint32_t idx = reader_.currentIndex();
    const uint32_t count = reader_.wordCount();
    if (idx != sLastIdx || count != sLastCount) {
      Serial.printf("[reader] renderReaderWord SKIP: currentWord empty (idx=%u count=%u)\n",
                    static_cast<unsigned>(idx), static_cast<unsigned>(count));
      sLastIdx = idx;
      sLastCount = count;
    }
    return;
  }
  const bool showFooter = state_ != AppState::Playing;
  const String beforeText = phantomWordsEnabled_ ? phantomBeforeText() : "";
  const String afterText = phantomWordsEnabled_ ? phantomAfterText() : "";
  display_.renderPhantomRsvpWord(beforeText, currentWord, afterText,
                                 readerFontSizeIndex_, currentChapterLabel(),
                                 readingProgressPercent(), showFooter);
}

bool App::isParagraphStart(size_t wordIndex) const
{
  if (wordIndex == 0)
  {
    return true;
  }

  return std::binary_search(paragraphStarts_.begin(), paragraphStarts_.end(), wordIndex);
}

size_t App::paragraphStartAtOrBefore(size_t wordIndex) const
{
  if (wordIndex == 0 || paragraphStarts_.empty())
  {
    return 0;
  }

  const auto it = std::upper_bound(paragraphStarts_.begin(), paragraphStarts_.end(), wordIndex);
  if (it == paragraphStarts_.begin())
  {
    return 0;
  }

  return *std::prev(it);
}

size_t App::contextPreviewAnchorIndex(size_t currentIndex) const
{
  if (currentIndex <= kContextPreviewAnchorLeadWords)
  {
    return 0;
  }

  const size_t anchorTarget = currentIndex - kContextPreviewAnchorLeadWords;
  const size_t paragraphStart = paragraphStartAtOrBefore(anchorTarget);
  if (anchorTarget - paragraphStart <= kContextPreviewMaxParagraphSnapWords)
  {
    return paragraphStart;
  }

  return anchorTarget;
}

void App::invalidateContextPreviewWindow() { contextPreviewWindowValid_ = false; }

void App::renderContextPreview()
{
  const size_t wordCount = reader_.wordCount();
  if (wordCount == 0)
  {
    renderReaderWord();
    return;
  }

  const size_t currentIndex = std::min(reader_.currentIndex(), wordCount - 1);
  if (!contextPreviewWindowValid_)
  {
    contextPreviewStartIndex_ = contextPreviewAnchorIndex(currentIndex);
    contextPreviewWindowValid_ = true;
  }

  size_t startIndex = contextPreviewStartIndex_;
  size_t endIndex = std::min(wordCount, startIndex + kContextPreviewWindowWords);
  if (currentIndex < startIndex || currentIndex >= endIndex)
  {
    startIndex = contextPreviewAnchorIndex(currentIndex);
    endIndex = std::min(wordCount, startIndex + kContextPreviewWindowWords);
    contextPreviewStartIndex_ = startIndex;
  }

  std::vector<DisplayManager::ContextWord> words;
  words.reserve(endIndex - startIndex);
  for (size_t index = startIndex; index < endIndex; ++index)
  {
    DisplayManager::ContextWord word;
    word.text = reader_.wordAt(index);
    word.paragraphStart = isParagraphStart(index);
    word.current = index == currentIndex;
    words.push_back(word);
  }

  contextViewVisible_ = true;
  display_.renderContextView(words, currentChapterLabel(), readingProgressPercent());
}

void App::renderWpmFeedback(uint32_t nowMs)
{
  contextViewVisible_ = false;
  wpmFeedbackVisible_ = true;
  wpmFeedbackUntilMs_ = nowMs + kWpmFeedbackMs;
  display_.setWpmHighlightUntil(wpmFeedbackUntilMs_);
  renderReaderWord();
}

void App::renderStorageStatus(const char *title, const char *line1, const char *line2,
                              int progressPercent)
{
  display_.renderProgress(title == nullptr ? "SD" : title, line1 == nullptr ? "" : line1,
                          line2 == nullptr ? "" : line2, progressPercent);
}

void App::handleStorageStatus(void *context, const char *title, const char *line1,
                              const char *line2, int progressPercent)
{
  if (context == nullptr)
  {
    return;
  }

  static_cast<App *>(context)->renderStorageStatus(title, line1, line2, progressPercent);
  delay(0);
}
