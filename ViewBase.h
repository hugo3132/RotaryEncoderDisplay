/**
 * @author    Hugo3132
 * @copyright 2-clause BSD license
 */
#pragma once

#include <Arduino.h>
#include <LiquidCrystal_PCF8574.h>

namespace lcd {

/**
 * @brief Base class for views on the LCD display
 */
class ViewBase {
protected:
  /**
   * @brief Helper class for implementing a global backlight timeout
   */
  class BacklightTimeoutManager {
    friend class ViewBase;

  protected:
    /**
     * @brief millis value after which the display should turn off
     */
    unsigned long nextTimeout = 0;

    /**
     * @brief configured value after which amount of milliseconds the backlight
     * should be turned off
     */
    unsigned long timeout = 0;

    /**
     * @brief current state of the display
     */
    bool displayCurrentlyOn = true;

    /**
     * @brief this value is set to true as soon as the timout occurred. If this
     * is true the display is turned off no matter what the evaluation of
     * nextTimeout says. The reason for this is that millis can overflow which
     * would turn the backlight back on again.
     */
    bool timedOut = false;

  public:
    /**
     * @brief Must be called in each tick call.
     */
    void tick(LiquidCrystal_PCF8574* display) {
      // check if this class should do anything
      if (timeout != 0) {
        // check if the a timeout is active / occurred
        if (timedOut || (nextTimeout < millis())) {
          timedOut = true;
          if (displayCurrentlyOn) {
            display->setBacklight(0);
            displayCurrentlyOn = false;
          }
        }
        else {
          if (!displayCurrentlyOn) {
            display->setBacklight(1);
            displayCurrentlyOn = true;
          }
        }
      }
    }

  public:
    /**
     * @brief Must be called when the timeout should be reset.
     *
     * @return the current state of the backlight
     */
    bool delayTimeout() {
      nextTimeout = millis() + timeout;
      timedOut = false;
      return displayCurrentlyOn;
    }
  };

protected:
  /**
   * @brief Returns the singleton of the BacklightTimeoutManager.
   */
  static BacklightTimeoutManager& getBacklightTimeoutManager() {
    static BacklightTimeoutManager manager;
    return manager;
  }

protected:
  /**
   * @brief Pointer to the LCD instance
   */
  LiquidCrystal_PCF8574* display;

protected:
  /**
   * @brief The previously displayed view
   */
  ViewBase* previousView;

#pragma region Special characters
public:
  /**
   * @brief Special character for Scrollbar Top
   */
  const static uint8_t scScrollbarTop = 0;

public:
  /**
   * @brief Special character for Scrollbar Middle
   */
  const static uint8_t scScrollbarMiddle = 1;

public:
  /**
   * @brief Special character for Scrollbar Bottom
   */
  const static uint8_t scScrollbarBottom = 2;

public:
  /**
   * @brief Special character for WIFI signal strength 0 (lowest)
   */
  const static uint8_t scWifiSignal0 = 3;

public:
  /**
   * @brief Special character for WIFI signal strength 1
   */
  const static uint8_t scWifiSignal1 = 4;

public:
  /**
   * @brief Special character for WIFI signal strength 2
   */
  const static uint8_t scWifiSignal2 = 5;

public:
  /**
   * @brief Special character for WIFI signal strength 3 (best)
   */
  const static uint8_t scWifiSignal3 = 6;
#pragma endregion

public:
  /**
   * @brief Construct a view object
   *
   * @param display pointer to the LCD instance
   */
  ViewBase(LiquidCrystal_PCF8574* display)
    : display(display)
    , previousView(nullptr) {}

public:
  /**
   * @brief Copy constructor - not available
   */
  ViewBase(const ViewBase& other) = delete;

public:
  /**
   * @brief Move constructor
   */
  ViewBase(ViewBase&& other) noexcept
    : display(std::move(other.display))
    , previousView(std::move(other.previousView)) {}

public:
  /**
   * @brief Destroy the View Base object
   */
  virtual ~ViewBase() {}

public:
  /**
   * @brief Get the refernce to the singleton current view pointer
   */
  static ViewBase*& getCurrentView() {
    static ViewBase* currentViewSingleton = nullptr;
    return currentViewSingleton;
  }

public:
  /**
   * @brief activates the passed view
   */
  static void activateView(ViewBase* view) {
    if (view) {
      view->previousView = getCurrentView();
      getCurrentView() = view;
      if (view) {
        view->activate();
        getBacklightTimeoutManager().delayTimeout();
        getBacklightTimeoutManager().tick(view->display);
      }
    }
    else {
      getCurrentView() = view;
    }
  }

public:
  /**
   * @brief Set the Backlight Timeout. To disable the timeout set the value to 0
   *
   * @param timeout timeout in milli-seconds
   */
  static void setBacklightTimeout(unsigned long timeout) {
    getBacklightTimeoutManager().timeout = timeout;
  }

public:
  /**
   * @brief activates the previous view
   */
  void activatePreviousView() {
    if (previousView) {
      getCurrentView() = previousView;
      previousView->activate();
    }
  }

protected:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() = 0;

public:
  /**
   * @brief called during the loop function
   *
   * @param forceRedraw if true everything should be redrawn
   */
  virtual void tick(const bool& forceRedraw) = 0;

protected:
  /**
   * @brief writes the special characters to the display
   */
  void initializeSpecialCharacters() {
    if (display) {
      byte customChar[8];
      std::fill_n(customChar, 8, 0);
      customChar[7] = B00100;
      display->createChar(scWifiSignal0, customChar);
      customChar[4] = B00100;
      customChar[5] = B01010;
      display->createChar(scWifiSignal1, customChar);
      customChar[2] = B01110;
      customChar[3] = B10001;
      display->createChar(scWifiSignal2, customChar);
      customChar[0] = B01110;
      customChar[1] = B10001;
      display->createChar(scWifiSignal3, customChar);

      std::fill_n(customChar, 8, B10001);
      display->createChar(scScrollbarMiddle, customChar);
      customChar[0] = B00100;
      customChar[1] = B01110;
      customChar[2] = B11111;
      display->createChar(scScrollbarTop, customChar);
      std::fill_n(customChar, 5, B10001);
      customChar[5] = B11111;
      customChar[6] = B01110;
      customChar[7] = B00100;
      display->createChar(scScrollbarBottom, customChar);
    }
  }
};
} // namespace lcd
