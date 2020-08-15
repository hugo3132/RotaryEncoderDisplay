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
class DialogYesNoBack : public DialogBase {
public:
  /**
   * @brief Possible selection options
   */
  enum class DialogResult { yes, no, back };

protected:
  /**
   * @brief The current selection
   */
  DialogResult selection;

protected:
  /**
   * @brief If true yes was drawn last
   */
  DialogResult lastDrawState;

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
  DialogYesNoBack(LiquidCrystal_PCF8574* display,
                  RotaryEncoder* encoder,
                  String text,
                  const int& numberOfColumns,
                  const int& numberOfRows)
    : DialogBase(display, encoder, text, numberOfColumns, numberOfRows)
    , selection(DialogResult::yes) {}

public:
  /**
   * @brief Copy constructor - not available
   */
  DialogYesNoBack(const DialogYesNoBack& other) = delete;

public:
  /**
   * @brief Move constructor
   */
  DialogYesNoBack(DialogYesNoBack&& other) noexcept = delete;

public:
  /**
   * @brief Shows the dialog modal and after closing it activates the previous
   * view again
   *
   * @param yesSelected if true yes is selected by default as soon as the the
   * dialog is displayed
   * @return true if yes was selected
   */
  DialogResult showModal(const DialogResult& defaultSelection) {
    this->selection = defaultSelection;
    lcd::ViewBase::activateView(this);
    activatePreviousView();
    return this->selection;
  }

protected:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() {
    DialogBase::activate();
    lastDrawState = (DialogResult)((int)selection + 2);

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

      if (encoderUpdate == RotaryEncoder::Direction::CLOCKWISE) {
        if (selection == DialogResult::yes) {
          selection = DialogResult::no;
        }
        else if (selection == DialogResult::no) {
          selection = DialogResult::back;
        }
      }
      else if (encoderUpdate == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
        if (selection == DialogResult::back) {
          selection = DialogResult::no;
        }
        else if (selection == DialogResult::no) {
          selection = DialogResult::yes;
        }
      }

      if (selection != lastDrawState) {
        switch (selection) {
        case DialogResult::yes:
          display->setCursor(1, 3);
          display->print(">yes<");
          display->setCursor(7, 3);
          display->print(" no ");
          display->setCursor(12, 3);
          display->print(" back ");
          break;
        case DialogResult::no:
          display->setCursor(1, 3);
          display->print(" yes ");
          display->setCursor(7, 3);
          display->print(">no<");
          display->setCursor(12, 3);
          display->print(" back ");
          break;
        case DialogResult::back:
          display->setCursor(1, 3);
          display->print(" yes ");
          display->setCursor(7, 3);
          display->print(" no ");
          display->setCursor(12, 3);
          display->print(">back<");
          break;
        }

        lastDrawState = selection;
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