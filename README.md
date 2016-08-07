# LiveFit Demonstration #

# Introduction #

This is a live fitting demonstration application to run during classes in order
to motivate discussion about standard projectile motion models.

# Preliminary Setup #

Before using the demonstration, there is some setup.

1. Connect the computer to the projector. Ideally, set the projector screen up
   as an extra screen (not screen mirroring).
2. If using an external camera, connect it to the machine and install that all
   appropriate drivers are installed. For example, if on a Mac, test that Photo
   Booth works with the camera to be used.
3. Note down the following measurements:
   - The FOV of the camera. This is a value in degrees, which is probably
     mentioned in the specifications for your camera, whether in the manual or
     online. For example, the PS3 eye has two FOV settings: 56deg (blue) and
     75deg (red). *If you cannot find this information, the default value of
     56deg may suffice*.
   - The **width** and **height** of the projection from the projector, in
     inches.
   - The distance from the projector screen, in inches, which you plan to throw
     the ball across. This is a **negative** number, as the negative direction
     points towards the camera.
4. Start the LiveFit application. Pop out the black projector docking widget on
   the left if it is not a floating window. Place the window on the projector
   screen and resize it so that it fills the entire projection.
5. The application has started, and should be displaying a video feed. If your
   computer has an internal webcam, it should be displayed inside of the LiveFit
   application; if not, it should have your external camera displayed. If it is
   displaying your internal webcam but you wish to use an external camera, try
   selecting a different camera under File>Camera>1 (or 2, or 3, etc.).
6. **Point your camera so that the entire projector screen is in frame**. Move
   the corners (the green circles with wedges in the video feed) so that they
   sit on the corners of the projector screen. Make sure that the correct
   corners are placed on the correct corners! The small slice of the wedge
   should cover the projector screen, and the large slice should be outside.
7. Click on the *World* tab on the settings panel on the right. Enter your
   measured projector width, height, distance of the ball-throwing plane ("ball
   Z"), and camera FOV.
8. Click on the *Video* tab. Stand out of the frame and test throwing some balls
   across the screen. You are checking that:
   - The balls are being detected. There should be blue circles and red boxes in
     the application video frame that follow the same trajectory of the balls
     you have tossed. If they are not appearing, you may need to alter some
     settings.
   - The projector display information is *not causing a feedback loop* and
     being detected as a phantom ball toss. If this happens, you may need to
     change the settings.
9. By this point, if you throw a ball across the screen, there should be
   information on the projector screen: Points should mark where the camera
   thinks the ball has passed, and after each toss a white parabola should
   indicate the predicted trajectory of the ball.
10. Now alter settings in the *Data* tab so that:
    - The appropriate amount of information is displayed for your demonstration,
      and
    - The information is visible to students in your classroom (line thickness,
      font size, mark radius, etc).
11. As you step through the demonstration, you may turn on data options here.
    See the example demonstration procedure for ideas.

# Example LiveFit demonstration procedure #

When running the demonstration in your class, the procedure is of course up to
you! Here is an example format for a typical-length calculus I class
demonstration period. This example scenario comes at a period right after
discussing solving a simultaneous system of a equations to find lines from 2
points and quadratic equations from 3, and immediately before starting a related
lab assignment on fitting trajectories to spotted data from a video file.

Setup camera, computer, projector, and settings before the class period starts.
Set program to hide fit curves, and not show any parametrization information.

1. Introduce the students to the program; demonstrate how it works: It sees
   balls in motion in front of the projector and identifies where they are.
   Throw a few balls in front of the screen slowly so they can see the marked
   trajectories. *Q*: What shape is the trajectory?
2. Remind students of the model of standard projectile motion; that x(t) is a
   linear equation and that y(t) is a quadratic equation. Turn on "mark 3
   points", and "lock fit until reset". Throw a ball in front of the screen; if
   the track is adequate (reset by clicking the button below the video and try
   again if not). Points are displayed as (t, x, y). 3 points marked like this
   are the first, highest, and most recent.
3. Use the first and last points to compute a linear x(t) function, then all 3
   points to compute a quadratic y(t) function. Turn on "show x(t) and y(t)".
   Equations displayed should be the same as worked out on the board. *Recall*
   that points displayed are not in any normal units (unknown space or time
   units) and that to the computer software, *down is the positive y direction*.
   Ask if this makes sense with regards to y(t): Does the parabola open towards
   the positive or the negative direction (it should towards the positive)?
4. Ask students what they recall the various coefficients of x(t), y(t) mean.
   + x(t) = t\*[initial x velocity] + [initial x position]:
   + y(t) = t<sup>2</sup>g/2 + t\*[initial y velocity] + [initial y position]

   What does the t<sup>2</sup> coefficient of y(t) mean (half of "acceleration
   due to gravity")? *Q* Do you think it will be different for different throws?
5. Turn off "lock fit until reset" and throw more in front of the projector.
   After each throw, ask students to record the coefficient of t<sup>2</sup>.
   After a number of "good" throws/fits, ask the students whether they agree
   that the coefficient isn't changing, and why? (The numbers should be more or
   less the same).

# Settings pane descriptions #

## World ##

- **ProjW**: Measured width of the projector screen, in inches.
- **ProjH**: Measured height of the projector screen, in inches.
- **BallZ**: Distance from the projector screen plane to the ball toss plane, in
  inches (this should be a negative number).
- **FOV**: The frame of view (FOV) of your particular camera, in degrees.

## Video ##

- **BlurSize**: Strength of the gaussian blur during preprocessing. In general,
  a larger value here will reduce some sensitivity and cause some blobs to be
  connected rather than disconnected. Possible suggestion: 10px
- **ThreshVal**: The minimum value for pixels to be considered during
  preprocessing. In general, a lower value will be *more noisy and more
  sensitive* while a higher value will be *less sensitive* but might miss
  important blobs if they are not clear enough. Possible suggestion: 7.00
- **MinRadius**: Minimum radius of a blob to be considered a "ball". Make
  higher if noise is being detected as ball, or smaller if balls are being
  ignored. Possible suggestion: 2px.
- **MaxRadius**: Maximum radius of a blob to be considered a "ball". Make higher
  if large blobs are being detected as balls (e.g. a whole moving person), or
  smaller if balls are being ignored. Possible suggestion: 30px.
- **GravConstant**: An *a priori* guess of the gravitational constant *g* in
  image coordinates, px/sec^2. This is used in tracking to help predict how
  balls will fall. Suggested value: 480px/s^2.
- **Frame**: What type of frame to display. Default is "Video," which displays
  video with blobs present after the diff/blur/thresh steps. "Video" is probably
  best here.
- **XYCovariance**: Covariance between the X and Y coordinates of a ball in
  motion for the tracker. Probably best to leave this at exp(1.00)

## Data ##

- **Lock fit until clear**: Once the camera has detected a fit (the fit curve is
  white and parametric equations are displayed), don't track any new balls or
  change any display information. **When this is enabled, use "Drop Tracking
  Points" to clear the lock and enable tracking again**. Suggested to use this
  while working examples on the board, so that data is not dropped unexpectedly.
- **Clip track to projector**: Whether or not to ignore blobs which are not in
  front of the projector screen. Suggested if you will be in the camera's frame
  so that the camera can ignore you entirely and focus on the balls.
- **Show x(t), y(t)**: Whether or not to show the parametric equations for the
  fit on the projector screen.
- **Color track misses**: Whether or not to color points on the projector yellow
  if they are not based on visual data. Suggested to leave this off during
  demonstrations.
- **Show dx/dt, dy/dt**: Whether or not to show the predicted velocity vector of
  the tracked balls on the projector.
- **Verbose KF track points**: Whether or not to show the size of the seen blobs
  on the projector screen. Suggested: Off.
- **Point radius**: Radius of the points for tracked balls on the projector.
- **Point thickness**: Thickness of the lines for tracked balls on the projector.
- **Show fit parabola**: Whether or not to show the fit curve defined by the
  parametric fit.
- **Only lock after falling**: Whether to wait until the ball has a downward
  trajectory to fit an equation. Suggested: on, since the quadratic fit might
  not be good enough otherwise...
- **Min fall speed**: Minimum speed in frame coordinates to be considered
  "falling". The larger this is, the longer it might take for a fit curve, but
  the better the curve should be. Suggested: 5px/s
- **Font size**: The font size of the display on the projector; equations,
  points, marked points.
