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
   * @brief Pointer to the LCD instance
   */
  LiquidCrystal_PCF8574* display;

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
   * @brief Construct a vie object
   *
   * @param display pointer to the LCD instance
   */
  ViewBase(LiquidCrystal_PCF8574* display)
    : display(display) {
    if (display) {
      activate();
    }
  }
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
    : display(std::move(other.display)) {}

public:
  /**
   * @brief Destroy the View Base object
   */
  virtual ~ViewBase() {}

public:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() {
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

public:
  /**
   * @brief called during the loop function
   */
  virtual void tick() = 0;
};
} // namespace lcd
