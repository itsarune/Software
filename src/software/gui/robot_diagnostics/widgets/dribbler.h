#pragma once

#include <QtWidgets/QWidget>

#include "shared/robot_constants.h"
#include "software/gui/generic_widgets/slider/slider.h"

// This include is autogenerated by the .ui file in the same folder
// The generated version will be names 'ui_<filename>.h'
#include "software/gui/robot_diagnostics/ui/ui_main_widget.h"

/**
 * Set up dribbler area of the widget
 *
 * @param widget widget to setup
 * @param dribbler_rpm_changed_callback callback for when dribbler rpm input changes
 * @param robot_constants The robot constants
 */
void setupDribbler(Ui::AutoGeneratedMainWidget *widget,
                   std::function<void(double)> dribbler_rpm_changed_callback,
                   const RobotConstants_t &robot_constants);
