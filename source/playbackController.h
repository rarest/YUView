/*  YUView - YUV player with advanced analytics toolset
*   Copyright (C) 2015  Institut für Nachrichtentechnik
*                       RWTH Aachen University, GERMANY
*
*   YUView is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   YUView is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with YUView.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H

#include <QBasicTimer>
#include <QPointer>
#include <QTime>
#include <QWidget>
#include "playlistTreeWidget.h"
#include "splitViewWidget.h"
#include "typedef.h"
#include "ui_playbackController.h"

class playlistItem;
class PlaylistTreeWidget;

class PlaybackController : public QWidget, private Ui::PlaybackController
{
  Q_OBJECT

public:

  /*
  */
  PlaybackController();

  void setSplitViews(splitViewWidget *primary, splitViewWidget *separate) { splitViewPrimary = primary; splitViewSeparate = separate; }
  void setPlaylist (PlaylistTreeWidget *playlistWidget) { playlist = playlistWidget; }

  // If playback is running, stop it by pressing the playPauseButton.
  void pausePlayback() { if (playing()) on_playPauseButton_clicked(); }

  // Is the playback running?
  bool playing() const { return timer.isActive(); }

  // Get the currently shown frame index
  int getCurrentFrame() const { return currentFrameIdx; }
  // Set the current frame in the controls and update the splitView without invoking more events from the controls.
  void setCurrentFrame(int frame);

public slots:
  // Slots for the play/stop/toggleRepera buttons (these are automatically connected by the UI file (connectSlotsByName))
  void on_playPauseButton_clicked();
  void on_stopButton_clicked();
  void on_repeatModeButton_clicked();

  // Slots for skipping to the next/previous frame. There could be buttons connected to these.
  void nextFrame();
  void previousFrame();

  // Accept the signal from the playlistTreeWidget that signals if a new (or two) item was selected.
  // The playback controller will save a pointer to this in order to get playback info from the item later
  // like the sampling or the framerate. This will also update the slider and the spin box.
  // Playback will be stopped if chageByPlayback is false.
  void currentSelectedItemsChanged(playlistItem *item1, playlistItem *item2, bool chageByPlayback);

  /* The properties of the currently selected item(s) changed. Update the frame sliders and toggle an update()
   * in the splitView if necessary.
  */
  void selectionPropertiesChanged(bool redraw);

signals:
  void ControllerStartCachingCurrentSelection(indexRange range);
  void ControllerRemoveFromCache(indexRange range);

private slots:
  // The user is fiddeling with the slider/spinBox controls (automatically connected)
  void on_frameSlider_valueChanged(int val);
  void on_frameSpinBox_valueChanged(int val) { on_frameSlider_valueChanged(val); }

private:

  // Enable/disable all controls
  void enableControls(bool enable);
  bool controlsEnabled;

  // The current frame index
  int currentFrameIdx;

  // Start the time if not running or update the timer interval. This is called when we jump to the next item, when the user presses 
  // play or when the rate of the current item changes.
  void startOrUpdateTimer();

  /* Set the new repeat mode and save it into the settings. Update the control.
   * Always use this function to set the new repeat mode.
  */
  typedef enum {
    RepeatModeOff,
    RepeatModeOne,
    RepeatModeAll
  } RepeatMode;
  RepeatMode repeatMode;
  void setRepeatMode(RepeatMode mode);

  QIcon iconPlay;
  QIcon iconStop;
  QIcon iconPause;
  QIcon iconRepeatOff;
  QIcon iconRepeatAll;
  QIcon iconRepeatOne;

  // The time for playback
  QBasicTimer timer;
  int    timerInterval;     // The current timer interval in milli seconds. If it changes, update the running timer.
  int    timerFPSCounter;   // Every time the timer is toggled count this up. If it reaches 50, calculate FPS.
  QTime  timerLastFPSTime;  // The last time we updated the FPS counter. Used to calculate new FPS.
  virtual void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE; // Overloaded from QObject. Called when the timer fires.

  // We keep a pointer to the currently selected item (only the first)
  QPointer<playlistItem> currentItem;

  // The playback controller has a pointer to the split view so it can toggle a redraw event when a new frame is selected.
  // This could also be done using signals/slots but the problem is that signals/slots have a small overhead. 
  // So when we are using a timer for high framerates, this is the faster option.
  QPointer<splitViewWidget> splitViewPrimary;
  QPointer<splitViewWidget> splitViewSeparate;

  // We keep a pointer to the playlist tree so we can select the next item, see if there is a next item and so on.
  QPointer<PlaylistTreeWidget> playlist;
};

#endif // PLAYBACKCONTROLLER_H
