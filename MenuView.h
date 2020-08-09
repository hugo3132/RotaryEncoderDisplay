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
 * @brief View which can be used for menus
 */
class MenuView : public ViewBase {
public:
  /**
   * @brief Class providing scrolling capabilities for a string which is longer
   * than the display.
   */
  class LongEntry {
  protected:
    /**
     * @brief position of the first shown character
     */
    size_t showPosition;

  protected:
    /**
     * @brief If true the animation is currently scrolling forwards
     */
    bool scrollForwards;

  protected:
    /**
     * @brief The text which should be displayed
     */
    const String text;

  public:
    /**
     * @brief Construct a new Long Entry object
     *
     * @param text the text which should be displayed
     */
    LongEntry(const String& text)
      : showPosition(0)
      , scrollForwards(false)
      , text(text) {}

  public:
    /**
     * @brief Copy constructor - not available
     */
    LongEntry(const LongEntry& other) = delete;

  public:
    /**
     * @brief Move constructor
     */
    LongEntry(LongEntry&& other) noexcept
      : showPosition(std::move(other.showPosition))
      , scrollForwards(std::move(other.scrollForwards))
      , text(std::move(other.text)) {}

  public:
    /**
     * @brief moves the string by one position if necessary
     *
     * @param maxLength maximum number of characters which should be displayed
     */
    void animationTick(const size_t& maxLength) {
      if (text.length() > maxLength) {
        if (scrollForwards) {
          if (text.length() - showPosition <= maxLength) {
            scrollForwards = false;
          }
          else {
            showPosition++;
          }
        }
        else {
          if (showPosition == 0) {
            scrollForwards = true;
          }
          else {
            showPosition--;
          }
        }
      }
    }

  public:
    /**
     * @brief Resets the animation to its initial state
     */
    void resetAnimation() {
      showPosition = 0;
      scrollForwards = false;
    }

  public:
    /**
     * @brief redraws the text
     *
     * @param display pointer to the lcd to which the text should be sent
     * @param maxLength maximum number of characters which should be displayed
     * @param fullRedraw if true trailing spaces are drawn if the text is too short.
     */
    virtual void show(LiquidCrystal_PCF8574* display, const size_t& maxLength, const bool& fullRedraw) {
      if (text.length() <= maxLength) {
        display->write(text.c_str());
        if (fullRedraw) {
          for (size_t i = text.length(); i < maxLength; i++) {
            display->write(' ');
          }
        }
      }
      else {
        display->write(text.c_str() + showPosition, maxLength);
      }
    }

  public:
    /**
     * @brief Get the text of the item
     */
    const String& getText() const {
      return text;
    }
  };

public:
  /**
   * @brief Class handling one menu entry with a callback function
   */
  class MenuItem : public LongEntry {
  public:
    /**
     * @brief Callback function which is called as soon as the item is selected
     */
    const std::function<void(MenuItem*)> callback;

  public:
    /**
     * @brief Creates a new item
     *
     * @param text text of the menu item
     * @param callback callback as soon as the item is selected
     */
    MenuItem(const String& text, const std::function<void(MenuItem*)>& callback)
      : LongEntry(text)
      , callback(callback) {}

  public:
    /**
     * @brief Copy constructor - not available
     */
    MenuItem(const MenuItem& other) = delete;

  public:
    /**
     * @brief Move constructor
     */
    MenuItem(MenuItem&& other) noexcept
      : LongEntry(std::move(other))
      , callback(std::move(other.callback)) {}
  };

protected:
  /**
   * @brief millis value of the last redraw
   */
  unsigned long lastMillisForAnimationRefresh;

protected:
  /**
   * @brief pointer to the encoder instance
   */
  RotaryEncoder* encoder;

protected:
  /**
   * @brief the menu title
   */
  LongEntry title;

protected:
  /**
   * @brief List of menu items
   */
  std::list<MenuItem> menuItems;

protected:
  /**
   * @brief Currently selected menu item
   */
  int selection;

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
   * @brief Number of display-rows used for items
   */
  const int numberOfRowsUsedForItems;

public:
  /**
   * @brief Construct a vie object
   *
   * @param display pointer to the display instance
   * @param encoder pointer to the encoder instance
   * @param title the menu title
   * @param numberOfColumns number of display-columns
   * @param numberOfRows number of display-rows
   */
  MenuView(LiquidCrystal_PCF8574* display,
           RotaryEncoder* encoder,
           const String& title,
           const int& numberOfColumns,
           const int& numberOfRows)
    : ViewBase(display)
    , encoder(encoder)
    , title(title)
    , selection(0)
    , numberOfColumns(numberOfColumns)
    , numberOfRows(numberOfRows)
    , numberOfRowsUsedForItems(((numberOfRows > 1) && (title.length() != 0)) ? numberOfRows - 1 : numberOfRows) {}

public:
  /**
   * @brief Copy constructor - not available
   */
  MenuView(const MenuView& other) = delete;

public:
  /**
   * @brief Move constructor
   */
  MenuView(MenuView&& other) noexcept
    : ViewBase(std::move(other))
    , lastMillisForAnimationRefresh(std::move(other.lastMillisForAnimationRefresh))
    , encoder(std::move(other.encoder))
    , title(std::move(other.title))
    , menuItems(std::move(other.menuItems))
    , selection(std::move(other.selection))
    , numberOfColumns(other.numberOfColumns)
    , numberOfRows(other.numberOfRows)
    , numberOfRowsUsedForItems(other.numberOfRowsUsedForItems) {}

public:
  /**
   * @brief called as soon as the view becomes active
   */
  virtual void activate() {
    // copy special characters to display
    ViewBase::activate();
    display->clear();

    if (((int)menuItems.size() > numberOfRowsUsedForItems) && (numberOfRows > 1)) {
      // draw a scrollbar
      if (numberOfRows == 2) {
        display->setCursor(numberOfColumns - 1, 0);
        display->write(scScrollbarTop);
        display->setCursor(numberOfColumns - 1, 1);
        display->write(scScrollbarBottom);
        display->setCursor(0, 0);
        title.show(display, numberOfColumns - 1, true);
      }
      else if (numberOfRows == 4) {
        if (numberOfRows == numberOfRowsUsedForItems) {
          display->setCursor(numberOfColumns - 1, 0);
          display->write(scScrollbarTop);
          display->setCursor(numberOfColumns - 1, 1);
          display->write(scScrollbarMiddle);
        }
        else {
          display->setCursor(numberOfColumns - 1, 1);
          display->write(scScrollbarTop);
          display->setCursor(0, 0);
          title.show(display, numberOfColumns, true);
        }
        display->setCursor(numberOfColumns - 1, 2);
        display->write(scScrollbarMiddle);
        display->setCursor(numberOfColumns - 1, 3);
        display->write(scScrollbarBottom);
      }
    }
    lastMillisForAnimationRefresh = millis();
    selection = 0;
  }

public:
  /**
   * @brief called during the loop function
   */
  virtual void tick() {
    const bool animationTickRequired = lastMillisForAnimationRefresh + 500 <= millis();
    auto encoderUpdate = encoder->getDirection();
    bool fullRedraw = false;
    bool redraw = animationTickRequired;

    // check if the scrollbar is visible
    size_t maxLength = numberOfColumns - 1;
    if (((int)menuItems.size() > numberOfRowsUsedForItems) && (numberOfRows > 1)) {
      maxLength = numberOfColumns - 2;
    }

    // check if we have to update the selection
    if ((encoderUpdate == RotaryEncoder::Direction::CLOCKWISE) && (selection + 1 < (int)menuItems.size())) {
      selection++;
      redraw = true;
      fullRedraw = selection % numberOfRowsUsedForItems == 0; // New page displayed
    }
    else if ((encoderUpdate == RotaryEncoder::Direction::COUNTERCLOCKWISE) && (selection != 0)) {
      selection--;
      redraw = true;
      fullRedraw = selection % numberOfRowsUsedForItems == 2; // New page displayed
    }

    // Update name of the Menu
    if (animationTickRequired && (numberOfRows != numberOfRowsUsedForItems)) {
      display->setCursor(0, 0);
      if (numberOfRows == 2) {
        title.animationTick(numberOfColumns - 1);
        title.show(display, numberOfColumns - 1, false);
      }
      else if (numberOfRows == 4) {
        title.animationTick(numberOfColumns);
        title.show(display, numberOfColumns, false);
      }
    }

    // redraw menu entries if necessary
    if (redraw) {
      // select the first entry to be shown
      auto itEntry = menuItems.begin();
      std::advance(itEntry, selection - (selection % numberOfRowsUsedForItems));

      // We have three lines left for the menu items
      for (int i = 0; i < numberOfRowsUsedForItems; i++) {
        display->setCursor(0, i + numberOfRows - numberOfRowsUsedForItems);

        if (itEntry != menuItems.end()) {
          // Is the item selected?
          if (i == selection % numberOfRowsUsedForItems) {
            display->print('>');
          }
          else {
            display->print(' ');
          }

          // Is currently a new page displayed
          if (fullRedraw) {
            // Yes start the animation of the item from the beginning
            itEntry->resetAnimation();
          }
          else if (animationTickRequired) {
            // The animation must be updated
            itEntry->animationTick(maxLength);
          }

          // draw the menu item
          itEntry->show(display, maxLength, fullRedraw);

          // select the next menu item for the next iteration
          itEntry++;
        }
        else {
          // Not enough items to be displayed, just clear the line
          if (fullRedraw) {
            for (size_t j = 0; j < maxLength + 1; j++) {
              display->print(' ');
            }
          }
        }
      }
    }

    // check if a menu entry was selected
    if (encoder->getNewClick()) {
      auto itEntry = menuItems.begin();
      std::advance(itEntry, selection);
      itEntry->callback(&*itEntry);
    }

    if (animationTickRequired) {
      lastMillisForAnimationRefresh = millis();
    }
  }

public:
  /**
   * @brief Add a new menu item
   *
   * @param text text of the menu item
   * @param callback callback as soon as the item is selected
   */
  void createMenuItem(
    const String& text,
    const std::function<void(MenuItem*)>& callback = [](MenuItem* item) {}) {
    menuItems.push_back(MenuItem(text, callback));
  }
};
} // namespace lcd