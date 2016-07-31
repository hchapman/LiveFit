# LiveFit Demonstration #

# Introduction #

This is a live fitting demonstration application to run during classes in order
to motivate discussion about standard projectile motion models.

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
   the track is adequate (reset and try again if not). Points are displayed as
   (t, x, y). 3 points marked like this are the first, highest, and most recent.
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
