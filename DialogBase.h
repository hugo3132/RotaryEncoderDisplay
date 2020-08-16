/**
 * @author    Hugo3132
 * @copyright 2-clause BSD license
 */
#pragma once

#include "ViewBase.h"

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <list>

namespace lcd {
/**
 * @brief Base class for a dialog
 */
class DialogBase : public ViewBase {
protected:
  /**
   * @brief pointer to the encoder instance
   */
  RotaryEncoder* encoder;

protected:
  /**
   * @brief the dialog text
   */
  String rows[3];

public:
  /**
   * @brief Number of display-columns
   */
  const int numberOfColumns;

public:
  /**
   * @brief Number of display-rows
   */
  const int numberOfRows;

public:
  /**
   * @brief Construct a new Dialog
   *
   * @param display pointer to the display instance
   * @param name The name of the view
   * @param encoder pointer to the encoder instance
   * @param text the dialog text
   * @param numberOfColumns number of display-columns
   * @param numberOfRows number of display-rows
   */
  DialogBase(LiquidCrystal_PCF8574* display,
             const String& name,
             RotaryEncoder* encoder,
             String text,
             const int& numberOfColumns,
             const int& numberOfRows)
    : ViewBase(display, name)
    , encoder(encoder)
    , numberOfColumns(numberOfColumns)
    , numberOfRows(numberOfRows) {
    for (int i = 0; i < numberOfRows - 1; i++) {
      auto linebreak1 = text.indexOf('\n');
      if (linebreak1 != -1) {
        rows[i] = text.substring(0, linebreak1);
        text = text.substring(linebreak1 + 1);
      }
      else {
        rows[i] = text;
        break;
      }
    }
  }

public:
  /**
   * @brief Copy constructor - not available
   */
  DialogBase(const DialogBase& other) = delete;

public:
  /**
   * @brief Move constructor
   */
  DialogBase(DialogBase&& other) noexcept = delete;

protected:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() {
    display->clear();
    display->setCursor(0, 0);
    display->print(rows[0]);
    display->setCursor(0, 1);
    display->print(rows[1]);
    display->setCursor(0, 2);
    display->print(rows[2]);
  }
};
} 