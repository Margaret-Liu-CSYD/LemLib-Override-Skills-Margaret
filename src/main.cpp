#include "main.h"
#include "configs.h"
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    printf("hello from the robot!\n");
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors
    // print position to brain screen
    pros::Task screen_task([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // delay to save resources
            printf("X: %.2f  Y: %.2f  Theta: %.2f\n", chassis.getPose().x, chassis.getPose().y, chassis.getPose().theta);
            pros::delay(100);
        }
    });
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    //set position to x:0, y:0, heading:0
    //toggling
    chassis.setPose(0, 0, 0);
    slide.move_absolute(600, 127);
    chassis.turnToHeading(-40, 300);
    chassis.moveToPoint(0, 10, 300,{.forwards=false});
    chassis.setPose(0, 0, 0);
    chassis.moveToPoint(0, 4, 300);
    chassis.moveToPoint(0, 10, 300,{.forwards=false});
    chassis.setPose(0, 0, 0);
    
    //going to red goal to score preload
    chassis.moveToPose(18, 18,90, 2000,{.lead = 0.6,.earlyExitRange = 3});
    chassis.waitUntilDone();
    slide.move_absolute(0, 127);
    
    pros::delay(500);
    claw.set_value(true);
    pros::delay(500);

    //backing out of red goal after scoring preload
    chassis.moveToPose(10, 18, 90, 2000,{.forwards = false,.lead = 0.6,.minSpeed=20,.earlyExitRange = 3});
    pros::lcd::print(3, "testt");
    return;
    // chassis.turnToHeading(90, 20000);
    // chassis.waitUntilDone();
    // chassis.turnToHeading(0, 20000);
    // chassis.waitUntilDone();
    // return;
    printf("got here\n");
    pros::delay(1000);
    
    chassis.moveToPoint(24, 24, 20000);
    chassis.waitUntilDone();
    return;
    chassis.turnToHeading(90, 20000);
    chassis.waitUntilDone();
    printf("turn 90 degrees\n");
    printf("X: %.2f  Y: %.2f  Theta: %.2f\n", chassis.getPose().x, chassis.getPose().y, chassis.getPose().theta);
    
    chassis.moveToPose(48, 48, 90,20000);
    chassis.waitUntilDone();
    
    chassis.turnToHeading(180, 20000);
    chassis.waitUntilDone();
    printf("turn 180 degrees\n");
    printf("X: %.2f  Y: %.2f  Theta: %.2f\n", chassis.getPose().x, chassis.getPose().y, chassis.getPose().theta);
    
    chassis.moveToPose(48, 0, 180, 20000);
    chassis.waitUntilDone();
    chassis.turnToHeading(270, 20000);

    printf("turn 270 degrees\n");
    printf("X: %.2f  Y: %.2f  Theta: %.2f\n", chassis.getPose().x, chassis.getPose().y, chassis.getPose().theta);
    
    chassis.waitUntilDone();
    chassis.moveToPose(0, 0, 270, 20000);
    chassis.waitUntilDone();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
pros::Controller controller(pros::E_CONTROLLER_MASTER);

void opcontrol() {
    // loop forever
    bool clawState = false;

    while (true) {
        // get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        // move the slide
        chassis.curvature(leftY, rightX);
        if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
            slide.move(127);    // Move lift up (max 127)
        } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
            slide.move(-127);   // Move lift down
        } else {
            slide.move(0);      // Holds the lift position (if brake mode is set)
        }
        // delay to save resources
        if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
            clawState = !clawState; // Flip the logical state
            claw.set_value(clawState);
        }
        pros::delay(25);
    }
}