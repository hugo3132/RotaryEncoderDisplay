/**
 * @author    Hugo3132
 * @copyright 2-clause BSD license
 */
#pragma once

#include "DialogBase.h"

namespace lcd {
/**
 * @brief Dialog with a yes and a no button
 */
class DialogYesNo : public DialogBase {
public:
  /**
   * @brief If true yes is selected.
   */
  bool yesSelected;

protected:
  /**
   * @brief If true yes was drawn last
   */
  bool lastDrawState;

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
  DialogYesNo(LiquidCrystal_PCF8574* display,
              RotaryEncoder* encoder,
              String text,
              const int& numberOfColumns,
              const int& numberOfRows)
    : DialogBase(display, encoder, text, numberOfColumns, numberOfRows)
    , yesSelected(true) {}

public:
  /**
   * @brief Copy constructor - not available
   */
  DialogYesNo(const DialogYesNo& other) = delete;

public:
  /**
   * @brief Move constructor
   */
  DialogYesNo(DialogYesNo&& other) noexcept = delete;

public:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() {
    DialogBase::activate();
    lastDrawState = !yesSelected;

    while (!encoder->getNewClick()) {
      if (yesSelected && (encoder->getDirection() == RotaryEncoder::Direction::CLOCKWISE)) {
        yesSelected = false;
      }
      if (!yesSelected && (encoder->getDirection() == RotaryEncoder::Direction::COUNTERCLOCKWISE)) {
        yesSelected = true;
      }

      if (yesSelected != lastDrawState) {
        auto space = (numberOfColumns - 9) / 3;
        if (yesSelected) {
          display->setCursor(space, 3);
          display->print(">YES<");
          display->setCursor(2 * space + 6, 3);
          display->print(" No ");
        }
        else {
          display->setCursor(space, 3);
          display->print(" YES ");
          display->setCursor(2 * space + 6, 3);
          display->print(">No<");
        }
        lastDrawState = yesSelected;
      }
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