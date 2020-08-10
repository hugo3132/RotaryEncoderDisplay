/**
 * @author    Hugo3132
 * @copyright 2-clause BSD license
 */
#pragma once

#include "DialogBase.h"

namespace lcd {
/**
 * @brief Dialog with a OK button
 */
class DialogOk : public DialogBase {
public:
  /**
   * @brief Construct a new Dialog
   *
   * @param display pointer to the display instance
   * @param encoder pointer to the encoder instance
   * @param text the dialog text
   * @param numberOfColumns number of display-columns
   * @param numberOfRows number of display-rows
   */
  DialogOk(LiquidCrystal_PCF8574* display,
           RotaryEncoder* encoder,
           String text,
           const int& numberOfColumns,
           const int& numberOfRows)
    : DialogBase(display, encoder, text, numberOfColumns, numberOfRows) {}

public:
  /**
   * @brief Copy constructor - not available
   */
  DialogOk(const DialogOk& other) = delete;

public:
  /**
   * @brief Move constructor
   */
  DialogOk(DialogOk&& other) noexcept = delete;

public:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() {
    DialogBase::activate();

    display->setCursor((numberOfColumns - 4) / 2, 3);
    display->print(">OK<");

    while (!encoder->getNewClick()) {
      delay(100);
    }
  }

protected:
  /**
   * @brief called during the loop function
   */
  virtual void tick() {}
};
} // namespace lcd