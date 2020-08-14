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
protected:
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
   * @brief Shows the dialog modal and after closing it activates the previous
   * view again
   *
   * @param yesSelected if true yes is selected by default as soon as the the
   * dialog is displayed
   * @return true if yes was selected
   */
  bool showModal(const bool& yesSelected) {
    this->yesSelected = yesSelected;
    lcd::ViewBase::activateView(this);
    activatePreviousView();
    return this->yesSelected;
  }

protected:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() {
    DialogBase::activate();
    lastDrawState = !yesSelected;

    auto encoderClicked = encoder->getNewClick();
    while (!encoderClicked) {
      auto encoderUpdate = encoder->getDirection();
      encoderClicked = encoder->getNewClick();

      // Update the backlight timeout
      if (encoderClicked || (encoderUpdate != RotaryEncoder::Direction::NOROTATION)) {
        if (!getBacklightTimeoutManager().delayTimeout()) {
          encoderClicked = false;
          continue;
        }
      }
      getBacklightTimeoutManager().tick(display);

      if (yesSelected && (encoderUpdate == RotaryEncoder::Direction::CLOCKWISE)) {
        yesSelected = false;
      }
      else if (!yesSelected && (encoderUpdate == RotaryEncoder::Direction::COUNTERCLOCKWISE)) {
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
   *
   * @param forceRedraw if true everything should be redrawn
   */
  virtual void tick(const bool& forceRedraw) {}
};
} // namespace lcd